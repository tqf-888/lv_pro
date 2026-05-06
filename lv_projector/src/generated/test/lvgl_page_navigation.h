#ifndef LVGL_PAGE_NAV_H
#define LVGL_PAGE_NAV_H

#include "lvgl/lvgl.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 页面 ID 枚举（保持你原来的定义） */
typedef enum {
    PAGE_ID_HOME = 0,
    PAGE_ID_MUSIC,
    PAGE_ID_SETTINGS,
    PAGE_ID_SETTINGS1,
    PAGE_ID_SETTINGS11,
    PAGE_ID_SETTINGS111,
    PAGE_ID_MAX
} page_id_e;

/* 页面任务回调（简单版本：无参数，由定时器调用） */
typedef void (*page_task_cb)(void);

/* 初始化页面导航（注册默认任务等） */
void page_nav_init(void);

/* 设置当前页面 */
void page_nav_set_current(page_id_e id);

/* 获取当前页面 */
page_id_e page_nav_get_current(void);

/* 注册页面任务（初始化时调用） */
void page_nav_register_task(page_id_e id, page_task_cb task);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_PAGE_NAV_H */
