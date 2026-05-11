#ifndef APP_POPUP_H
#define APP_POPUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * 强制显示版 Toast / 弹窗组件
 *
 * 用法：
 *   app_popup_show("正在重播", 2500);
 *   app_popup_hide();
 *   app_popup_deinit();
 *
 * 特点：
 * - 懒初始化：第一次 show 才创建 LVGL 对象
 * - app_popup_show() 可从非 LVGL 线程调用，内部 lv_async_call 切回 LVGL 线程
 * - 默认挂 lv_layer_top()
 * - 显示时强制 move_foreground、clear hidden、set opa cover、invalidate、lv_refr_now
 * - 本版先取消动画，优先保证一定看得见
 */

int app_popup_show(const char *text, uint32_t hide_ms);
void app_popup_hide(void);
void app_popup_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_POPUP_H */
