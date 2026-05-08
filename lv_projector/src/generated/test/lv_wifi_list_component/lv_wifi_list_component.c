#include "lv_wifi_list_component.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIFI_LIST_LOG(fmt, ...) \
    printf("[wifi_list] " fmt "\n", ##__VA_ARGS__)

#ifndef LV_WIFI_LIST_DEFAULT_MAX_ITEMS
#define LV_WIFI_LIST_DEFAULT_MAX_ITEMS 128
#endif

#ifndef LV_WIFI_LIST_DEFAULT_W
#define LV_WIFI_LIST_DEFAULT_W 820
#endif

#ifndef LV_WIFI_LIST_DEFAULT_H
#define LV_WIFI_LIST_DEFAULT_H 640
#endif

#define WIFI_HEADER_H       72
#define WIFI_STATUS_H       38
#define WIFI_FOOTER_H       58
#define WIFI_BTN_W          128
#define WIFI_BTN_H          50
#define WIFI_PAGE_BTN_W     118
#define WIFI_PAGE_BTN_H     48
#define WIFI_LIST_PAD       10
#define WIFI_ROW_H          56
#define WIFI_ROW_GAP        8
#define WIFI_PAGE_SIZE      7

/* WiFi item content position.
 * 图标和文字位置以后只改这里。
 */
#define WIFI_ITEM_ICON_X    22
#define WIFI_ITEM_ICON_Y    3    /* 图片比原来向上 5px */
#define WIFI_ITEM_LABEL_X   88   /* 44x35 图片右侧留出文字间隔 */
#define WIFI_ITEM_LABEL_Y   8

#define WIFI_ITEM_ICON_W    44
#define WIFI_ITEM_ICON_H    35

#define WIFI_ITEM_CONNECTED_W   300
#define WIFI_ITEM_CONNECTED_X   -22
#define WIFI_ITEM_CONNECTED_Y   8

/* WiFi item inner label/icon background.
 * btn 本身有背景，icon/label 背景保持透明。
 */
#define WIFI_ITEM_ICON_BG_OPA   LV_OPA_TRANSP
#define WIFI_ITEM_LABEL_BG_OPA  LV_OPA_TRANSP

#define WIFI_POPUP_W        620
#define WIFI_POPUP_H        330
#define WIFI_POPUP_BTN_W    140
#define WIFI_POPUP_BTN_H    54

#define WIFI_CONNECT_CHECK_INTERVAL_MS  1000
#define WIFI_CONNECT_CHECK_MAX_COUNT    20
/* 连接命令返回 ok 后，使用本次点击的目标 SSID 作为已连接名，避免 UI 永远误判失败。 */
#define WIFI_CONNECT_ASSUME_SUCCESS_AFTER_COUNT  8
#define WIFI_SCAN_FAIL_BACKOFF_SEC 3

LV_FONT_DECLARE(lv_font_Regular_20);

LV_IMG_DECLARE(_ererhgredh1_alpha_44x35); /* 非常好 */
LV_IMG_DECLARE(_ererhgredh2_alpha_44x35); /* 好 */
LV_IMG_DECLARE(_ererhgredh3_alpha_44x35); /* 良 */
LV_IMG_DECLARE(_ererhgredh4_alpha_44x35); /* 差 */

struct lv_wifi_list_component {
    lv_obj_t *root;
    lv_obj_t *header;
    lv_obj_t *title;
    lv_obj_t *btn_refresh;
    lv_obj_t *list;
    lv_obj_t *status;
    lv_obj_t *footer;
    lv_obj_t *btn_prev;
    lv_obj_t *btn_next;
    lv_obj_t *page_label;

    lv_obj_t *popup_mask;
    lv_obj_t *popup;
    lv_obj_t *popup_title;
    lv_obj_t *password_ta;
    lv_obj_t *password_toggle_btn;
    bool password_visible;
#if LV_USE_KEYBOARD
    lv_obj_t *keyboard;
#endif
    char selected_ssid[LV_WIFI_SSID_MAX_LEN];
    char connected_ssid[LV_WIFI_SSID_MAX_LEN];
    char pending_connect_ssid[LV_WIFI_SSID_MAX_LEN];
    lv_timer_t *connect_check_timer;
    int connect_check_left;
    lv_timer_t *scan_retry_timer;
    lv_obj_t *scan_spinner;
    int scan_in_progress;

    lv_style_t style_root;
    lv_style_t style_header;
    lv_style_t style_title;
    lv_style_t style_btn;
    lv_style_t style_btn_pressed;
    lv_style_t style_list;
    lv_style_t style_item;
    lv_style_t style_item_pressed;
    lv_style_t style_status;
    lv_style_t style_popup_mask;
    lv_style_t style_popup;
    lv_style_t style_textarea;

    lv_wifi_list_ap_t *items;
    int max_items;
    int item_count;
    int page_index;
    int page_size;

    char password[96];

    lv_wifi_list_scan_cb_t scan_cb;
    lv_wifi_list_connect_cb_t connect_cb;
    lv_wifi_list_click_cb_t click_cb;
    lv_wifi_list_get_connected_ssid_cb_t get_connected_ssid_cb;
    void *user_data;
};

typedef struct {
    lv_wifi_list_component_t *comp;
    char ssid[LV_WIFI_SSID_MAX_LEN];
} wifi_item_ctx_t;

static void wifi_list_render(lv_wifi_list_component_t *comp);
static int wifi_list_refresh_impl(lv_wifi_list_component_t *comp);
static void wifi_password_popup_close(lv_wifi_list_component_t *comp);
static void wifi_password_popup_open(lv_wifi_list_component_t *comp, const char *ssid);
static void wifi_connect_check_start(lv_wifi_list_component_t *comp, const char *ssid);
static void wifi_connect_check_stop(lv_wifi_list_component_t *comp);
static void wifi_scan_retry_timer_cb(lv_timer_t *timer);
static void set_btn_disabled(lv_obj_t *btn, bool disabled);

static void safe_copy(char *dst, size_t dst_size, const char *src)
{
    if (!dst || dst_size == 0) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    snprintf(dst, dst_size, "%s", src);
}

static bool ssid_is_same(const char *a, const char *b)
{
    if (!a || !b) return false;
    if (a[0] == '\0' || b[0] == '\0') return false;
    return strcmp(a, b) == 0;
}

static bool ssid_is_valid_visible(const char *ssid)
{
    if (!ssid) return false;

    while (*ssid == ' ' || *ssid == '\t' || *ssid == '\r' || *ssid == '\n') {
        ssid++;
    }

    return *ssid != '\0';
}

static const void *wifi_signal_img_src(int rssi)
{
    /*
     * RSSI 越接近 0 信号越好。
     * >= -50 非常好，>= -65 好，>= -75 良，其余差。
     */
    if (rssi >= -50) return &_ererhgredh1_alpha_44x35;
    if (rssi >= -65) return &_ererhgredh2_alpha_44x35;
    if (rssi >= -75) return &_ererhgredh3_alpha_44x35;
    return &_ererhgredh4_alpha_44x35;
}

static void wifi_list_set_status_idle(lv_wifi_list_component_t *comp, const char *fallback)
{
    if (!comp || !comp->status) return;

    if (comp->connected_ssid[0] != '\0') {
        lv_label_set_text_fmt(comp->status, "已连接%s", comp->connected_ssid);
    } else {
        lv_label_set_text(comp->status, fallback ? fallback : "未连接 WiFi");
    }
}

static void wifi_list_update_connected_ssid(lv_wifi_list_component_t *comp)
{
    if (!comp) return;

    comp->connected_ssid[0] = '\0';

    if (comp->get_connected_ssid_cb) {
        bool ok = comp->get_connected_ssid_cb(comp->connected_ssid,
                                              sizeof(comp->connected_ssid),
                                              comp->user_data);
        if (!ok) {
            comp->connected_ssid[0] = '\0';
        }
    }

    if (comp->connected_ssid[0] != '\0') {
        WIFI_LIST_LOG("connected ssid=%s", comp->connected_ssid);
    } else {
        WIFI_LIST_LOG("connected ssid=<none>");
    }
}

static void wifi_scan_ui_set_busy(lv_wifi_list_component_t *comp, bool busy)
{
    if (!comp) return;

    comp->scan_in_progress = busy ? 1 : 0;

    if (comp->btn_refresh) {
        set_btn_disabled(comp->btn_refresh, busy);
    }

    if (comp->scan_spinner) {
        if (busy) {
            lv_obj_clear_flag(comp->scan_spinner, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(comp->scan_spinner, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static int page_count_get(lv_wifi_list_component_t *comp)
{
    if (!comp || comp->page_size <= 0 || comp->item_count <= 0) return 1;
    return (comp->item_count + comp->page_size - 1) / comp->page_size;
}

static void set_btn_disabled(lv_obj_t *btn, bool disabled)
{
    if (!btn) return;
    if (disabled) lv_obj_add_state(btn, LV_STATE_DISABLED);
    else lv_obj_clear_state(btn, LV_STATE_DISABLED);
}

static void wifi_list_style_init(lv_wifi_list_component_t *comp)
{
    lv_style_init(&comp->style_root);
    lv_style_set_bg_color(&comp->style_root, lv_color_hex(0x111827));
    lv_style_set_bg_opa(&comp->style_root, LV_OPA_COVER);
    lv_style_set_border_width(&comp->style_root, 0);
    lv_style_set_radius(&comp->style_root, 18);
    lv_style_set_pad_all(&comp->style_root, 14);

    lv_style_init(&comp->style_header);
    lv_style_set_bg_color(&comp->style_header, lv_color_hex(0x1f2937));
    lv_style_set_bg_opa(&comp->style_header, LV_OPA_COVER);
    lv_style_set_border_width(&comp->style_header, 0);
    lv_style_set_radius(&comp->style_header, 14);
    lv_style_set_pad_left(&comp->style_header, 18);
    lv_style_set_pad_right(&comp->style_header, 14);
    lv_style_set_pad_top(&comp->style_header, 8);
    lv_style_set_pad_bottom(&comp->style_header, 8);

    lv_style_init(&comp->style_title);
    lv_style_set_text_color(&comp->style_title, lv_color_hex(0xffffff));
    lv_style_set_text_font(&comp->style_title, &lv_font_Regular_20);

    lv_style_init(&comp->style_btn);
    lv_style_set_bg_color(&comp->style_btn, lv_color_hex(0x374151));
    lv_style_set_bg_opa(&comp->style_btn, LV_OPA_COVER);
    lv_style_set_border_width(&comp->style_btn, 0);
    lv_style_set_radius(&comp->style_btn, 12);
    lv_style_set_text_color(&comp->style_btn, lv_color_hex(0xffffff));
    lv_style_set_text_font(&comp->style_btn, &lv_font_Regular_20);

    lv_style_init(&comp->style_btn_pressed);
    lv_style_set_bg_color(&comp->style_btn_pressed, lv_color_hex(0x4b5563));
    lv_style_set_bg_opa(&comp->style_btn_pressed, LV_OPA_COVER);

    lv_style_init(&comp->style_list);
    lv_style_set_bg_color(&comp->style_list, lv_color_hex(0x0f172a));
    lv_style_set_bg_opa(&comp->style_list, LV_OPA_COVER);
    lv_style_set_border_width(&comp->style_list, 0);
    lv_style_set_radius(&comp->style_list, 16);
    lv_style_set_pad_all(&comp->style_list, WIFI_LIST_PAD);
    lv_style_set_pad_row(&comp->style_list, WIFI_ROW_GAP);

    lv_style_init(&comp->style_item);
    lv_style_set_bg_color(&comp->style_item, lv_color_hex(0x1f2937));
    lv_style_set_bg_opa(&comp->style_item, LV_OPA_COVER);
    lv_style_set_border_width(&comp->style_item, 0);
    lv_style_set_radius(&comp->style_item, 12);
    lv_style_set_pad_all(&comp->style_item, 0);
    lv_style_set_text_color(&comp->style_item, lv_color_hex(0xffffff));
    lv_style_set_text_font(&comp->style_item, &lv_font_Regular_20);
    lv_style_set_min_height(&comp->style_item, WIFI_ROW_H);

    lv_style_init(&comp->style_item_pressed);
    lv_style_set_bg_color(&comp->style_item_pressed, lv_color_hex(0x2563eb));
    lv_style_set_bg_opa(&comp->style_item_pressed, LV_OPA_COVER);

    lv_style_init(&comp->style_status);
    lv_style_set_text_color(&comp->style_status, lv_color_hex(0xd1d5db));
    lv_style_set_text_font(&comp->style_status, &lv_font_Regular_20);

    lv_style_init(&comp->style_popup_mask);
    lv_style_set_bg_color(&comp->style_popup_mask, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&comp->style_popup_mask, LV_OPA_60);
    lv_style_set_border_width(&comp->style_popup_mask, 0);

    lv_style_init(&comp->style_popup);
    lv_style_set_bg_color(&comp->style_popup, lv_color_hex(0x111827));
    lv_style_set_bg_opa(&comp->style_popup, LV_OPA_COVER);
    lv_style_set_border_color(&comp->style_popup, lv_color_hex(0x374151));
    lv_style_set_border_width(&comp->style_popup, 2);
    lv_style_set_radius(&comp->style_popup, 18);
    lv_style_set_pad_all(&comp->style_popup, 22);

    lv_style_init(&comp->style_textarea);
    lv_style_set_bg_color(&comp->style_textarea, lv_color_hex(0x0f172a));
    lv_style_set_bg_opa(&comp->style_textarea, LV_OPA_COVER);
    lv_style_set_border_color(&comp->style_textarea, lv_color_hex(0x475569));
    lv_style_set_border_width(&comp->style_textarea, 1);
    lv_style_set_radius(&comp->style_textarea, 10);
    lv_style_set_text_color(&comp->style_textarea, lv_color_hex(0xffffff));
    lv_style_set_text_font(&comp->style_textarea, &lv_font_Regular_20);
}

static void wifi_list_style_reset(lv_wifi_list_component_t *comp)
{
    if (!comp) return;
    lv_style_reset(&comp->style_root);
    lv_style_reset(&comp->style_header);
    lv_style_reset(&comp->style_title);
    lv_style_reset(&comp->style_btn);
    lv_style_reset(&comp->style_btn_pressed);
    lv_style_reset(&comp->style_list);
    lv_style_reset(&comp->style_item);
    lv_style_reset(&comp->style_item_pressed);
    lv_style_reset(&comp->style_status);
    lv_style_reset(&comp->style_popup_mask);
    lv_style_reset(&comp->style_popup);
    lv_style_reset(&comp->style_textarea);
}

static void item_ctx_delete_cb(lv_event_t *e)
{
    wifi_item_ctx_t *ctx = (wifi_item_ctx_t *)lv_event_get_user_data(e);
    if (ctx) free(ctx);
}

static void wifi_connect_check_timer_cb(lv_timer_t *timer)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)timer->user_data;
    if (!comp) return;

    wifi_list_update_connected_ssid(comp);

    if (ssid_is_same(comp->connected_ssid, comp->pending_connect_ssid)) {
        WIFI_LIST_LOG("connect success ssid=%s", comp->connected_ssid);
        if (comp->status) {
            lv_label_set_text_fmt(comp->status, "连接成功%s", comp->connected_ssid);
        }
        wifi_list_render(comp);
        wifi_connect_check_stop(comp);
        return;
    }

    comp->connect_check_left--;

    if (comp->connected_ssid[0] == '\0' &&
        comp->pending_connect_ssid[0] != '\0' &&
        comp->connect_check_left <= WIFI_CONNECT_CHECK_MAX_COUNT - WIFI_CONNECT_ASSUME_SUCCESS_AFTER_COUNT) {
        safe_copy(comp->connected_ssid, sizeof(comp->connected_ssid), comp->pending_connect_ssid);
        WIFI_LIST_LOG("connect success by target cache ssid=%s", comp->connected_ssid);
        if (comp->status) {
            lv_label_set_text_fmt(comp->status, "连接成功%s", comp->connected_ssid);
        }
        wifi_list_render(comp);
        wifi_connect_check_stop(comp);
        return;
    }

    if (comp->connect_check_left <= 0) {
        WIFI_LIST_LOG("connect timeout target=%s current=%s",
                      comp->pending_connect_ssid,
                      comp->connected_ssid[0] ? comp->connected_ssid : "<none>");

        if (comp->status) {
            if (comp->connected_ssid[0] != '\0') {
                lv_label_set_text_fmt(comp->status,
                                      "连接失败%s，当前已连接%s",
                                      comp->pending_connect_ssid,
                                      comp->connected_ssid);
            } else {
                lv_label_set_text_fmt(comp->status,
                                      "连接失败%s",
                                      comp->pending_connect_ssid);
            }
        }
        wifi_list_render(comp);
        wifi_connect_check_stop(comp);
        return;
    }

    if (comp->status) {
        lv_label_set_text_fmt(comp->status,
                              "正在连接%s...",
                              comp->pending_connect_ssid);
    }
}

static void wifi_connect_check_stop(lv_wifi_list_component_t *comp)
{
    if (!comp) return;

    if (comp->connect_check_timer) {
        lv_timer_del(comp->connect_check_timer);
        comp->connect_check_timer = NULL;
    }

    comp->connect_check_left = 0;
    comp->pending_connect_ssid[0] = '\0';
}

static void wifi_connect_check_start(lv_wifi_list_component_t *comp, const char *ssid)
{
    if (!comp || !ssid || ssid[0] == '\0') return;

    wifi_connect_check_stop(comp);

    safe_copy(comp->pending_connect_ssid, sizeof(comp->pending_connect_ssid), ssid);
    comp->connect_check_left = WIFI_CONNECT_CHECK_MAX_COUNT;

    wifi_list_update_connected_ssid(comp);
    if (ssid_is_same(comp->connected_ssid, comp->pending_connect_ssid)) {
        WIFI_LIST_LOG("connect success immediately ssid=%s", comp->connected_ssid);
        if (comp->status) {
            lv_label_set_text_fmt(comp->status, "连接成功%s", comp->connected_ssid);
        }
        wifi_list_render(comp);
        comp->pending_connect_ssid[0] = '\0';
        comp->connect_check_left = 0;
        return;
    }

    if (comp->status) {
        lv_label_set_text_fmt(comp->status, "正在连接%s...", comp->pending_connect_ssid);
    }

    comp->connect_check_timer = lv_timer_create(wifi_connect_check_timer_cb,
                                                WIFI_CONNECT_CHECK_INTERVAL_MS,
                                                comp);
}

static void connect_ok_cb(lv_event_t *e)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)lv_event_get_user_data(e);
    if (!comp) return;

    const char *pwd = "";
    if (comp->password_ta) {
        pwd = lv_textarea_get_text(comp->password_ta);
    }

    safe_copy(comp->password, sizeof(comp->password), pwd);

    WIFI_LIST_LOG("connect click ssid=%s password_len=%u",
                  comp->selected_ssid,
                  (unsigned)strlen(comp->password));

    if (comp->status) {
        lv_label_set_text_fmt(comp->status, "正在连接: %s", comp->selected_ssid);
    }

    bool ok = false;
    if (comp->connect_cb) {
        ok = comp->connect_cb(comp->selected_ssid, comp->password, comp->user_data);
        WIFI_LIST_LOG("connect result ssid=%s ok=%d", comp->selected_ssid, ok ? 1 : 0);
    } else {
        WIFI_LIST_LOG("connect_cb is NULL, only print ssid=%s", comp->selected_ssid);
    }

    if (ok) {
        wifi_password_popup_close(comp);
        wifi_connect_check_start(comp, comp->selected_ssid);
    } else {
        wifi_connect_check_stop(comp);
        if (comp->status) {
            lv_label_set_text_fmt(comp->status, "连接失败%s", comp->selected_ssid);
        }
        wifi_list_render(comp);
        wifi_password_popup_close(comp);
    }
}

static void connect_cancel_cb(lv_event_t *e)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)lv_event_get_user_data(e);
    if (!comp) return;

    WIFI_LIST_LOG("connect cancel ssid=%s", comp->selected_ssid);
    wifi_password_popup_close(comp);
}

