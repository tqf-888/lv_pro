#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "db_list_pro_thread.h"

/* ---------------------------- 模块内部静态状态 ---------------------------- */
static db_list_pro_t *s_worker_queue = NULL;
static db_list_pro_t *s_ui_queue = NULL;
static pthread_t s_worker_thread;
static int s_module_inited = 0;
static int s_worker_thread_started = 0;

/* worker 回调注册点。 */
static dbp_worker_msg_cb_t s_worker_cb = NULL;
static void *s_worker_user_data = NULL;

/*
 * worker 线程主循环。
 *
 * 说明：
 * 1. 只消费内部 worker_queue。
 * 2. 收到 EXIT 消息后主动退出。
 * 3. 除 EXIT 外，其余消息统一交给 s_worker_cb 处理。
 * 4. 本线程中严禁直接操作 LVGL。
 */
static void *db_list_pro_thread_worker_main(void *arg)
{
    db_list_pro_msg_t msg;
    (void)arg;

    DBP_LOGI("db_list_pro_thread worker start\n");
    
    while (1) {
        memset(&msg, 0, sizeof(msg));

        if (db_list_pro_pop(s_worker_queue, &msg) != 0) {
            if (errno == ECANCELED) {
                DBP_LOGW("worker wake by shutdown\n");
                break;
            }
            continue;
        }

        if (msg.i1 == DBP_WORKER_MSG_EXIT) {
            DBP_LOGI("worker recv exit msg\n");
            break;
        }

        if (s_worker_cb != NULL) {
            s_worker_cb(msg.i1,
                        msg.i2,
                        msg.i3,
                        msg.p1,
                        msg.p2,
                        msg.p3,
                        s_worker_user_data);
        } else {
            DBP_LOGW("worker recv msg=%d, but worker_cb is null\n", msg.i1);
        }
    }

    DBP_LOGI("db_list_pro_thread worker exit\n");
    return NULL;
}

/* 初始化模块并自动启动 worker 线程。 */
int db_list_pro_thread_init(int worker_queue_capacity, int ui_queue_capacity)
{
    int ret = 0;

    if (worker_queue_capacity <= 0 || ui_queue_capacity <= 0) {
        errno = EINVAL;
        DBP_LOGE("thread_init failed: invalid capacity\n");
        return -1;
    }

    if (s_module_inited) {
        DBP_LOGW("thread_init skipped: already inited\n");
        return 0;
    }

    /*
     * worker_queue:
     * - 供后台线程阻塞消费。
     * - 队列满时是否阻塞，当前直接开启阻塞，避免误丢消息。
     */
    s_worker_queue = db_list_pro_create("dbp_worker_queue",
                                        worker_queue_capacity,
                                        1,
                                        1);
    if (s_worker_queue == NULL) {
        DBP_LOGE("thread_init failed: create worker queue error\n");
        goto error;
    }

    /*
     * ui_queue:
     * - 供任意线程投递给 LVGL。
     * - LVGL 主线程采用 poll_ui 非阻塞轮询，不阻塞 UI。
     */
    s_ui_queue = db_list_pro_create("dbp_ui_queue",
                                    ui_queue_capacity,
                                    1,
                                    0);
    if (s_ui_queue == NULL) {
        DBP_LOGE("thread_init failed: create ui queue error\n");
        goto error;
    }

    pthread_attr_t attr;
    size_t stack_size = 1024 * 1024; /* 1MB */

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stack_size);
    ret = pthread_create(&s_worker_thread, &attr, db_list_pro_thread_worker_main, NULL);
    pthread_attr_destroy(&attr);

    if (ret != 0) {
        errno = ret;
        DBP_LOGE("thread_init failed: pthread_create error=%d\n", ret);
        goto error;
    }

    s_worker_thread_started = 1;
    s_module_inited = 1;
    DBP_LOGI("db_list_pro_thread init ok\n");
    return 0;

error:
    if (s_ui_queue != NULL) {
        db_list_pro_destroy(s_ui_queue);
        s_ui_queue = NULL;
    }
    if (s_worker_queue != NULL) {
        db_list_pro_destroy(s_worker_queue);
        s_worker_queue = NULL;
    }
    s_module_inited = 0;
    s_worker_thread_started = 0;
    s_worker_cb = NULL;
    s_worker_user_data = NULL;
    return -1;
}

