#ifndef __DB_LIST_PRO_WORKER_H__
#define __DB_LIST_PRO_WORKER_H__

#include "db_list_pro_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * worker 消息类型直接复用 db_list_pro_thread.h 中的定义：
 * - DBP_WORKER_MSG_PLAY
 * - DBP_WORKER_MSG_PAUSE
 * - DBP_WORKER_MSG_NEXT
 */

/*
 * 注册默认 worker 消息处理函数。
 *
 * 调用方式：
 * 1. 先调用 db_list_pro_thread_init()。
 * 2. 再调用本函数完成 worker 回调注册。
 */
int db_list_pro_worker_register(void);

/*
 * 默认 worker 消息处理函数。
 *
 * 说明：
 * - 本函数运行在内部 worker 线程。
 * - 这里只做后台业务处理，禁止直接操作 LVGL。
 * - 需要刷新 UI 时，请调用 db_list_pro_thread_send_to_ui()。
 */
void db_list_pro_worker_handle(int i1,
                               int i2,
                               int i3,
                               void *p1,
                               void *p2,
                               void *p3,
                               void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* __DB_LIST_PRO_WORKER_H__ */
