#include "lvgl_page_navigation.h"
#include "lvgl_app_timer.h"
#include "gui_guider.h"
#include "ktv.h"

void page1_fun(void)
{
    char qq[100];
    // ktv_get_image_path_by_index("/tmp/666", 0, "png", qq, sizeof(qq));
    // lv_img_set_src(guider_ui.screen_8_img_1, qq);
    dbg_print("ktv_get_image_path_by_index %d", page_nav_get_current());
}

/* 内部：页面任务表 */
static page_task_cb g_page_tasks[PAGE_ID_MAX] = { NULL };

/* 内部：当前页面 */
static page_id_e g_current_page = PAGE_ID_MAX;

/* 内部：页面任务适配层，适配给 app_timer 的 app_task_cb */
static void page_task_adapter(void *param)
{
    (void)param;  /* 暂不使用，消息通过 app_timer_get_msg 获取 */

    page_task_cb task = g_page_tasks[g_current_page];
    if (task) {
        task();
    }
}

/* 切换页面时，把适配层注册给定时器 */
void page_nav_set_current(page_id_e id)
{
    if (id >= PAGE_ID_MAX) {
        return;
    }

    g_current_page = id;

    /* 核心：页面变了，告诉定时器：现在执行这个任务 */
    app_timer_set_task(page_task_adapter, NULL);
}

page_id_e page_nav_get_current(void)
{
    return g_current_page;
}

void page_nav_init(void)
{
    /* 1. 初始化定时器模块 */
    app_timer_init();

    page_nav_register_task(PAGE_ID_HOME, page1_fun);
    page_nav_register_task(PAGE_ID_MUSIC, page1_fun);
    page_nav_register_task(PAGE_ID_SETTINGS, page1_fun);
    page_nav_register_task(PAGE_ID_SETTINGS1, page1_fun);
    page_nav_register_task(PAGE_ID_SETTINGS11, page1_fun);
    page_nav_register_task(PAGE_ID_SETTINGS111, page1_fun);

    /* 4. 设置默认页面 */
    page_nav_set_current(PAGE_ID_MAX);
}

void page_nav_register_task(page_id_e id, page_task_cb task)
{
    if (id < PAGE_ID_MAX) {
        g_page_tasks[id] = task;
    }
}
