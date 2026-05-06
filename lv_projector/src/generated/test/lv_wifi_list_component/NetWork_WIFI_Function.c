#include "NetWork_WIFI_Function.h"

// 标准库头文件（已包含的保留，补充必要的）
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <unistd.h>
#include <stdio.h>   // 必须：printf/fprintf/perror依赖
#include <errno.h>   // 可选：如果要用到errno（系统错误码）
#include <sys/wait.h>

static int wifi_run_argv(char *const argv[])
{
    pid_t pid;
    int status;

    if (argv == NULL || argv[0] == NULL) {
        return -1;
    }

    pid = fork();
    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        execvp(argv[0], argv);
        _exit(127);
    }

    if (waitpid(pid, &status, 0) < 0) {
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

// =============================================================================
// 功能：连接WIFI网络
// =============================================================================
bool NetWork_WIFI_Connect(const char *ssid, const char *password)
{
    if (ssid == NULL || password == NULL) {
        // 错误信息用fprintf(stderr)，区分普通输出
        fprintf(stderr, "[ERROR] WIFI连接参数错误：SSID或密码为空\r\n");
        return false;
    }
    
    if (strlen(ssid) == 0 || strlen(password) == 0) {
        fprintf(stderr, "[ERROR] WIFI连接参数错误：SSID或密码为空字符串\r\n");
        return false;
    }
    
    // 普通调试信息用printf，加[DEBUG]前缀
    printf("[DEBUG] 开始连接WIFI: SSID=%s\r\n", ssid);
    
    // 1. 启动wifi守护进程
    int ret = system("wifi_daemon &");
    if (ret != 0) {
        fprintf(stderr, "[WARN] 启动wifi守护进程失败，返回码: %d（可能已运行）\r\n", ret);
    } else {
        printf("[DEBUG] wifi守护进程已启动\r\n");
    }
    
    usleep(500000);
    
    // 2. 设置station模式
    ret = system("wifi -o sta");
    if (ret != 0) {
        fprintf(stderr, "[WARN] 设置WIFI为station模式失败，返回码: %d（可能已设置）\r\n", ret);
    } else {
        printf("[DEBUG] WIFI已设置为station模式\r\n");
    }
    
    usleep(300000);
    
    /*
     * Security:
     * Use fork/exec argv instead of system("wifi -c ssid password").
     * SSID/password are passed as argv values, not parsed by shell, so shell
     * metacharacters cannot inject extra commands.
     */
    char *const connect_argv[] = {
        "wifi",
        "-c",
        (char *)ssid,
        (char *)password,
        NULL
    };

    printf("[DEBUG] 执行WIFI连接命令: wifi -c <ssid> <password>\r\n");
    ret = wifi_run_argv(connect_argv);
    
    if (ret != 0) {
        fprintf(stderr, "[ERROR] WIFI连接命令执行失败，返回码: %d\r\n", ret);
        return false;
    }
    
    printf("[DEBUG] WIFI连接命令执行成功，等待连接结果...\r\n");
    usleep(2000000);
    
    printf("[DEBUG] WIFI连接流程完成\r\n");
    return true;
}

// =============================================================================
// 内部工具：通过 ifconfig 判断 wlan0 是否已经拿到 IPv4
// =============================================================================
static bool NetWork_WIFI_HasIPv4ByCmd(void)
{
    FILE *fp = popen("ifconfig wlan0 2>/dev/null", "r");
    if (!fp) return false;

    char line[256];
    bool ok = false;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "inet addr:") || strstr(line, "inet ")) {
            if (!strstr(line, "127.0.0.1") && !strstr(line, "0.0.0.0")) {
                ok = true;
                break;
            }
        }
    }

    pclose(fp);
    return ok;
}

