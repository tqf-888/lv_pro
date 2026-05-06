#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include "ktv_player_wrapper.h"

/*
 * 单路 demo 上下文
 */
typedef struct
{
    ktv_player_wrapper_t player;
    sem_t prepared_sem;
    int prepared_ok;
    int error_flag;
} ktv_app_player_demo_t;

/*
 * 等待 prepared
 */
static int ktv_app_wait_prepared(sem_t *sem, int timeout_ms)
{
    struct timespec ts;

    if (sem == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;

    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }

    if (sem_timedwait(sem, &ts) != 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 播放器回调
 */
static int ktv_app_player_callback(void *user_data, int msg, int ext1, void *para)
{
    ktv_app_player_demo_t *app;

    (void)ext1;
    (void)para;

    app = (ktv_app_player_demo_t *)user_data;
    if (app == NULL)
    {
        return 0;
    }

    switch (msg)
    {
        case TPLAYER_NOTIFY_PREPARED:
        {
            app->prepared_ok = 1;
            sem_post(&app->prepared_sem);
            break;
        }

        case TPLAYER_NOTIFY_MEDIA_ERROR:
        {
            app->error_flag = 1;
            sem_post(&app->prepared_sem);
            break;
        }

        case TPLAYER_NOTIFY_PLAYBACK_COMPLETE:
        {
            dbg_print("[KTV_DEMO] playback complete");
            break;
        }

        case TPLAYER_NOTIFY_NOT_SEEKABLE:
        {
            app->player.seekable_flag = 0;
            break;
        }

        default:
        {
            break;
        }
    }

    return 0;
}

/*
 * 单路播放
 */
int ktv_app_player_demo_run(void)
{
    ktv_app_player_demo_t app;
    ktv_player_rect_t rect;
    const char *url;

    memset(&app, 0, sizeof(ktv_app_player_demo_t));

    url = "/mnt/SDCARD/111.mp4";

    if (sem_init(&app.prepared_sem, 0, 0) != 0)
    {
        dbg_print("[KTV_DEMO] sem_init failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_init(&app.player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_DEMO] ktv_player_wrapper_init failed");
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_set_callback(&app.player,
                                        ktv_app_player_callback,
                                        &app) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_DEMO] ktv_player_wrapper_set_callback failed");
        ktv_player_wrapper_deinit(&app.player);
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    rect.x = 0;
    rect.y = 0;
    rect.width = KTV_PLAYER_SCREEN_WIDTH;
    rect.height = KTV_PLAYER_SCREEN_HEIGHT;

    if (ktv_player_wrapper_set_display_rect(&app.player, &rect) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_DEMO] ktv_player_wrapper_set_display_rect failed");
        ktv_player_wrapper_deinit(&app.player);
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_set_source(&app.player, url) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_DEMO] ktv_player_wrapper_set_source failed");
        ktv_player_wrapper_deinit(&app.player);
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_prepare_async(&app.player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_DEMO] ktv_player_wrapper_prepare_async failed");
        ktv_player_wrapper_deinit(&app.player);
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_app_wait_prepared(&app.prepared_sem, KTV_PLAYER_PREPARE_TIMEOUT_MS) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_DEMO] wait prepared timeout");
        ktv_player_wrapper_deinit(&app.player);
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (app.error_flag != 0 || app.prepared_ok == 0)
    {
        dbg_print("[KTV_DEMO] prepared failed");
        ktv_player_wrapper_deinit(&app.player);
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_start(&app.player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_DEMO] ktv_player_wrapper_start failed");
        ktv_player_wrapper_deinit(&app.player);
        sem_destroy(&app.prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    dbg_print("[KTV_DEMO] play start success");

    while (1)
    {
        sleep(1);
    }

    ktv_player_wrapper_stop(&app.player);
    ktv_player_wrapper_deinit(&app.player);
    sem_destroy(&app.prepared_sem);

    return KTV_PLAYER_RET_OK;
}