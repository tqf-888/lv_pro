#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ktv_player_wrapper.h"

/*
 * 内部有效性检查
 */
static int ktv_player_wrapper_is_valid(ktv_player_wrapper_t *player)
{
    if (player == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (player->handle == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 初始化单个播放器（默认视频播放器）
 */
int ktv_player_wrapper_init(ktv_player_wrapper_t *player)
{
    return ktv_player_wrapper_init_with_type(player, CEDARX_PLAYER);
}

/*
 * 初始化单个播放器（可指定类型）
 */
int ktv_player_wrapper_init_with_type(ktv_player_wrapper_t *player, int player_type)
{
    if (player == NULL)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_init_with_type: player is NULL");
        return KTV_PLAYER_RET_FAIL;
    }

    memset(player, 0, sizeof(ktv_player_wrapper_t));

    player->handle = TPlayerCreate(player_type);
    if (player->handle == NULL)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_init_with_type: TPlayerCreate failed, type=%d", player_type);
        return KTV_PLAYER_RET_FAIL;
    }

    player->player_type = player_type;
    player->seekable_flag = 1;

    KTV_PLAYER_LOGI("ktv_player_wrapper_init_with_type: success, type=%d", player_type);
    return KTV_PLAYER_RET_OK;
}

/*
 * 释放单个播放器
 */
int ktv_player_wrapper_deinit(ktv_player_wrapper_t *player)
{
    if (player == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (player->handle != NULL)
    {
        TPlayerDestroy(player->handle);
        player->handle = NULL;
    }

    memset(player->url, 0, sizeof(player->url));
    player->prepared_flag = 0;
    player->complete_flag = 0;
    player->error_flag = 0;
    player->seekable_flag = 0;
    player->loop_flag = 0;
    player->player_type = 0;

    KTV_PLAYER_LOGI("ktv_player_wrapper_deinit: success");
    return KTV_PLAYER_RET_OK;
}

/*
 * 设置回调
 */
int ktv_player_wrapper_set_callback(ktv_player_wrapper_t *player,
                                    TPlayerNotifyCallback callback,
                                    void *user_data)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (callback == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerSetNotifyCallback(player->handle, callback, user_data) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_set_callback: failed");
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 设置播放源
 */
int ktv_player_wrapper_set_source(ktv_player_wrapper_t *player, const char *url)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (url == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    /*
     * TPlayerSetDataSource() only accepts IDLE/INITIALIZED. A previous
     * prepare/start failure can leave the native player in another state even
     * when the UI state has already gone back to idle.
     */
    if (TPlayerReset(player->handle) != 0)
    {
        KTV_PLAYER_LOGW("ktv_player_wrapper_set_source: reset before set source failed");
    }

    memset(player->url, 0, sizeof(player->url));
    snprintf(player->url, sizeof(player->url), "%s", url);

    player->prepared_flag = 0;
    player->complete_flag = 0;
    player->error_flag = 0;
    player->seekable_flag = 1;

    if (TPlayerSetDataSource(player->handle, player->url, NULL) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_set_source: failed");
        (void)TPlayerReset(player->handle);
        return KTV_PLAYER_RET_FAIL;
    }

    KTV_PLAYER_LOGI("ktv_player_wrapper_set_source: success, type=%d, url=%s",
                    player->player_type,
                    player->url);
    return KTV_PLAYER_RET_OK;
}

/*
 * 异步准备
 */
int ktv_player_wrapper_prepare_async(ktv_player_wrapper_t *player)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    player->prepared_flag = 0;
    player->error_flag = 0;

    if (TPlayerPrepareAsync(player->handle) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_prepare_async: failed");
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 开始播放
 */
int ktv_player_wrapper_start(ktv_player_wrapper_t *player)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerStart(player->handle) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_start: failed");
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 暂停播放
 */
int ktv_player_wrapper_pause(ktv_player_wrapper_t *player)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerPause(player->handle) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_pause: failed");
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 停止播放
 */
int ktv_player_wrapper_stop(ktv_player_wrapper_t *player)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerStop(player->handle) != 0)
    {
        KTV_PLAYER_LOGW("ktv_player_wrapper_stop: failed");
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 重置播放器
 */
int ktv_player_wrapper_reset(ktv_player_wrapper_t *player)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerReset(player->handle) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_reset: failed");
        return KTV_PLAYER_RET_FAIL;
    }

    player->prepared_flag = 0;
    player->complete_flag = 0;
    player->error_flag = 0;
    player->seekable_flag = 1;

    return KTV_PLAYER_RET_OK;
}

/*
 * 设置循环播放
 */
int ktv_player_wrapper_set_loop(ktv_player_wrapper_t *player, int loop_flag)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    player->loop_flag = loop_flag;

    if (TPlayerSetLooping(player->handle, loop_flag) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_set_loop: failed");
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 设置音量
 */
int ktv_player_wrapper_set_volume(ktv_player_wrapper_t *player, int volume)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerSetVolume(player->handle, volume) != 0)
    {
        KTV_PLAYER_LOGE("ktv_player_wrapper_set_volume: failed, type=%d, volume=%d",
                        player->player_type,
                        volume);
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 设置显示区域
 */
int ktv_player_wrapper_set_display_rect(ktv_player_wrapper_t *player,
                                        const ktv_player_rect_t *rect)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (rect == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    TPlayerSetDisplayRect(player->handle,
                              rect->x,
                              rect->y,
                              rect->width,
                              rect->height);

    return KTV_PLAYER_RET_OK;
}

/*
 * 获取时长
 */
int ktv_player_wrapper_get_duration(ktv_player_wrapper_t *player, int *msec)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (msec == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerGetDuration(player->handle, msec) != 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 获取当前位置
 */
int ktv_player_wrapper_get_position(ktv_player_wrapper_t *player, int *msec)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (msec == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerGetCurrentPosition(player->handle, msec) != 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    return KTV_PLAYER_RET_OK;
}

/*
 * 判断是否正在播放
 */
int ktv_player_wrapper_is_playing(ktv_player_wrapper_t *player)
{
    if (ktv_player_wrapper_is_valid(player) == KTV_PLAYER_RET_FAIL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    return TPlayerIsPlaying(player->handle) ? KTV_PLAYER_RET_OK : KTV_PLAYER_RET_FAIL;
}
