#ifndef __DB_LIST_PRO_LVGL_H__
#define __DB_LIST_PRO_LVGL_H__

#include "db_list_pro_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * UI 消息类型直接复用 db_list_pro_thread.h 中的定义：
 * - DBP_UI_MSG_RECV_REFRESH_CB
 */

/*
 * 刷新回调函数类型。
 *
 * 约定：
 * - p1 传回调函数指针时，类型按这个函数指针解释。
 * - p2/p3 以及 i2/i3 由你自己决定业务含义。
 */
typedef void (*dbp_ui_refresh_cb_t)(int i2,
                                    int i3,
                                    void *p2,
                                    void *p3);

/*
 * 由 LVGL 线程主动调用的唯一入口。
 *
 * 说明：
 * - 本函数内部会从 ui_queue 非阻塞取消息。
 * - 取到消息后，按 i1 进行分发处理。
 * - 你只需要在 LVGL 线程里周期调用这个函数。
 *
 * @max_count 本次最多处理多少条消息：
 *            - >0 : 最多处理 max_count 条。
 *            - <=0: 一直处理到 ui_queue 为空。
 *
 * 返回值：
 * - >=0 : 本次实际处理条数。
 * - -1  : 调用失败。
 */
int db_list_pro_lvgl_process(int max_count);

#ifdef __cplusplus
}
#endif

#endif /* __DB_LIST_PRO_LVGL_H__ */
