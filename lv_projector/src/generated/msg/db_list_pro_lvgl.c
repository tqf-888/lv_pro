#include "db_list_pro_lvgl.h"

/*
 * UI 队列内部消息处理函数。
 *
 * 说明：
 * - 这个函数只在 db_list_pro_lvgl_process() 内部使用。
 * - 外部调用者不需要关心，也不需要自己写 UI 回调。
 */
static void db_list_pro_lvgl_dispatch(int i1,
                                      int i2,
                                      int i3,
                                      void *p1,
                                      void *p2,
                                      void *p3,
                                      void *user_data)
{
    dbp_ui_refresh_cb_t refresh_cb;
    (void)user_data;

    switch (i1) {
    case DBP_UI_MSG_RECV_REFRESH_CB:
        /*
         * 约定：
         * - p1 : 刷新回调函数指针。
         * - i2/i3、p2/p3 : 原样透传给刷新回调。
         */
        refresh_cb = (dbp_ui_refresh_cb_t)p1;
        if (refresh_cb != NULL) {
            refresh_cb(i2, i3, p2, p3);
        } else {
            DBP_LOGW("lvgl process: refresh cb is null\n");
        }
        break;

    default:
        DBP_LOGW("lvgl process: unknown ui msg=%d\n", i1);
        break;
    }
}

/*
 * 由 LVGL 线程调用的唯一入口。
 */
int db_list_pro_lvgl_process(int max_count)
{
    return db_list_pro_thread_poll_ui(db_list_pro_lvgl_dispatch, NULL, max_count);
}