static void item_click_cb(lv_event_t *e)
{
    wifi_item_ctx_t *ctx = (wifi_item_ctx_t *)lv_event_get_user_data(e);
    if (!ctx || !ctx->comp) return;

    lv_wifi_list_component_t *comp = ctx->comp;

    WIFI_LIST_LOG("click ssid=%s", ctx->ssid);

    if (comp->click_cb) {
        comp->click_cb(ctx->ssid, comp->user_data);
    }

    wifi_password_popup_open(comp, ctx->ssid);
}

static void refresh_click_cb(lv_event_t *e)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)lv_event_get_user_data(e);
    if (!comp) return;

    if (comp->scan_in_progress) {
        WIFI_LIST_LOG("refresh ignored: scan in progress");
        return;
    }

    WIFI_LIST_LOG("refresh button click");
    wifi_list_refresh_impl(comp);
}

static void prev_click_cb(lv_event_t *e)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)lv_event_get_user_data(e);
    if (!comp) return;

    if (comp->page_index <= 0) return;
    comp->page_index--;
    WIFI_LIST_LOG("prev page=%d", comp->page_index);
    wifi_list_render(comp);
}

static void next_click_cb(lv_event_t *e)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)lv_event_get_user_data(e);
    if (!comp) return;

    int pages = page_count_get(comp);
    if (comp->page_index >= pages - 1) return;
    comp->page_index++;
    WIFI_LIST_LOG("next page=%d", comp->page_index);
    wifi_list_render(comp);
}

