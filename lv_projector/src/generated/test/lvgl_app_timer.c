#include "lvgl_app_timer.h"
#include <string.h>

typedef struct {
    app_task_cb current_task;   /* 当前任务函数 */
    void       *user_data;      /* 任务携带的用户数据 */
    app_msg_t   msg;            /* 消息载体 */
} app_timer_ctx_t;

static app_timer_ctx_t g_ctx;
static lv_timer_t *g_timer = NULL;

/* 定时器回调：LVGL 的“灵魂” */
static void app_timer_cb(lv_timer_t *timer)
{
    /* 1. 如果有消息，可以统一处理，或者交给 task 处理 */
    if (g_ctx.msg.has_msg) {
        /* 这里可以根据需要做统一预处理 */
        g_ctx.msg.has_msg = false;  /* 清除标志 */
    }

    /* 2. 执行当前任务 */
    if (g_ctx.current_task) {
        g_ctx.current_task(g_ctx.user_data);
    }
}

void app_timer_init(void)
{
    memset(&g_ctx, 0, sizeof(g_ctx));
    g_timer = lv_timer_create(app_timer_cb, APP_TIMER_PERIOD_MS, NULL);
}

void app_timer_set_task(app_task_cb task, void *user_data)
{
    g_ctx.current_task = task;
    g_ctx.user_data    = user_data;
}

app_msg_t *app_timer_get_msg(void)
{
    return &g_ctx.msg;
}

lv_timer_t *app_timer_get_handle(void)
{
    return g_timer;
}

//demo
void http_download_done(const char *path)
{
    app_msg_t *msg = app_timer_get_msg();
    if (msg) {
        snprintf(msg->body, sizeof(msg->body), "%s", path);
        msg->has_msg = true;
    }
}