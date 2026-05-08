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

#define APP_WIFI_SCAN_CACHE_MAX 128
#define APP_WIFI_SCAN_POLL_MS   100

typedef struct {
    pthread_mutex_t lock;
    int inflight;
    int result_ready;
    int ret;
    int count;
    lv_wifi_list_ap_t items[APP_WIFI_SCAN_CACHE_MAX];
} app_wifi_async_scan_t;

static app_wifi_async_scan_t g_async_scan = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .inflight = 0,
    .result_ready = 0,
    .ret = -1,
    .count = 0,
};

static lv_timer_t *g_async_scan_poll_timer = NULL;

static void app_wifi_scan_poll_cb(lv_timer_t *timer);

static void app_wifi_scan_poll_start(void)
{
    if (g_async_scan_poll_timer) return;

    g_async_scan_poll_timer = lv_timer_create(app_wifi_scan_poll_cb,
                                              APP_WIFI_SCAN_POLL_MS,
                                              NULL);
}

static void app_wifi_scan_poll_stop(void)
{
    if (!g_async_scan_poll_timer) return;

    lv_timer_del(g_async_scan_poll_timer);
    g_async_scan_poll_timer = NULL;
}

static void *app_wifi_scan_worker(void *arg)
{
    int ret;
    int count = 0;
    lv_wifi_list_ap_t local_items[APP_WIFI_SCAN_CACHE_MAX];
    (void)arg;

    memset(local_items, 0, sizeof(local_items));

    ret = lv_pro_res_wifi_scan();

    if (ret == 0) {
        count = scan_results_num;
        if (count > APP_WIFI_SCAN_CACHE_MAX) count = APP_WIFI_SCAN_CACHE_MAX;
        if (count < 0) count = 0;

        int visible_count = 0;
        for (int i = 0; i < count && visible_count < APP_WIFI_SCAN_CACHE_MAX; i++) {
            const char *ssid = wifi_scan_res[i].ssid;
            if (ssid == NULL || ssid[0] == '\0') {
                continue;
            }

            snprintf(local_items[visible_count].ssid, sizeof(local_items[visible_count].ssid), "%s", ssid);
            local_items[visible_count].rssi = wifi_scan_res[i].rssi;
            local_items[visible_count].security = wifi_scan_res[i].key_mgmt;
            visible_count++;
        }
        count = visible_count;
    }

    pthread_mutex_lock(&g_async_scan.lock);
    g_async_scan.ret = ret;
    g_async_scan.count = count;
    memcpy(g_async_scan.items, local_items, sizeof(local_items));
    g_async_scan.result_ready = 1;
    g_async_scan.inflight = 0;
    pthread_mutex_unlock(&g_async_scan.lock);

    printf("[APP_WIFI] background scan done ret=%d count=%d\n", ret, count);
    return NULL;
}

static int app_wifi_scan_start_background(void)
{
    pthread_t th;

    pthread_mutex_lock(&g_async_scan.lock);
    if (g_async_scan.inflight) {
        pthread_mutex_unlock(&g_async_scan.lock);
        return LV_WIFI_LIST_SCAN_PENDING;
    }

    g_async_scan.inflight = 1;
    g_async_scan.result_ready = 0;
    g_async_scan.ret = -1;
    g_async_scan.count = 0;
    pthread_mutex_unlock(&g_async_scan.lock);

    if (pthread_create(&th, NULL, app_wifi_scan_worker, NULL) != 0) {
        pthread_mutex_lock(&g_async_scan.lock);
        g_async_scan.inflight = 0;
        g_async_scan.result_ready = 0;
        g_async_scan.ret = -1;
        g_async_scan.count = 0;
        pthread_mutex_unlock(&g_async_scan.lock);
        printf("[APP_WIFI] scan failed: create worker thread failed\n");
        return -1;
    }

    pthread_detach(th);
    app_wifi_scan_poll_start();
    printf("[APP_WIFI] background scan started\n");
    return LV_WIFI_LIST_SCAN_PENDING;
}