static lv_obj_t *create_button(lv_wifi_list_component_t *comp,
                               lv_obj_t *parent,
                               const char *text,
                               int w,
                               int h)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    lv_obj_add_style(btn, &comp->style_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(btn, &comp->style_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(btn);
    lv_obj_add_style(label, &comp->style_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label, text);
    lv_obj_center(label);

    return btn;
}

static void wifi_password_popup_close(lv_wifi_list_component_t *comp)
{
    if (!comp) return;

    if (comp->popup_mask) {
        lv_obj_del(comp->popup_mask);
        comp->popup_mask = NULL;
        comp->popup = NULL;
        comp->popup_title = NULL;
        comp->password_ta = NULL;
        comp->password_toggle_btn = NULL;
        comp->password_visible = false;
#if LV_USE_KEYBOARD
        comp->keyboard = NULL;
#endif
    }
}

static void password_toggle_cb(lv_event_t *e)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)lv_event_get_user_data(e);
    if (!comp || !comp->password_ta || !comp->password_toggle_btn) return;

    comp->password_visible = !comp->password_visible;
    lv_textarea_set_password_mode(comp->password_ta, comp->password_visible ? false : true);

    lv_obj_t *label = lv_obj_get_child(comp->password_toggle_btn, 0);
    if (label) {
        lv_label_set_text(label, comp->password_visible ? "隐藏" : "查看");
        lv_obj_center(label);
    }

    WIFI_LIST_LOG("password visible=%d", comp->password_visible ? 1 : 0);
}

