#ifndef __KTV_INPUT_MONITOR_H__
#define __KTV_INPUT_MONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "ktv_player_common.h"

/*
 * 输入事件
 */
typedef enum
{
    KTV_INPUT_EVENT_NONE = 0,
    KTV_INPUT_EVENT_POWER_KEY
} ktv_input_event_t;

/*
 * 输入回调
 */
typedef int (*ktv_input_monitor_callback_t)(void *user_data, ktv_input_event_t event);

/*
 * 输入监控上下文
 */
typedef struct
{
    int running;
    int done_flag;
    pthread_t thread_id;
    pthread_mutex_t lock;
    void *user_data;
    ktv_input_monitor_callback_t callback;
} ktv_input_monitor_t;

/*
 * 启动输入监控
 */
int ktv_input_monitor_start(ktv_input_monitor_t *monitor,
                            void *user_data,
                            ktv_input_monitor_callback_t callback);

/*
 * 停止输入监控
 */
int ktv_input_monitor_stop(ktv_input_monitor_t *monitor);

/*
 * 通知业务处理完成
 */
void ktv_input_monitor_notify_done(ktv_input_monitor_t *monitor);

#ifdef __cplusplus
}
#endif

#endif