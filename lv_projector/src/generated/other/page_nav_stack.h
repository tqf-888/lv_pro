#ifndef PAGE_NAV_STACK_H
#define PAGE_NAV_STACK_H

#include <stdbool.h>
#include <stdint.h>
#include "lvgl/lvgl.h"
#include "gui_guider.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PAGE_NAV_STACK_MAX
#define PAGE_NAV_STACK_MAX 16
#endif

#ifndef PAGE_NAV_NAME_MAX
#define PAGE_NAV_NAME_MAX 64
#endif

typedef enum {
    PAGE_NAV_OK        = 0,
    PAGE_NAV_ERR_PARAM = -1,
    PAGE_NAV_ERR_EMPTY = -2,
    PAGE_NAV_ERR_FULL  = -3,
    PAGE_NAV_ERR_BUSY  = -4,
} page_nav_ret_t;

/*
 * 注册桌面。
 *
 * 规则：
 * - 只记录桌面，不跳转。
 * - 不调用 ui_load_scr_animation()。
 * - 不调用 setup_scr()。
 * - 只允许注册一次。
 * - 第二次调用直接返回 PAGE_NAV_ERR_BUSY。
 *
 * 注意：
 * - 调用前桌面 screen 应该已经存在。
 * - scr 必须是 &guider_ui.screen_xxx。
 * - scr_del 必须是 &guider_ui.screen_xxx_del。
 */
int page_nav_register_home(const char *page_name,
                           lv_obj_t **scr,
                           bool *scr_del,
                           ui_setup_scr_t setup_scr);

/*
 * 正向进入页面。
 *
 * 规则：
 * - 必须先 page_nav_register_home()。
 * - 内部调用 ui_load_scr_animation()。
 * - 固定无动画：
 *   LV_SCR_LOAD_ANIM_NONE, 0, 0, 0, 0
 */
int page_nav_push(const char *page_name,
                  lv_obj_t **new_scr,
                  bool *new_scr_del,
                  ui_setup_scr_t setup_scr);

/*
 * 返回上一页。
 *
 * 规则：
 * - depth <= 1 时失败，说明已经在桌面。
 * - 内部调用 ui_load_scr_animation()。
 * - 固定无动画：
 *   LV_SCR_LOAD_ANIM_NONE, 0, 0, 0, 0
 */
int page_nav_back(void);

int page_nav_get_depth(void);
const char *page_nav_get_current_name(void);

#ifdef __cplusplus
}
#endif

#endif /* PAGE_NAV_STACK_H */