// =============================================================================
// 功能：检查WIFI连接状态（补充perror示例）
// =============================================================================
bool NetWork_WIFI_IsConnected(void)
{
    struct ifaddrs *ifaddr, *ifa;
    bool connected = false;
    
    // 系统调用失败时，用perror打印错误原因
    if (getifaddrs(&ifaddr) == -1) {
        perror("[ERROR] 获取网络接口信息失败"); // 自动拼接errno描述
        return false;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        
        if (strcmp(ifa->ifa_name, "wlan0") == 0) {
            /* 部分嵌入式驱动 IFF_RUNNING 不稳定，这里只要求接口 UP 且存在有效 IPv4。 */
            if (ifa->ifa_flags & IFF_UP) {
                if (ifa->ifa_addr->sa_family == AF_INET) {
                    struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
                    if (sin->sin_addr.s_addr != INADDR_ANY && sin->sin_addr.s_addr != 0) {
                        char ip_str[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &(sin->sin_addr), ip_str, INET_ADDRSTRLEN);
                        printf("[DEBUG] WIFI已连接，IP地址: %s\r\n", ip_str);
                        connected = true;
                        break;
                    }
                }
            }
        }
    }
    
    freeifaddrs(ifaddr);

    if (!connected && NetWork_WIFI_HasIPv4ByCmd()) {
        printf("[DEBUG] WIFI已连接，ifconfig 检测到 wlan0 IPv4\r\n");
        connected = true;
    }

    return connected;
}
// =============================================================================
// 内部工具：去掉首尾空白
// =============================================================================
static void wifi_trim_inplace(char *s)
{
    if (!s) return;

    char *p = s;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
        p++;
    }

    if (p != s) {
        memmove(s, p, strlen(p) + 1);
    }

    size_t len = strlen(s);
    while (len > 0) {
        char c = s[len - 1];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            s[len - 1] = '\0';
            len--;
        } else {
            break;
        }
    }
}

// =============================================================================
// 内部工具：去掉 AW WiFi 日志前缀，例如 "WDUG:" / "WINF:" / "WERR:"
// =============================================================================
static void wifi_strip_log_prefix(char *s)
{
    if (!s) return;
    wifi_trim_inplace(s);

    const char *prefixes[] = {
        "WDUG:", "WINF:", "WERR:", "WWAR:", "WIFI:", "[APP_WIFI]", "[wifi_list]"
    };

    for (unsigned int i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); i++) {
        size_t n = strlen(prefixes[i]);
        if (strncmp(s, prefixes[i], n) == 0) {
            memmove(s, s + n, strlen(s + n) + 1);
            wifi_trim_inplace(s);
            return;
        }
    }
}

static bool wifi_is_all_digit(const char *s)
{
    if (!s || !*s) return false;
    for (const char *p = s; *p; p++) {
        if (*p < '0' || *p > '9') return false;
    }
    return true;
}

static bool wifi_is_mac_line(const char *s)
{
    if (!s) return false;

    int colon_count = 0;
    int hex_count = 0;
    for (const char *p = s; *p; p++) {
        char c = *p;
        if (c == ':') {
            colon_count++;
        } else if ((c >= '0' && c <= '9') ||
                   (c >= 'a' && c <= 'f') ||
                   (c >= 'A' && c <= 'F')) {
            hex_count++;
        } else {
            return false;
        }
    }

    return colon_count == 5 && hex_count == 12;
}

static bool wifi_is_freq_line(const char *s)
{
    if (!wifi_is_all_digit(s)) return false;
    int v = atoi(s);
    return (v >= 2400 && v <= 6000);
}

static bool wifi_is_bad_ssid_candidate(const char *s)
{
    if (!s || !*s) return true;

    if (strlen(s) >= 64) return true;
    if (wifi_is_mac_line(s)) return true;
    if (wifi_is_all_digit(s)) return true;

    if (strstr(s, "CCMP") || strstr(s, "TKIP") || strstr(s, "WPA") ||
        strstr(s, "RSSI") || strstr(s, "completed") || strstr(s, "success") ||
        strstr(s, "state") || strstr(s, "cmd") || strstr(s, "event") ||
        strstr(s, "arg:") || strstr(s, "wlan0") || strstr(s, "ipv4") ||
        strstr(s, "Wi-Fi") || strstr(s, "wifi")) {
        return true;
    }

    return false;
}

static bool wifi_copy_if_valid_ssid(char *out, size_t out_size, const char *candidate)
{
    if (!out || out_size == 0 || !candidate) return false;

    char tmp[128];
    snprintf(tmp, sizeof(tmp), "%s", candidate);
    wifi_strip_log_prefix(tmp);
    wifi_trim_inplace(tmp);

    if (wifi_is_bad_ssid_candidate(tmp)) return false;

    snprintf(out, out_size, "%s", tmp);
    return out[0] != '\0';
}