/*
 * 反初始化模块。
 *
 * 退出顺序：
 * 1. 给 worker_queue 投递 EXIT 消息。
 * 2. 如果投递失败，则 shutdown worker_queue 强制唤醒阻塞 pop。
 * 3. join worker 线程。
 * 4. 销毁两个队列。
 */
void db_list_pro_thread_deinit(void)
{
    if (!s_module_inited) {
        return;
    }

    if (s_worker_thread_started) {
        if (db_list_pro_push(s_worker_queue,
                             DBP_WORKER_MSG_EXIT,
                             0,
                             0,
                             NULL,
                             NULL,
                             NULL) != 0) {
            DBP_LOGW("thread_deinit: push exit failed, try shutdown worker queue\n");
            db_list_pro_shutdown(s_worker_queue);
        }

        pthread_join(s_worker_thread, NULL);
        s_worker_thread_started = 0;
    }

    if (s_ui_queue != NULL) {
        db_list_pro_destroy(s_ui_queue);
        s_ui_queue = NULL;
    }

    if (s_worker_queue != NULL) {
        db_list_pro_destroy(s_worker_queue);
        s_worker_queue = NULL;
    }

    s_worker_cb = NULL;
    s_worker_user_data = NULL;
    s_module_inited = 0;

    DBP_LOGI("db_list_pro_thread deinit ok\n");
}

/* 注册 worker 回调。 */
int db_list_pro_thread_set_worker_cb(dbp_worker_msg_cb_t cb, void *user_data)
{
    if (!s_module_inited) {
        errno = EPERM;
        DBP_LOGE("set_worker_cb failed: module not init\n");
        return -1;
    }

    s_worker_cb = cb;
    s_worker_user_data = user_data;
    return 0;
}

/* 向内部 worker_queue 发送一条通用消息。 */
int db_list_pro_thread_send_to_worker(int i1,
                                      int i2,
                                      int i3,
                                      void *p1,
                                      void *p2,
                                      void *p3)
{
    if (!s_module_inited || s_worker_queue == NULL) {
        errno = EPERM;
        DBP_LOGE("send_to_worker failed: module not init\n");
        return -1;
    }

    return db_list_pro_push(s_worker_queue, i1, i2, i3, p1, p2, p3);
}

/* 向内部 ui_queue 发送一条通用消息。 */
int db_list_pro_thread_send_to_ui(int i1,
                                  int i2,
                                  int i3,
                                  void *p1,
                                  void *p2,
                                  void *p3)
{
    if (!s_module_inited || s_ui_queue == NULL) {
        errno = EPERM;
        DBP_LOGE("send_to_ui failed: module not init\n");
        return -1;
    }

    return db_list_pro_push(s_ui_queue, i1, i2, i3, p1, p2, p3);
}

/*
 * 由 LVGL 线程主动轮询 UI 消息。
 *
 * 注意：
 * - 本函数不阻塞。
 * - 如果 cb 为 NULL，则只做“丢弃式取出”，通常不建议这样用。
 */
int db_list_pro_thread_poll_ui(dbp_ui_msg_cb_t cb, void *user_data, int max_count)
{
    int handled = 0;
    db_list_pro_msg_t msg;

    if (!s_module_inited || s_ui_queue == NULL) {
        errno = EPERM;
        DBP_LOGE("poll_ui failed: module not init\n");
        return -1;
    }

    while (1) {
        memset(&msg, 0, sizeof(msg));

        if (db_list_pro_try_pop(s_ui_queue, &msg) != 0) {
            if (errno == EAGAIN) {
                break;
            }
            return (handled > 0) ? handled : -1;
        }

        handled++;

        if (cb != NULL) {
            cb(msg.i1,
               msg.i2,
               msg.i3,
               msg.p1,
               msg.p2,
               msg.p3,
               user_data);
        }

        if (max_count > 0 && handled >= max_count) {
            break;
        }
    }

    return handled;
}
