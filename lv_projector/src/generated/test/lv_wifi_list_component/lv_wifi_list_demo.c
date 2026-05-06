/*
 * 用法示例：
 * 1. 把 lv_wifi_list_component.c/.h 加入工程。
 * 2. 在你的页面 .c 里参考本文件创建组件。
 * 3. 这个 demo 依赖你现有的 lv_pro_res_wifi_scan(), wifi_scan_res[], scan_results_num。
 *    如果你的 wifi_scan_result_t 定义在某个 SDK 头文件里，请在下面 include 那个头文件。
 */

#include "lv_wifi_list_demo.h"
#include "lv_wifi_list_component.h"
#include "lv_pro_res_wifi.h"
#include "lv_pro_wifi_common.h"
#include "NetWork_WIFI_Function.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

/*
 * 你需要在这里 include 真实定义 wifi_scan_result_t 的头文件。
 * 例如：
 * #include "wifi_intf.h"
 * #include "wifi_daemon_api.h"
 */

extern int lv_pro_res_wifi_scan(void);
extern int scan_results_num;
extern wifi_scan_result_t wifi_scan_res[];

static lv_wifi_list_component_t *g_wifi_list = NULL;
static char g_last_connect_request_ssid[LV_WIFI_SSID_MAX_LEN] = {0};

#define APP_WIFI_SCAN_TIMEOUT_SEC  8

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int inflight;
    int done;
    int ret;
} app_wifi_scan_guard_t;

static app_wifi_scan_guard_t g_scan_guard = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .inflight = 0,
    .done = 0,
    .ret = -1,
};

static void *app_wifi_scan_worker(void *arg)
{
    int ret;
    (void)arg;

    ret = lv_pro_res_wifi_scan();

    pthread_mutex_lock(&g_scan_guard.lock);
    g_scan_guard.ret = ret;
    g_scan_guard.done = 1;
    g_scan_guard.inflight = 0;
    pthread_cond_broadcast(&g_scan_guard.cond);
    pthread_mutex_unlock(&g_scan_guard.lock);
    return NULL;
}

static void trim_line_end(char *s)
{
    if (!s) return;

    size_t len = strlen(s);
    while (len > 0) {
        char c = s[len - 1];
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
            s[len - 1] = '\0';
            len--;
        } else {
            break;
        }
    }
}

static bool app_wifi_read_cmd_first_line(const char *cmd, char *buf, size_t buf_size)
{
    if (!cmd || !buf || buf_size == 0) return false;

    buf[0] = '\0';

    FILE *fp = popen(cmd, "r");
    if (!fp) return false;

    if (fgets(buf, (int)buf_size, fp) == NULL) {
        pclose(fp);
        buf[0] = '\0';
        return false;
    }

    pclose(fp);
    trim_line_end(buf);

    return buf[0] != '\0';
}

static bool app_wifi_get_connected_ssid_cb(char *ssid_buf,
                                           size_t ssid_buf_size,
                                           void *user_data)
{
    (void)user_data;

    if (!ssid_buf || ssid_buf_size == 0) return false;
    ssid_buf[0] = '\0';

    if (NetWork_WIFI_GetConnectedSSID(ssid_buf, ssid_buf_size)) {
        printf("[APP_WIFI] current connected ssid=%s\n", ssid_buf);
        return true;
    }

    if (g_last_connect_request_ssid[0] != '\0' && NetWork_WIFI_IsConnected()) {
        snprintf(ssid_buf, ssid_buf_size, "%s", g_last_connect_request_ssid);
        printf("[APP_WIFI] current connected ssid=%s (ip fallback)\n", ssid_buf);
        return true;
    }

    printf("[APP_WIFI] current connected ssid=<none>\n");
    return false;
}


