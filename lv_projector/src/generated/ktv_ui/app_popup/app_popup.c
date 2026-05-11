#include "app_popup.h"

#include "lvgl/lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef APP_POPUP_LOG_ENABLE
#define APP_POPUP_LOG_ENABLE 1
#endif

#ifndef APP_POPUP_WIDTH
#define APP_POPUP_WIDTH 620
#endif

#ifndef APP_POPUP_HEIGHT
#define APP_POPUP_HEIGHT 96
#endif

#ifndef APP_POPUP_MAX_TEXT_LEN
#define APP_POPUP_MAX_TEXT_LEN 256
#endif

#ifndef APP_POPUP_Y_OFFSET
#define APP_POPUP_Y_OFFSET 0
#endif

#if APP_POPUP_LOG_ENABLE
#define POPUP_LOG(fmt, ...) printf("[popup] " fmt "\n", ##__VA_ARGS__)
#else
#define POPUP_LOG(fmt, ...) ((void)0)
#endif

/* 如果你的工程没有这个字体，改成 LV_FONT_DEFAULT 即可。 */
LV_FONT_DECLARE(lv_font_Regular_20);

typedef struct {
    lv_obj_t *root;
    lv_obj_t *label;
    lv_timer_t *hide_timer;
    uint8_t inited;
    uint8_t visible;
} app_popup_ctx_t;

typedef struct {
    char text[APP_POPUP_MAX_TEXT_LEN];
    uint32_t hide_ms;
} app_popup_show_req_t;

static app_popup_ctx_t g_popup;

static void popup_del_hide_timer(void)
{
    if (g_popup.hide_timer != NULL) {
        lv_timer_del(g_popup.hide_timer);
        g_popup.hide_timer = NULL;
    }
}

static lv_obj_t *popup_get_parent(void)
{
    lv_obj_t *parent = lv_layer_top();
    if (parent == NULL) {
        parent = lv_scr_act();
    }
    return parent;
}

static void popup_delete_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);

    if (target != g_popup.root) return;

    popup_del_hide_timer();
    memset(&g_popup, 0, sizeof(g_popup));
}