static void app_wifi_scan_drop_ready_result_if_idle(void)
{
    pthread_mutex_lock(&g_async_scan.lock);
    if (!g_async_scan.inflight) {
        g_async_scan.result_ready = 0;
        g_async_scan.ret = -1;
        g_async_scan.count = 0;
        memset(g_async_scan.items, 0, sizeof(g_async_scan.items));
    }
    pthread_mutex_unlock(&g_async_scan.lock);
}

static void app_wifi_cache_last_ssid(const char *ssid)
{
    if (!ssid || ssid[0] == '\0') return;
    snprintf(g_last_connect_request_ssid, sizeof(g_last_connect_request_ssid), "%s", ssid);
}

static bool app_wifi_get_connected_ssid_cb(char *ssid_buf,
                                           size_t ssid_buf_size,
                                           void *user_data)
{
    (void)user_data;

    if (!ssid_buf || ssid_buf_size == 0) return false;
    ssid_buf[0] = '\0';

    /*
     * 不再执行任何系统命令查询当前 WiFi。
     * 当前显示的 SSID 只从 /usr/share/lv_projector 持久缓存读取，避免卡 UI。
     */
    if (NetWork_WIFI_GetConnectedSSID(ssid_buf, ssid_buf_size)) {
        printf("[APP_WIFI] cached connected ssid=%s\n", ssid_buf);
        return true;
    }

    printf("[APP_WIFI] cached connected ssid=<none>\n");
    return false;
}

static int app_wifi_scan_cb(lv_wifi_list_ap_t *items, int max_items, void *user_data)
{
    int ret;
    int count;
    (void)user_data;

    if (!items || max_items <= 0) return -1;

    pthread_mutex_lock(&g_async_scan.lock);
    if (g_async_scan.result_ready) {
        ret = g_async_scan.ret;
        count = g_async_scan.count;
        if (count > max_items) count = max_items;

        if (ret == 0 && count > 0) {
            memcpy(items, g_async_scan.items, sizeof(lv_wifi_list_ap_t) * count);
        }

        g_async_scan.result_ready = 0;
        pthread_mutex_unlock(&g_async_scan.lock);

        if (ret != 0) {
            printf("[APP_WIFI] scan result failed ret=%d\n", ret);
            return -1;
        }

        printf("[APP_WIFI] scan copy count=%d\n", count);
        return count;
    }

    if (g_async_scan.inflight) {
        pthread_mutex_unlock(&g_async_scan.lock);
        printf("[APP_WIFI] scan pending: previous background scan still running\n");
        return LV_WIFI_LIST_SCAN_PENDING;
    }
    pthread_mutex_unlock(&g_async_scan.lock);

    return app_wifi_scan_start_background();
}

static void app_wifi_scan_poll_cb(lv_timer_t *timer)
{
    int ready;
    int inflight;
    (void)timer;

    pthread_mutex_lock(&g_async_scan.lock);
    ready = g_async_scan.result_ready;
    inflight = g_async_scan.inflight;
    pthread_mutex_unlock(&g_async_scan.lock);

    if (ready) {
        app_wifi_scan_poll_stop();
        if (g_wifi_list) {
            lv_wifi_list_refresh(g_wifi_list);
        }
        return;
    }

    if (!inflight) {
        app_wifi_scan_poll_stop();
    }
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
        app_wifi_cache_last_ssid(ssid ? ssid : "");
    } else {
        g_last_connect_request_ssid[0] = '\0';
    }
    return ok;
}

void app_wifi_list_open(lv_obj_t *current_screen)
{
    if (g_wifi_list) {
        printf("[APP_WIFI] open existing: show and scan once\n");
        lv_obj_t *root = lv_wifi_list_get_root(g_wifi_list);
        if (root) lv_obj_clear_flag(root, LV_OBJ_FLAG_HIDDEN);

        /*
         * 每次打开页面只触发一次真正的后台扫描。
         * 不再只是 show，也不查询系统命令。
         * 丢掉上一次未消费的旧扫描结果，避免打开后只显示旧缓存而没有新扫描动作。
         */
        app_wifi_scan_drop_ready_result_if_idle();
        lv_wifi_list_refresh_connected_ssid(g_wifi_list);
        (void)lv_wifi_list_refresh(g_wifi_list);
        return;
    }

    app_wifi_scan_drop_ready_result_if_idle();
    printf("[APP_WIFI] open new: create and scan once\n");

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