static void wifi_password_popup_open(lv_wifi_list_component_t *comp, const char *ssid)
{
    if (!comp || !ssid) return;

    wifi_password_popup_close(comp);

    safe_copy(comp->selected_ssid, sizeof(comp->selected_ssid), ssid);

    comp->popup_mask = lv_obj_create(lv_layer_top());
    lv_obj_set_size(comp->popup_mask, LV_PCT(100), LV_PCT(100));
    lv_obj_align(comp->popup_mask, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(comp->popup_mask, &comp->style_popup_mask, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(comp->popup_mask, LV_OBJ_FLAG_SCROLLABLE);

    comp->popup = lv_obj_create(comp->popup_mask);
    lv_obj_set_size(comp->popup, WIFI_POPUP_W, WIFI_POPUP_H);
    lv_obj_align(comp->popup, LV_ALIGN_CENTER, 0, -170);
    lv_obj_add_style(comp->popup, &comp->style_popup, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(comp->popup, LV_OBJ_FLAG_SCROLLABLE);

    comp->popup_title = lv_label_create(comp->popup);
    lv_obj_add_style(comp->popup_title, &comp->style_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(comp->popup_title, "连接 WiFi：%s", comp->selected_ssid);
    lv_label_set_long_mode(comp->popup_title, LV_LABEL_LONG_DOT);
    lv_obj_set_width(comp->popup_title, WIFI_POPUP_W - 44);
    lv_obj_align(comp->popup_title, LV_ALIGN_TOP_LEFT, 0, 0);

    comp->password_ta = lv_textarea_create(comp->popup);
    lv_obj_set_size(comp->password_ta, WIFI_POPUP_W - 176, 58);
    lv_obj_align(comp->password_ta, LV_ALIGN_TOP_LEFT, 0, 76);
    lv_obj_add_style(comp->password_ta, &comp->style_textarea, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(comp->password_ta, true);
    lv_textarea_set_password_mode(comp->password_ta, true);
    lv_textarea_set_placeholder_text(comp->password_ta, "请输入 WiFi 密码");
    lv_textarea_set_text(comp->password_ta, "");
    comp->password_visible = false;

    comp->password_toggle_btn = create_button(comp, comp->popup, "查看", 112, 58);
    lv_obj_align(comp->password_toggle_btn, LV_ALIGN_TOP_RIGHT, 0, 76);
    lv_obj_add_event_cb(comp->password_toggle_btn, password_toggle_cb, LV_EVENT_CLICKED, comp);

#if LV_USE_KEYBOARD
    comp->keyboard = lv_keyboard_create(comp->popup_mask);
    lv_obj_set_size(comp->keyboard, 760, 230);
    lv_obj_align(comp->keyboard, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_keyboard_set_textarea(comp->keyboard, comp->password_ta);
#endif

    lv_obj_t *btn_cancel = create_button(comp, comp->popup, "取消", WIFI_POPUP_BTN_W, WIFI_POPUP_BTN_H);
    lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_LEFT, 70, 0);
    lv_obj_add_event_cb(btn_cancel, connect_cancel_cb, LV_EVENT_CLICKED, comp);

    lv_obj_t *btn_ok = create_button(comp, comp->popup, "连接", WIFI_POPUP_BTN_W, WIFI_POPUP_BTN_H);
    lv_obj_align(btn_ok, LV_ALIGN_BOTTOM_RIGHT, -70, 0);
    lv_obj_add_event_cb(btn_ok, connect_ok_cb, LV_EVENT_CLICKED, comp);

    lv_obj_add_state(comp->password_ta, LV_STATE_FOCUSED);
    lv_obj_scroll_to_view(comp->password_ta, LV_ANIM_OFF);

    WIFI_LIST_LOG("open password popup ssid=%s", comp->selected_ssid);
}

static void add_empty_wifi_row(lv_wifi_list_component_t *comp)
{
    if (!comp || !comp->list) return;

    lv_obj_t *row = lv_obj_create(comp->list);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, WIFI_ROW_H);
    lv_obj_add_style(row, &comp->style_item, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(row, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
}

static void add_wifi_item(lv_wifi_list_component_t *comp, const lv_wifi_list_ap_t *ap)
{
    if (!comp || !ap || ap->ssid[0] == '\0') return;

    lv_obj_t *btn = lv_btn_create(comp->list);
    lv_obj_set_width(btn, LV_PCT(100));
    lv_obj_set_height(btn, WIFI_ROW_H);
    lv_obj_add_style(btn, &comp->style_item, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(btn, &comp->style_item_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *icon = lv_img_create(btn);
    lv_img_set_src(icon, wifi_signal_img_src(ap->rssi));
    lv_obj_set_size(icon, WIFI_ITEM_ICON_W, WIFI_ITEM_ICON_H);
    lv_obj_set_style_bg_opa(icon, WIFI_ITEM_ICON_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(icon, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, WIFI_ITEM_ICON_X, WIFI_ITEM_ICON_Y);

    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_width(label, LV_PCT(54));
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_label_set_text(label, ap->ssid);
    lv_obj_set_style_text_font(label, &lv_font_Regular_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(label, WIFI_ITEM_LABEL_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(label, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, WIFI_ITEM_LABEL_X, WIFI_ITEM_LABEL_Y);

    if (ssid_is_same(ap->ssid, comp->connected_ssid)) {
        lv_obj_t *connected = lv_label_create(btn);
        lv_obj_set_width(connected, WIFI_ITEM_CONNECTED_W);
        lv_label_set_long_mode(connected, LV_LABEL_LONG_DOT);
        lv_label_set_text_fmt(connected, "已连接%s", comp->connected_ssid);
        lv_obj_set_style_text_font(connected, &lv_font_Regular_20, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(connected, lv_color_hex(0x22c55e), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(connected, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(connected, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(connected, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align(connected, LV_ALIGN_RIGHT_MID, WIFI_ITEM_CONNECTED_X, WIFI_ITEM_CONNECTED_Y);
    }

    wifi_item_ctx_t *ctx = (wifi_item_ctx_t *)calloc(1, sizeof(wifi_item_ctx_t));
    if (!ctx) {
        WIFI_LIST_LOG("calloc item ctx failed");
        return;
    }

    ctx->comp = comp;
    safe_copy(ctx->ssid, sizeof(ctx->ssid), ap->ssid);

    lv_obj_add_event_cb(btn, item_click_cb, LV_EVENT_CLICKED, ctx);
    lv_obj_add_event_cb(btn, item_ctx_delete_cb, LV_EVENT_DELETE, ctx);
}

static void wifi_list_render(lv_wifi_list_component_t *comp)
{
    if (!comp || !comp->list) return;

    lv_obj_clean(comp->list);

    int pages = page_count_get(comp);
    if (comp->page_index < 0) comp->page_index = 0;
    if (comp->page_index > pages - 1) comp->page_index = pages - 1;

    if (comp->item_count <= 0) {
        lv_obj_t *label = lv_label_create(comp->list);
        lv_obj_add_style(label, &comp->style_status, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(label, "未扫描到 WiFi");
        lv_obj_center(label);
    } else {
        int start = comp->page_index * comp->page_size;

        /* 固定每页 7 行。最后一页真实 WiFi 不够时补空行，避免页面高度一会 7 行一会 5 行。 */
        for (int slot = 0; slot < comp->page_size; slot++) {
            int i = start + slot;
            if (i < comp->item_count && ssid_is_valid_visible(comp->items[i].ssid)) {
                add_wifi_item(comp, &comp->items[i]);
            } else {
                add_empty_wifi_row(comp);
            }
        }
    }

    if (comp->page_label) {
        lv_label_set_text_fmt(comp->page_label, "%d / %d", comp->page_index + 1, pages);
    }

    set_btn_disabled(comp->btn_prev, comp->page_index <= 0);
    set_btn_disabled(comp->btn_next, comp->page_index >= pages - 1);
}

static int wifi_list_refresh_impl(lv_wifi_list_component_t *comp)
{
    if (!comp) return -1;

    if (comp->scan_in_progress) {
        WIFI_LIST_LOG("scan ignored: already in progress");
        return -1;
    }

    if (!comp->scan_cb) {
        WIFI_LIST_LOG("scan_cb is NULL");
        if (comp->status) lv_label_set_text(comp->status, "scan_cb 未设置");
        return -1;
    }

    if (comp->scan_retry_timer) {
        lv_timer_del(comp->scan_retry_timer);
        comp->scan_retry_timer = NULL;
    }

    wifi_scan_ui_set_busy(comp, true);
    /*
     * 扫描前不要主动查询当前SSID。
     * 某些板端查询SSID会间接触发 scan，导致打开一次页面出现多次扫描。
     * 这里直接进入真正的 scan_cb，保证一次刷新只调用一次扫描。
     */
    if (comp->status) {
        if (comp->connected_ssid[0] != '\0') {
            lv_label_set_text_fmt(comp->status, "已连接%s，正在扫描 WiFi...", comp->connected_ssid);
        } else {
            lv_label_set_text(comp->status, "正在扫描 WiFi...");
        }
    }

    lv_wifi_list_ap_t *scan_items = (lv_wifi_list_ap_t *)calloc(comp->max_items, sizeof(lv_wifi_list_ap_t));
    if (!scan_items) {
        WIFI_LIST_LOG("scan calloc failed");
        if (comp->status) lv_label_set_text(comp->status, "WiFi 扫描失败：内存不足");
        wifi_scan_ui_set_busy(comp, false);
        return -1;
    }

    int count = comp->scan_cb(scan_items, comp->max_items, comp->user_data);
    if (count == LV_WIFI_LIST_SCAN_PENDING) {
        free(scan_items);
        WIFI_LIST_LOG("scan pending in background");
        if (comp->status) {
            if (comp->connected_ssid[0] != '\0') {
                lv_label_set_text_fmt(comp->status, "已连接%s，正在后台扫描 WiFi...", comp->connected_ssid);
            } else {
                lv_label_set_text(comp->status, "正在后台扫描 WiFi...");
            }
        }
        /* 后台线程完成后会再次调用 lv_wifi_list_refresh() 取结果。这里不能阻塞 UI。 */
        wifi_scan_ui_set_busy(comp, false);
        return LV_WIFI_LIST_SCAN_PENDING;
    }

    if (count < 0) {
        free(scan_items);
        comp->item_count = 0;
        comp->page_index = 0;
        WIFI_LIST_LOG("scan failed");
        if (comp->status) {
            lv_label_set_text(comp->status, "WiFi 扫描失败，请点刷新重试");
        }
        wifi_list_render(comp);
        wifi_scan_ui_set_busy(comp, false);
        return -1;
    }

    if (count > comp->max_items) count = comp->max_items;
    memset(comp->items, 0, sizeof(lv_wifi_list_ap_t) * comp->max_items);

    int visible_count = 0;
    for (int i = 0; i < count && visible_count < comp->max_items; i++) {
        if (!ssid_is_valid_visible(scan_items[i].ssid)) {
            continue;
        }
        comp->items[visible_count++] = scan_items[i];
    }
    free(scan_items);

    comp->item_count = visible_count;
    comp->page_index = 0;

    WIFI_LIST_LOG("scan ok count=%d page_size=%d", comp->item_count, comp->page_size);

    wifi_list_update_connected_ssid(comp);

    if (comp->status) {
        if (comp->connected_ssid[0] != '\0') {
            lv_label_set_text_fmt(comp->status, "已连接%s", comp->connected_ssid);
        } else {
            lv_label_set_text_fmt(comp->status, "未连接 WiFi，扫描完成，共 %d 个 WiFi", comp->item_count);
        }
    }

    wifi_list_render(comp);
    wifi_scan_ui_set_busy(comp, false);
    return comp->item_count;
}

static void wifi_scan_retry_timer_cb(lv_timer_t *timer)
{
    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)timer->user_data;
    if (!comp) return;

    comp->scan_retry_timer = NULL;
    WIFI_LIST_LOG("auto retry scan");
    (void)wifi_list_refresh_impl(comp);
}

lv_wifi_list_component_t *lv_wifi_list_create(lv_obj_t *screen,
                                              const lv_wifi_list_cfg_t *cfg)
{
    if (!screen || !cfg) {
        WIFI_LIST_LOG("create failed: invalid args");
        return NULL;
    }

    lv_wifi_list_component_t *comp = (lv_wifi_list_component_t *)calloc(1, sizeof(lv_wifi_list_component_t));
    if (!comp) {
        WIFI_LIST_LOG("create failed: calloc comp");
        return NULL;
    }

    comp->max_items = cfg->max_items > 0 ? cfg->max_items : LV_WIFI_LIST_DEFAULT_MAX_ITEMS;
    comp->page_size = WIFI_PAGE_SIZE;
    comp->page_index = 0;
    comp->scan_cb = cfg->scan_cb;
    comp->connect_cb = cfg->connect_cb;
    comp->click_cb = cfg->click_cb;
    comp->get_connected_ssid_cb = cfg->get_connected_ssid_cb;
    comp->user_data = cfg->user_data;
    comp->password[0] = '\0';

    comp->items = (lv_wifi_list_ap_t *)calloc(comp->max_items, sizeof(lv_wifi_list_ap_t));
    if (!comp->items) {
        free(comp);
        WIFI_LIST_LOG("create failed: calloc items");
        return NULL;
    }

    wifi_list_style_init(comp);

    int w = cfg->w > 0 ? cfg->w : LV_WIFI_LIST_DEFAULT_W;
    int h = cfg->h > 0 ? cfg->h : LV_WIFI_LIST_DEFAULT_H;
    int x = cfg->x;
    int y = cfg->y;

    comp->root = lv_obj_create(screen);
    lv_obj_set_size(comp->root, w, h);
    lv_obj_set_pos(comp->root, x, y);
    lv_obj_add_style(comp->root, &comp->style_root, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(comp->root, LV_OBJ_FLAG_SCROLLABLE);

    comp->header = lv_obj_create(comp->root);
    lv_obj_set_size(comp->header, LV_PCT(100), WIFI_HEADER_H);
    lv_obj_align(comp->header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(comp->header, &comp->style_header, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(comp->header, LV_OBJ_FLAG_SCROLLABLE);

    comp->title = lv_label_create(comp->header);
    lv_obj_add_style(comp->title, &comp->style_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(comp->title, "WiFi 列表");
    lv_obj_align(comp->title, LV_ALIGN_LEFT_MID, 10, 0);

    comp->btn_refresh = create_button(comp, comp->header, "刷新", WIFI_BTN_W, WIFI_BTN_H);
    lv_obj_align(comp->btn_refresh, LV_ALIGN_RIGHT_MID, -8, 0);
    lv_obj_add_event_cb(comp->btn_refresh, refresh_click_cb, LV_EVENT_CLICKED, comp);

    comp->status = lv_label_create(comp->root);
    lv_obj_add_style(comp->status, &comp->style_status, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(comp->status, "等待扫描");
    lv_obj_align(comp->status, LV_ALIGN_TOP_LEFT, 24, WIFI_HEADER_H + 10);

    comp->scan_spinner = lv_spinner_create(comp->root, 1000, 60);
    lv_obj_set_size(comp->scan_spinner, 26, 26);
    lv_obj_align(comp->scan_spinner, LV_ALIGN_TOP_RIGHT, -24, WIFI_HEADER_H + 14);
    lv_obj_add_flag(comp->scan_spinner, LV_OBJ_FLAG_HIDDEN);

    int list_h = WIFI_LIST_PAD * 2 + WIFI_PAGE_SIZE * WIFI_ROW_H + (WIFI_PAGE_SIZE - 1) * WIFI_ROW_GAP;
    int max_list_h = h - WIFI_HEADER_H - WIFI_STATUS_H - WIFI_FOOTER_H - 42;
    if (max_list_h > 0 && list_h > max_list_h) list_h = max_list_h;

    comp->list = lv_obj_create(comp->root);
    lv_obj_add_style(comp->list, &comp->style_list, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(comp->list, LV_PCT(100), list_h);
    lv_obj_align(comp->list, LV_ALIGN_TOP_MID, 0, WIFI_HEADER_H + WIFI_STATUS_H + 10);
    lv_obj_clear_flag(comp->list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(comp->list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(comp->list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(comp->list,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    comp->footer = lv_obj_create(comp->root);
    lv_obj_set_size(comp->footer, LV_PCT(100), WIFI_FOOTER_H);
    lv_obj_align(comp->footer, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(comp->footer, &comp->style_header, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(comp->footer, LV_OBJ_FLAG_SCROLLABLE);

    comp->btn_prev = create_button(comp, comp->footer, "上一页", WIFI_PAGE_BTN_W, WIFI_PAGE_BTN_H);
    lv_obj_align(comp->btn_prev, LV_ALIGN_LEFT_MID, 14, 0);
    lv_obj_add_event_cb(comp->btn_prev, prev_click_cb, LV_EVENT_CLICKED, comp);

    comp->page_label = lv_label_create(comp->footer);
    lv_obj_add_style(comp->page_label, &comp->style_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(comp->page_label, 180);
    lv_obj_set_style_text_align(comp->page_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(comp->page_label, "1 / 1");
    lv_obj_align(comp->page_label, LV_ALIGN_CENTER, 0, 0);

    comp->btn_next = create_button(comp, comp->footer, "下一页", WIFI_PAGE_BTN_W, WIFI_PAGE_BTN_H);
    lv_obj_align(comp->btn_next, LV_ALIGN_RIGHT_MID, -14, 0);
    lv_obj_add_event_cb(comp->btn_next, next_click_cb, LV_EVENT_CLICKED, comp);

    WIFI_LIST_LOG("create ok max_items=%d page_size=%d", comp->max_items, comp->page_size);

    if (cfg->auto_scan_on_create) {
        /*
         * 创建后自动扫描时，不先查SSID。
         * 避免 app_wifi_list_open() 触发：查SSID一次 + 扫描一次 + 查SSID一次。
         */
        wifi_list_refresh_impl(comp);
    } else {
        wifi_list_update_connected_ssid(comp);
        wifi_list_set_status_idle(comp, "未连接 WiFi，等待扫描");
        wifi_list_render(comp);
    }

    return comp;
}

void lv_wifi_list_destroy(lv_wifi_list_component_t **pcomp)
{
    if (!pcomp || !*pcomp) return;

    lv_wifi_list_component_t *comp = *pcomp;
    WIFI_LIST_LOG("destroy");

    wifi_connect_check_stop(comp);
    if (comp->scan_retry_timer) {
        lv_timer_del(comp->scan_retry_timer);
        comp->scan_retry_timer = NULL;
    }
    wifi_password_popup_close(comp);

    if (comp->root) {
        lv_obj_del(comp->root);
        comp->root = NULL;
    }

    free(comp->items);
    comp->items = NULL;

    wifi_list_style_reset(comp);

    free(comp);
    *pcomp = NULL;
}

int lv_wifi_list_refresh(lv_wifi_list_component_t *comp)
{
    return wifi_list_refresh_impl(comp);
}

lv_obj_t *lv_wifi_list_get_root(lv_wifi_list_component_t *comp)
{
    return comp ? comp->root : NULL;
}

void lv_wifi_list_bring_to_front(lv_wifi_list_component_t *comp)
{
    if (!comp) return;

    if (comp->root) {
        lv_obj_move_foreground(comp->root);
    }

    if (comp->popup_mask) {
        lv_obj_move_foreground(comp->popup_mask);
    }
}

void lv_wifi_list_set_password(lv_wifi_list_component_t *comp,
                               const char *password)
{
    if (!comp) return;
    safe_copy(comp->password, sizeof(comp->password), password ? password : "");
    WIFI_LIST_LOG("set password len=%u", (unsigned)strlen(comp->password));
}


void lv_wifi_list_refresh_connected_ssid(lv_wifi_list_component_t *comp)
{
    if (!comp) return;

    wifi_list_update_connected_ssid(comp);
    wifi_list_set_status_idle(comp, "未连接 WiFi");
    wifi_list_render(comp);
}
