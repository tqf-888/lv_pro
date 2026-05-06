#ifndef LV_WIFI_LIST_COMPONENT_H
#define LV_WIFI_LIST_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef LV_WIFI_SSID_MAX_LEN
#define LV_WIFI_SSID_MAX_LEN 64
#endif

typedef struct lv_wifi_list_component lv_wifi_list_component_t;

typedef struct {
    char ssid[LV_WIFI_SSID_MAX_LEN];
    int rssi;
    int security;
} lv_wifi_list_ap_t;

/*
 * 扫描回调：
 *  - items: 组件提供的临时缓存
 *  - max_items: 最多可写入数量
 *  - user_data: 用户透传数据
 *  - 返回实际写入的 WiFi 数量，失败返回负数
 */
typedef int (*lv_wifi_list_scan_cb_t)(lv_wifi_list_ap_t *items,
                                      int max_items,
                                      void *user_data);

/*
 * 连接回调：
 *  - 用户点击 WiFi 后弹出密码框
 *  - 点击弹窗里的“连接”后才调用这个回调
 *  - 返回 true 只表示连接命令下发成功，不代表已经连上
 *  - 组件会继续轮询当前 SSID，确认后显示“连接成功xxx”
 */
typedef bool (*lv_wifi_list_connect_cb_t)(const char *ssid,
                                          const char *password,
                                          void *user_data);

/* 点击 WiFi item 时触发，只打印/通知，不负责连接，可为 NULL */
typedef void (*lv_wifi_list_click_cb_t)(const char *ssid, void *user_data);

/*
 * 获取当前已连接 WiFi 名称。
 * 返回 true 表示 ssid_buf 中有有效 SSID。
 * 组件会在创建、刷新完成、连接命令发送后调用，用于显示“已连接xxx”。
 */
typedef bool (*lv_wifi_list_get_connected_ssid_cb_t)(char *ssid_buf,
                                                     size_t ssid_buf_size,
                                                     void *user_data);

typedef struct {
    int x;
    int y;
    int w;
    int h;

    int max_items;                  /* 建议 64/128，不够可以加大 */
    const char *default_password;   /* 保留兼容字段；当前弹窗默认仍为空 */
    bool auto_scan_on_create;       /* 创建后是否立即异步扫描刷新 */

    lv_wifi_list_scan_cb_t scan_cb;
    lv_wifi_list_connect_cb_t connect_cb;
    lv_wifi_list_click_cb_t click_cb;
    lv_wifi_list_get_connected_ssid_cb_t get_connected_ssid_cb;
    void *user_data;
} lv_wifi_list_cfg_t;

lv_wifi_list_component_t *lv_wifi_list_create(lv_obj_t *screen,
                                              const lv_wifi_list_cfg_t *cfg);

void lv_wifi_list_destroy(lv_wifi_list_component_t **comp);

/*
 * 触发一次异步扫描。
 * 返回：
 *   >=0 = 当前扫描到的 WiFi 数量
 *   -1  = 参数/scan_cb 错误或扫描失败
 */
int lv_wifi_list_refresh(lv_wifi_list_component_t *comp);

lv_obj_t *lv_wifi_list_get_root(lv_wifi_list_component_t *comp);

void lv_wifi_list_bring_to_front(lv_wifi_list_component_t *comp);

void lv_wifi_list_set_password(lv_wifi_list_component_t *comp,
                               const char *password);

/* 主动重新读取当前已连接 SSID，并刷新“已连接xxx”显示 */
void lv_wifi_list_refresh_connected_ssid(lv_wifi_list_component_t *comp);

#ifdef __cplusplus
}
#endif

#endif