static bool wifi_read_first_line_cmd(const char *cmd, char *out, size_t out_size)
{
    if (!cmd || !out || out_size == 0) return false;
    out[0] = '\0';

    FILE *fp = popen(cmd, "r");
    if (!fp) return false;

    char line[256];
    bool ok = false;
    if (fgets(line, sizeof(line), fp) != NULL) {
        wifi_trim_inplace(line);
        if (line[0] != '\0') {
            snprintf(out, out_size, "%s", line);
            ok = true;
        }
    }

    pclose(fp);
    return ok;
}

static bool wifi_parse_status_output_cmd(const char *cmd, char *out, size_t out_size)
{
    if (!cmd || !out || out_size == 0) return false;
    out[0] = '\0';

    FILE *fp = popen(cmd, "r");
    if (!fp) return false;

    char line[256];
    bool seen_completed = false;
    bool next_after_freq_is_ssid = false;
    bool ok = false;

    while (fgets(line, sizeof(line), fp) != NULL) {
        wifi_strip_log_prefix(line);
        wifi_trim_inplace(line);

        if (line[0] == '\0') continue;

        char *p = NULL;

        p = strstr(line, "ssid=");
        if (p) {
            p += 5;
            if (wifi_copy_if_valid_ssid(out, out_size, p)) {
                ok = true;
                break;
            }
        }

        p = strstr(line, "SSID=");
        if (p) {
            p += 5;
            if (wifi_copy_if_valid_ssid(out, out_size, p)) {
                ok = true;
                break;
            }
        }

        p = strstr(line, "ssid:");
        if (p) {
            p += 5;
            if (wifi_copy_if_valid_ssid(out, out_size, p)) {
                ok = true;
                break;
            }
        }

        p = strstr(line, "SSID:");
        if (p) {
            p += 5;
            if (wifi_copy_if_valid_ssid(out, out_size, p)) {
                ok = true;
                break;
            }
        }

        if (strstr(line, "wpa state is completed") || strstr(line, "WPA state is completed")) {
            seen_completed = true;
            next_after_freq_is_ssid = false;
            continue;
        }

        if (seen_completed && wifi_is_freq_line(line)) {
            next_after_freq_is_ssid = true;
            continue;
        }

        if (seen_completed && next_after_freq_is_ssid) {
            if (wifi_copy_if_valid_ssid(out, out_size, line)) {
                ok = true;
                break;
            }
            next_after_freq_is_ssid = false;
        }
    }

    pclose(fp);
    return ok;
}

// =============================================================================
// 功能：获取当前已连接 WiFi 的 SSID
// =============================================================================
bool NetWork_WIFI_GetConnectedSSID(char *ssid_buf, size_t ssid_buf_size)
{
    if (!ssid_buf || ssid_buf_size == 0) return false;
    ssid_buf[0] = '\0';

    /* 1. 标准 wireless-tools */
    if (wifi_read_first_line_cmd("iwgetid -r 2>/dev/null", ssid_buf, ssid_buf_size)) {
        printf("[DEBUG] 当前已连接SSID(iwgetid): %s\r\n", ssid_buf);
        return true;
    }

    /* 2. wpa_supplicant */
    if (wifi_read_first_line_cmd("wpa_cli -i wlan0 status 2>/dev/null | sed -n 's/^ssid=//p' | head -n 1",
                                 ssid_buf,
                                 ssid_buf_size)) {
        printf("[DEBUG] 当前已连接SSID(wpa_cli): %s\r\n", ssid_buf);
        return true;
    }

    /* 3. 全志/板端 wifi 命令：你的日志里 status 会打印 wpa state/freq/SSID。 */
    if (wifi_parse_status_output_cmd("wifi -s 2>&1", ssid_buf, ssid_buf_size)) {
        printf("[DEBUG] 当前已连接SSID(wifi -s): %s\r\n", ssid_buf);
        return true;
    }

    if (wifi_parse_status_output_cmd("wifi -g 2>&1", ssid_buf, ssid_buf_size)) {
        printf("[DEBUG] 当前已连接SSID(wifi -g): %s\r\n", ssid_buf);
        return true;
    }

    if (wifi_parse_status_output_cmd("wifi -S 2>&1", ssid_buf, ssid_buf_size)) {
        printf("[DEBUG] 当前已连接SSID(wifi -S): %s\r\n", ssid_buf);
        return true;
    }

    return false;
}