static int popup_lazy_init(void)
{
    lv_obj_t *parent;

    if (g_popup.inited && g_popup.root != NULL && g_popup.label != NULL) {
        return 0;
    }

    memset(&g_popup, 0, sizeof(g_popup));

    parent = popup_get_parent();
    if (parent == NULL) {
        POPUP_LOG("parent is NULL");
        return -1;
    }

    g_popup.root = lv_obj_create(parent);
    if (g_popup.root == NULL) {
        POPUP_LOG("root create failed");
        return -2;
    }

    lv_obj_set_size(g_popup.root, APP_POPUP_WIDTH, APP_POPUP_HEIGHT);

    /* 关键：本版先做成明显可见，不用透明动画。 */
    lv_obj_set_style_opa(g_popup.root, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_popup.root, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_popup.root, lv_color_hex(0xE53935), LV_PART_MAIN);
    lv_obj_set_style_border_width(g_popup.root, 4, LV_PART_MAIN);
    lv_obj_set_style_border_color(g_popup.root, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_radius(g_popup.root, 20, LV_PART_MAIN);
    lv_obj_set_style_pad_left(g_popup.root, 24, LV_PART_MAIN);
    lv_obj_set_style_pad_right(g_popup.root, 24, LV_PART_MAIN);
    lv_obj_set_style_pad_top(g_popup.root, 16, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(g_popup.root, 16, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(g_popup.root, 0, LV_PART_MAIN);

    lv_obj_clear_flag(g_popup.root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(g_popup.root, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(g_popup.root, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(g_popup.root, popup_delete_event_cb, LV_EVENT_DELETE, NULL);

    g_popup.label = lv_label_create(g_popup.root);
    if (g_popup.label == NULL) {
        lv_obj_del(g_popup.root);
        memset(&g_popup, 0, sizeof(g_popup));
        POPUP_LOG("label create failed");
        return -3;
    }

    lv_obj_set_width(g_popup.label, APP_POPUP_WIDTH - 48);
    lv_label_set_long_mode(g_popup.label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(g_popup.label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_popup.label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_opa(g_popup.label, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_text_font(g_popup.label, &lv_font_Regular_20, LV_PART_MAIN);
    lv_obj_center(g_popup.label);

    g_popup.inited = 1U;

    POPUP_LOG("lazy init ok parent=%p root=%p label=%p", (void *)parent, (void *)g_popup.root, (void *)g_popup.label);
    return 0;
}

static void popup_force_show(const char *text)
{
    lv_obj_t *parent;

    if (g_popup.root == NULL || g_popup.label == NULL) return;

    parent = popup_get_parent();
    if (parent != NULL && lv_obj_get_parent(g_popup.root) != parent) {
        lv_obj_set_parent(g_popup.root, parent);
    }

    if (text == NULL) text = "";
    lv_label_set_text(g_popup.label, text);

    /* 关键：强制显示在屏幕中央，先排除坐标/底部被遮挡问题。 */
    lv_obj_clear_flag(g_popup.root, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_popup.label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_opa(g_popup.root, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_popup.root, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_align(g_popup.root, LV_ALIGN_CENTER, 0, APP_POPUP_Y_OFFSET);
    lv_obj_center(g_popup.label);
    lv_obj_move_foreground(g_popup.root);

    lv_obj_update_layout(g_popup.root);
    lv_obj_invalidate(g_popup.root);
    lv_obj_invalidate(lv_obj_get_parent(g_popup.root));

#if LVGL_VERSION_MAJOR >= 8
    lv_refr_now(NULL);
#endif

    g_popup.visible = 1U;

    POPUP_LOG("force show ok text=%s parent=%p root=%p x=%d y=%d w=%d h=%d hidden=%d",
              text,
              (void *)lv_obj_get_parent(g_popup.root),
              (void *)g_popup.root,
              (int)lv_obj_get_x(g_popup.root),
              (int)lv_obj_get_y(g_popup.root),
              (int)lv_obj_get_width(g_popup.root),
              (int)lv_obj_get_height(g_popup.root),
              (int)lv_obj_has_flag(g_popup.root, LV_OBJ_FLAG_HIDDEN));
}

static void popup_force_hide(void)
{
    popup_del_hide_timer();

    if (g_popup.root != NULL) {
        lv_obj_add_flag(g_popup.root, LV_OBJ_FLAG_HIDDEN);
        lv_obj_invalidate(g_popup.root);
    }

    g_popup.visible = 0U;
    POPUP_LOG("hide ok");
}

static void popup_hide_timer_cb(lv_timer_t *timer)
{
    if (timer == g_popup.hide_timer) {
        g_popup.hide_timer = NULL;
    }

    lv_timer_del(timer);
    popup_force_hide();
}

static int popup_show_direct(const char *text, uint32_t hide_ms)
{
    int ret;

    ret = popup_lazy_init();
    if (ret != 0) {
        POPUP_LOG("lazy init failed ret=%d", ret);
        return ret;
    }

    popup_del_hide_timer();
    popup_force_show(text);

    if (hide_ms > 0U) {
        g_popup.hide_timer = lv_timer_create(popup_hide_timer_cb, hide_ms, NULL);
        if (g_popup.hide_timer == NULL) {
            POPUP_LOG("hide timer create failed");
            return -4;
        }
        lv_timer_set_repeat_count(g_popup.hide_timer, 1);
    }

    POPUP_LOG("show direct ok hide_ms=%u text=%s", hide_ms, text ? text : "");
    return 0;
}

static void popup_show_async_cb(void *user_data)
{
    app_popup_show_req_t *req = (app_popup_show_req_t *)user_data;

    if (req != NULL) {
        popup_show_direct(req->text, req->hide_ms);
        free(req);
    }
}

static void popup_hide_async_cb(void *user_data)
{
    (void)user_data;
    popup_force_hide();
}

static void popup_deinit_async_cb(void *user_data)
{
    (void)user_data;

    popup_del_hide_timer();

    if (g_popup.root != NULL) {
        lv_obj_del(g_popup.root);
    } else {
        memset(&g_popup, 0, sizeof(g_popup));
    }

    POPUP_LOG("deinit ok");
}

int app_popup_show(const char *text, uint32_t hide_ms)
{
    app_popup_show_req_t *req;

    req = (app_popup_show_req_t *)malloc(sizeof(app_popup_show_req_t));
    if (req == NULL) return -10;

    if (text == NULL) text = "";
    snprintf(req->text, sizeof(req->text), "%s", text);
    req->hide_ms = hide_ms;

    lv_async_call(popup_show_async_cb, req);

    POPUP_LOG("show posted hide_ms=%u text=%s", hide_ms, req->text);
    return 0;
}

void app_popup_hide(void)
{
    lv_async_call(popup_hide_async_cb, NULL);
}

void app_popup_deinit(void)
{
    lv_async_call(popup_deinit_async_cb, NULL);
}
