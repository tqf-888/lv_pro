#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/epoll.h>
#include "ktv_input_monitor.h"
#include "ktv.h"
/*
 * 输入层日志
 */
#define KTV_INPUT_LOGE(fmt, args...)      dbg_print("[KTV_INPUT][E] " fmt, ##args)
#define KTV_INPUT_LOGW(fmt, args...)      dbg_print("[KTV_INPUT][W] " fmt, ##args)
#define KTV_INPUT_LOGI(fmt, args...)      dbg_print("[KTV_INPUT][I] " fmt, ##args)
#define KTV_INPUT_LOGD(fmt, args...)      dbg_print("[KTV_INPUT][D] " fmt, ##args)

/*
 * 打开 input 设备
 */
static int ktv_input_monitor_open_devices(int fd_list[], int max_count)
{
    int i;
    int count;
    char dev_name[64];

    count = 0;

    for (i = 0; i < max_count; i++)
    {
        memset(dev_name, 0, sizeof(dev_name));
        snprintf(dev_name, sizeof(dev_name), "/dev/input/event%d", i);

        fd_list[count] = open(dev_name, O_RDONLY);
        if (fd_list[count] >= 0)
        {
            count++;
        }
    }

    return count;
}

/*
 * 关闭 input 设备
 */
static void ktv_input_monitor_close_devices(int fd_list[], int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (fd_list[i] >= 0)
        {
            close(fd_list[i]);
            fd_list[i] = -1;
        }
    }
}

/*
 * 监控线程
 */
static void *ktv_input_monitor_thread(void *arg)
{
    ktv_input_monitor_t *monitor;
    int epoll_fd;
    int fd_list[KTV_INPUT_MONITOR_MAX_COUNT];
    int fd_count;
    int i;
    int nevents;
    struct epoll_event ev;
    struct epoll_event events[KTV_INPUT_MONITOR_MAX_COUNT];
    struct input_event key_event;

    monitor = (ktv_input_monitor_t *)arg;
    if (monitor == NULL)
    {
        return NULL;
    }

    memset(fd_list, -1, sizeof(fd_list));

    epoll_fd = epoll_create(32);
    if (epoll_fd < 0)
    {
        KTV_INPUT_LOGE("ktv_input_monitor_thread: epoll_create failed");
        return NULL;
    }

    fd_count = ktv_input_monitor_open_devices(fd_list, KTV_INPUT_MONITOR_MAX_COUNT);

    for (i = 0; i < fd_count; i++)
    {
        memset(&ev, 0, sizeof(ev));
        ev.data.fd = fd_list[i];
        ev.events = EPOLLIN | EPOLLWAKEUP;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_list[i], &ev);
    }

    pthread_mutex_lock(&monitor->lock);
    monitor->done_flag = 1;
    pthread_mutex_unlock(&monitor->lock);

    while (1)
    {
        pthread_mutex_lock(&monitor->lock);
        if (monitor->running == 0)
        {
            pthread_mutex_unlock(&monitor->lock);
            break;
        }
        pthread_mutex_unlock(&monitor->lock);

        nevents = epoll_wait(epoll_fd,
                             events,
                             KTV_INPUT_MONITOR_MAX_COUNT,
                             KTV_INPUT_MONITOR_WAIT_TIMEOUT_MS);

        if (nevents < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }

        for (i = 0; i < nevents; i++)
        {
            if (read(events[i].data.fd, &key_event, sizeof(struct input_event)) != sizeof(struct input_event))
            {
                continue;
            }

            if (key_event.type == EV_KEY &&
                key_event.code == KEY_POWER &&
                key_event.value == 1)
            {
                pthread_mutex_lock(&monitor->lock);
                if (monitor->done_flag == 0)
                {
                    pthread_mutex_unlock(&monitor->lock);
                    continue;
                }
                monitor->done_flag = 0;
                pthread_mutex_unlock(&monitor->lock);

                if (monitor->callback != NULL)
                {
                    monitor->callback(monitor->user_data, KTV_INPUT_EVENT_POWER_KEY);
                }
            }
        }

        usleep(500);
    }

    ktv_input_monitor_close_devices(fd_list, fd_count);
    close(epoll_fd);

    return NULL;
}

/*
 * 启动输入监控
 */
int ktv_input_monitor_start(ktv_input_monitor_t *monitor,
                            void *user_data,
                            ktv_input_monitor_callback_t callback)
{
    if (monitor == NULL || callback == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    memset(monitor, 0, sizeof(ktv_input_monitor_t));

    if (pthread_mutex_init(&monitor->lock, NULL) != 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    monitor->running = 1;
    monitor->done_flag = 1;
    monitor->user_data = user_data;
    monitor->callback = callback;

    if (pthread_create(&monitor->thread_id, NULL, ktv_input_monitor_thread, monitor) != 0)
    {
        pthread_mutex_destroy(&monitor->lock);
        return KTV_PLAYER_RET_FAIL;
    }

    KTV_INPUT_LOGI("ktv_input_monitor_start: success");
    return KTV_PLAYER_RET_OK;
}

/*
 * 停止输入监控
 */
int ktv_input_monitor_stop(ktv_input_monitor_t *monitor)
{
    if (monitor == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    pthread_mutex_lock(&monitor->lock);
    monitor->running = 0;
    pthread_mutex_unlock(&monitor->lock);

    pthread_join(monitor->thread_id, NULL);
    pthread_mutex_destroy(&monitor->lock);

    KTV_INPUT_LOGI("ktv_input_monitor_stop: success");
    return KTV_PLAYER_RET_OK;
}

/*
 * 通知业务处理完成
 */
void ktv_input_monitor_notify_done(ktv_input_monitor_t *monitor)
{
    if (monitor == NULL)
    {
        return;
    }

    pthread_mutex_lock(&monitor->lock);
    monitor->done_flag = 1;
    pthread_mutex_unlock(&monitor->lock);
}