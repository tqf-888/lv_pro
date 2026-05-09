/*
 * WiFi list demo glue layer.
 * 最简设计：扫描线程由组件内部管理；本文件只负责真正扫描一次和拷贝结果。
 * 当前已连接 SSID 只读 /usr/share/lv_projector 缓存文件，不执行系统查询命令。
 */

#include "lv_wifi_list_demo.h"
#include "lv_wifi_list_component.h"
#include "lv_pro_res_wifi.h"
#include "lv_pro_wifi_common.h"
#include "NetWork_WIFI_Function.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern int lv_pro_res_wifi_scan(void);
extern int scan_results_num;
extern wifi_scan_result_t wifi_scan_res[];

static lv_wifi_list_component_t *g_wifi_list = NULL;

static bool app_wifi_get_connected_ssid_cb(char *ssid_buf,
                                           size_t ssid_buf_size,
                                           void *user_data)
{
    (void)user_data;

    if (!ssid_buf || ssid_buf_size == 0) return false;
    ssid_buf[0] = '\0';

    if (NetWork_WIFI_GetConnectedSSID(ssid_buf, ssid_buf_size)) {
        printf("[APP_WIFI] cached connected ssid=%s\n", ssid_buf);
        return true;
    }

    printf("[APP_WIFI] cached connected ssid=<none>\n");
    return false;
}

static int app_wifi_scan_cb(lv_wifi_list_ap_t *items, int max_items, void *user_data)
{
    (void)user_data;

    if (!items || max_items <= 0) return -1;

    int ret = lv_pro_res_wifi_scan();
    if (ret != 0) {
        printf("[APP_WIFI] scan failed ret=%d\n", ret);
        return -1;
    }

    int out_count = 0;
    int in_count = scan_results_num;

    for (int i = 0; i < in_count && out_count < max_items; i++) {
        const char *ssid = wifi_scan_res[i].ssid;
        if (!ssid || ssid[0] == '\0') {
            continue;
        }

        snprintf(items[out_count].ssid, sizeof(items[out_count].ssid), "%s", ssid);
        items[out_count].rssi = wifi_scan_res[i].rssi;
        items[out_count].security = wifi_scan_res[i].key_mgmt;
        out_count++;
    }

    printf("[APP_WIFI] scan copy visible count=%d raw=%d\n", out_count, in_count);
    return out_count;
}

static void app_wifi_click_cb(const char *ssid, void *user_data)
{
    (void)user_data;
    printf("[APP_WIFI] click wifi ssid=%s\n", ssid ? ssid : "<null>");
}

static bool app_wifi_connect_cb(const char *ssid, const char *password, void *user_data)
{
    (void)user_data;
    printf("[APP_WIFI] connect request ssid=%s\n", ssid ? ssid : "<null>");
    return NetWork_WIFI_Connect(ssid, password);
}

void app_wifi_list_open(lv_obj_t *current_screen)
{
    if (g_wifi_list) {
        printf("[APP_WIFI] open existing\n");
        lv_obj_t *root = lv_wifi_list_get_root(g_wifi_list);
        if (root) lv_obj_clear_flag(root, LV_OBJ_FLAG_HIDDEN);
        lv_wifi_list_refresh_connected_ssid(g_wifi_list);
        lv_wifi_list_refresh(g_wifi_list);
        return;
    }

    printf("[APP_WIFI] open new\n");

    lv_wifi_list_cfg_t cfg = {
        .x = 0,
        .y = 0,
        .w = 900,
        .h = 700,
        .max_items = 128,
        .default_password = NULL,
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
    if (g_wifi_list) lv_wifi_list_refresh(g_wifi_list);
}

void app_wifi_list_set_password(const char *password)
{
    if (g_wifi_list) lv_wifi_list_set_password(g_wifi_list, password);
}

void app_wifi_list_refresh_connected(void)
{
    if (g_wifi_list) lv_wifi_list_refresh_connected_ssid(g_wifi_list);
}
