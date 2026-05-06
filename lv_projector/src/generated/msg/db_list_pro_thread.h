#ifndef __DB_LIST_PRO_THREAD_H__
#define __DB_LIST_PRO_THREAD_H__

#include "db_list_pro.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * db_list_pro_thread
 * -----------------------------------------------------------------------------
 * 设计目标：
 * 1. 对外隐藏底层队列句柄，业务层不需要知道 db_list_pro_t *。
 * 2. 模块内部固定维护两个队列：
 *    - worker_queue：后台工作线程消费。
 *    - ui_queue    ：LVGL 主线程轮询消费。
 * 3. 模块内部自动创建并维护一个 worker 线程。
 * 4. LVGL 不创建线程，只提供一个“由 LVGL 线程主动调用”的轮询函数。
 *
 * 典型用法：
 * 1. 启动时：
 *      db_list_pro_thread_init(32, 32);
 *      db_list_pro_thread_set_worker_cb(my_worker_cb, user_data);
 * 2. 任意线程发后台消息：
 *      db_list_pro_thread_send_to_worker(MSG_PLAY, 0, 0, p1, p2, p3);
 * 3. 任意线程发 UI 消息：
 *      db_list_pro_thread_send_to_ui(DBP_UI_MSG_RECV_REFRESH_CB, 0, 0, p1, p2, p3);
 * 4. 在 LVGL 安全上下文中周期调用：
 *      db_list_pro_thread_poll_ui(my_ui_cb, user_data, 8);
 * 5. 退出时：
 *      db_list_pro_thread_deinit();
 */

/* ------------------------------ 示例消息类型 ------------------------------- */
enum {
    DBP_WORKER_MSG_PLAY         = 1,
    DBP_WORKER_MSG_PAUSE        = 2,
    DBP_WORKER_MSG_NEXT         = 3,
    DBP_WORKER_MSG_REPLAY       = 4,   /* 重播 */
    DBP_WORKER_MSG_VIDEO_VOLUME = 5,   /* 视频音量调节，i2 传音量值 */
    DBP_WORKER_MSG_EXIT_VIDEO   = 6,   /* 退出视频 */

    DBP_WORKER_MSG_set_video_pos = 7,

    DBP_WORKER_MSG_add_song,
    DBP_WORKER_MSG_love_song,
    DBP_WORKER_MSG_AUTO_PLAY,
    


    DBP_WORKER_MSG_EXIT         = 9999,
};

enum {
    /* UI 常用消息示例：通知 LVGL 线程接收并执行刷新 cb 相关逻辑 */
    DBP_UI_MSG_RECV_REFRESH_CB = 1001,
};

/*
 * worker 消息处理回调。
 *
 * 说明：
 * - 本回调运行在模块内部 worker 线程中。
 * - 禁止在该回调里直接操作 LVGL 对象。
 * - 如需刷新界面，请转发到 ui_queue，再由 LVGL 线程处理。
 */
typedef void (*dbp_worker_msg_cb_t)(int i1,
                                    int i2,
                                    int i3,
                                    void *p1,
                                    void *p2,
                                    void *p3,
                                    void *user_data);

/*
 * UI 消息处理回调。
 *
 * 说明：
 * - 本回调由 db_list_pro_thread_poll_ui() 在调用线程中执行。
 * - 正常用法就是在 LVGL 线程里调用 poll_ui，因此这里是安全的 UI 处理点。
 */
typedef void (*dbp_ui_msg_cb_t)(int i1,
                                int i2,
                                int i3,
                                void *p1,
                                void *p2,
                                void *p3,
                                void *user_data);

/*
 * 初始化模块。
 *
 * 功能：
 * 1. 创建内部 worker_queue / ui_queue。
 * 2. 自动启动一个 worker 线程。
 *
 * @worker_queue_capacity worker 队列容量，必须 > 0。
 * @ui_queue_capacity     UI 队列容量，必须 > 0。
 *
 * 返回值：
 * - 0  : 成功。
 * - -1 : 失败。
 */
int db_list_pro_thread_init(int worker_queue_capacity, int ui_queue_capacity);

/*
 * 反初始化模块。
 *
 * 功能：
 * 1. 通知 worker 线程退出并 join。
 * 2. 销毁内部两个队列。
 * 3. 清空模块内部状态。
 */
void db_list_pro_thread_deinit(void);

/*
 * 设置 worker 消息处理回调。
 *
 * 说明：
 * - worker_queue 收到的普通消息，会在内部 worker 线程中回调到这里。
 */
int db_list_pro_thread_set_worker_cb(dbp_worker_msg_cb_t cb, void *user_data);

/*
 * 向内部 worker_queue 发送一条通用消息。
 *
 * 说明：
 * - 调用者不需要关心底层队列句柄。
 * - i1/i2/i3 + p1/p2/p3 全部原样透传。
 */
int db_list_pro_thread_send_to_worker(int i1,
                                      int i2,
                                      int i3,
                                      void *p1,
                                      void *p2,
                                      void *p3);

/*
 * 向内部 ui_queue 发送一条通用消息。
 *
 * 说明：
 * - 常用于后台线程通知 LVGL 主线程做界面刷新。
 */
int db_list_pro_thread_send_to_ui(int i1,
                                  int i2,
                                  int i3,
                                  void *p1,
                                  void *p2,
                                  void *p3);

/*
 * 在当前线程中轮询并分发 UI 消息。
 *
 * 典型用途：
 * - 由 LVGL 主线程周期性调用。
 * - 本函数不会阻塞。
 *
 * @cb        UI 消息回调，收到消息后在当前线程执行。
 * @user_data 透传给回调的用户参数。
 * @max_count 本次最多处理多少条消息：
 *            - >0 : 最多处理 max_count 条。
 *            - <=0: 一直处理到 ui_queue 为空。
 *
 * 返回值：
 * - >=0 : 实际处理的消息条数。
 * - -1  : 参数错误或模块未初始化。
 */
int db_list_pro_thread_poll_ui(dbp_ui_msg_cb_t cb, void *user_data, int max_count);

#ifdef __cplusplus
}
#endif

#endif /* __DB_LIST_PRO_THREAD_H__ */
