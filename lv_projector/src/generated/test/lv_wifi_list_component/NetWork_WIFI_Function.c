#include "NetWork_WIFI_Function.h"

#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>

#define WIFI_SSID_CACHE_PATH "/usr/share/lv_projector/app_wifi_last_connected_ssid.txt"
#define WIFI_SSID_CACHE_TMP  "/usr/share/lv_projector/app_wifi_last_connected_ssid.txt.tmp"

static int wifi_run_argv(char *const argv[])
{
    pid_t pid;
    int status;

    if (argv == NULL || argv[0] == NULL) return -1;

    pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        execvp(argv[0], argv);
        _exit(127);
    }

    if (waitpid(pid, &status, 0) < 0) return -1;
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
}

static void wifi_trim_inplace(char *s)
{
    if (!s) return;

    char *p = s;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    if (p != s) memmove(s, p, strlen(p) + 1);

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

bool NetWork_WIFI_SaveConnectedSSID(const char *ssid)
{
    if (!ssid || ssid[0] == '\0') return false;

    FILE *fp = fopen(WIFI_SSID_CACHE_TMP, "w");
    if (!fp) {
        printf("[DEBUG] 保存WiFi SSID失败: open tmp failed\r\n");
        return false;
    }

    fprintf(fp, "%s\n", ssid);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);

    if (rename(WIFI_SSID_CACHE_TMP, WIFI_SSID_CACHE_PATH) != 0) {
        printf("[DEBUG] 保存WiFi SSID失败: rename failed errno=%d\r\n", errno);
        unlink(WIFI_SSID_CACHE_TMP);
        return false;
    }

    printf("[DEBUG] 保存WiFi SSID缓存: %s\r\n", ssid);
    return true;
}

bool NetWork_WIFI_GetConnectedSSID(char *ssid_buf, size_t ssid_buf_size)
{
    if (!ssid_buf || ssid_buf_size == 0) return false;
    ssid_buf[0] = '\0';

    FILE *fp = fopen(WIFI_SSID_CACHE_PATH, "r");
    if (!fp) return false;

    if (fgets(ssid_buf, (int)ssid_buf_size, fp) == NULL) {
        fclose(fp);
        ssid_buf[0] = '\0';
        return false;
    }

    fclose(fp);
    wifi_trim_inplace(ssid_buf);
    if (ssid_buf[0] == '\0') return false;

    printf("[DEBUG] 当前WiFi SSID缓存: %s\r\n", ssid_buf);
    return true;
}

bool NetWork_WIFI_Connect(const char *ssid, const char *password)
{
    if (ssid == NULL || password == NULL) {
        fprintf(stderr, "[ERROR] WIFI连接参数错误：SSID或密码为空\r\n");
        return false;
    }

    if (strlen(ssid) == 0 || strlen(password) == 0) {
        fprintf(stderr, "[ERROR] WIFI连接参数错误：SSID或密码为空字符串\r\n");
        return false;
    }

    printf("[DEBUG] 开始连接WIFI: SSID=%s\r\n", ssid);

    int ret = system("wifi_daemon &");
    if (ret != 0) {
        fprintf(stderr, "[WARN] 启动wifi守护进程失败，返回码: %d（可能已运行）\r\n", ret);
    }

    usleep(500000);

    ret = system("wifi -o sta");
    if (ret != 0) {
        fprintf(stderr, "[WARN] 设置WIFI为station模式失败，返回码: %d（可能已设置）\r\n", ret);
    }

    usleep(300000);

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

    NetWork_WIFI_SaveConnectedSSID(ssid);
    printf("[DEBUG] WIFI连接命令执行成功\r\n");
    return true;
}

bool NetWork_WIFI_Disconnect(void)
{
    int ret = system("wifi -d");
    if (ret == 0) {
        unlink(WIFI_SSID_CACHE_PATH);
        unlink(WIFI_SSID_CACHE_TMP);
        return true;
    }
    return false;
}

bool NetWork_WIFI_IsConnected(void)
{
    struct ifaddrs *ifaddr, *ifa;
    bool connected = false;

    if (getifaddrs(&ifaddr) == -1) {
        perror("[ERROR] 获取网络接口信息失败");
        return false;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        if (strcmp(ifa->ifa_name, "wlan0") == 0 &&
            (ifa->ifa_flags & IFF_UP) &&
            ifa->ifa_addr->sa_family == AF_INET) {
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

    freeifaddrs(ifaddr);
    return connected;
}