static int app_wifi_scan_cb(lv_wifi_list_ap_t *items, int max_items, void *user_data)
{
    (void)user_data;
    pthread_t th;
    struct timespec ts;
    int wait_ret;
    int ret;

    pthread_mutex_lock(&g_scan_guard.lock);
    if (g_scan_guard.inflight) {
        pthread_mutex_unlock(&g_scan_guard.lock);
        printf("[APP_WIFI] scan skipped: previous scan still running\n");
        return -1;
    }

    g_scan_guard.inflight = 1;
    g_scan_guard.done = 0;
    g_scan_guard.ret = -1;
    pthread_mutex_unlock(&g_scan_guard.lock);

    if (pthread_create(&th, NULL, app_wifi_scan_worker, NULL) != 0) {
        pthread_mutex_lock(&g_scan_guard.lock);
        g_scan_guard.inflight = 0;
        g_scan_guard.done = 1;
        g_scan_guard.ret = -1;
        pthread_mutex_unlock(&g_scan_guard.lock);
        printf("[APP_WIFI] scan failed: create worker thread failed\n");
        return -1;
    }
    pthread_detach(th);

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
    }
    ts.tv_sec += APP_WIFI_SCAN_TIMEOUT_SEC;

    pthread_mutex_lock(&g_scan_guard.lock);
    while (!g_scan_guard.done) {
        wait_ret = pthread_cond_timedwait(&g_scan_guard.cond, &g_scan_guard.lock, &ts);
        if (wait_ret == ETIMEDOUT) {
            pthread_mutex_unlock(&g_scan_guard.lock);
            printf("[APP_WIFI] scan timeout after %d sec, keep UI responsive\n",
                   APP_WIFI_SCAN_TIMEOUT_SEC);
            return -1;
        }
    }
    ret = g_scan_guard.ret;
    pthread_mutex_unlock(&g_scan_guard.lock);

    if (ret != 0) {
        printf("[APP_WIFI] scan failed ret=%d\n", ret);
        return -1;
    }

    int count = scan_results_num;
    if (count > max_items) count = max_items;

    for (int i = 0; i < count; i++) {
        snprintf(items[i].ssid, sizeof(items[i].ssid), "%s", wifi_scan_res[i].ssid);
        items[i].rssi = wifi_scan_res[i].rssi;
        items[i].security = wifi_scan_res[i].key_mgmt;
    }

    printf("[APP_WIFI] scan copy count=%d\n", count);
    return count;
}

static void app_wifi_click_cb(const char *ssid, void *user_data)
{
    (void)user_data;
    printf("[APP_WIFI] click wifi ssid=%s\n", ssid);
}

static bool app_wifi_connect_cb(const char *ssid, const char *password, void *user_data)
{
    (void)user_data;

    printf("[APP_WIFI] connect request ssid=%s\n", ssid);

    bool ok = NetWork_WIFI_Connect(ssid, password);
    if (ok) {
        snprintf(g_last_connect_request_ssid, sizeof(g_last_connect_request_ssid), "%s", ssid ? ssid : "");
    } else {
        g_last_connect_request_ssid[0] = '\0';
    }
    return ok;
}

void app_wifi_list_open(lv_obj_t *current_screen)
{
    if (g_wifi_list) {
        printf("[APP_WIFI] already opened, show and refresh async\n");
        lv_obj_t *root = lv_wifi_list_get_root(g_wifi_list);
        if (root) lv_obj_clear_flag(root, LV_OBJ_FLAG_HIDDEN);
        lv_wifi_list_refresh_connected_ssid(g_wifi_list);
        lv_wifi_list_refresh(g_wifi_list);
        return;
    }

    lv_wifi_list_cfg_t cfg = {
        .x = 0,
        .y = 0,
        .w = 900,
        .h = 700,
        .max_items = 128,
        .default_password = NULL,         /* 不使用默认密码，点击 WiFi 后手动输入 */
        .auto_scan_on_create = true,
        .scan_cb = app_wifi_scan_cb,
        .connect_cb = app_wifi_connect_cb,
        .click_cb = app_wifi_click_cb,
        .get_connected_ssid_cb = app_wifi_get_connected_ssid_cb,
        .user_data = NULL,
    };

    g_wifi_list = lv_wifi_list_create(current_screen, &cfg);
}

void app_wifi_list_close(void)
{
    lv_wifi_list_destroy(&g_wifi_list);
}

void app_wifi_list_refresh(void)
{
    if (g_wifi_list) {
        lv_wifi_list_refresh(g_wifi_list);
    }
}

void app_wifi_list_set_password(const char *password)
{
    if (g_wifi_list) {
        lv_wifi_list_set_password(g_wifi_list, password);
    }
}


void app_wifi_list_refresh_connected(void)
{
    if (g_wifi_list) {
        lv_wifi_list_refresh_connected_ssid(g_wifi_list);
    }
}
