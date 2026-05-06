#ifndef LVGL_APP_TIMER_H
#define LVGL_APP_TIMER_H

#include "lvgl/lvgl.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 定时器周期 (ms) */
#define APP_TIMER_PERIOD_MS    (1000)

/* 任务函数类型：参数是消息指针，可扩展 */
typedef void (*app_task_cb)(void *user_data);

/* 消息结构：可以按需扩展 */
typedef struct {
    volatile bool has_msg;      /* 是否有新消息 */
    char body[256];             /* 消息内容（例如图片路径） */
    int  value;                 /* 额外参数 */
} app_msg_t;

/* 初始化定时器模块（创建 LVGL timer） */
void app_timer_init(void);

/* 设置当前要执行的任务（页面切换时调用） */
void app_timer_set_task(app_task_cb task, void *user_data);

/* 获取消息对象指针（给外部发送消息用） */
app_msg_t *app_timer_get_msg(void);

/* 获取内部定时器句柄（可选，如果需要直接操作） */
lv_timer_t *app_timer_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_APP_TIMER_H */
