#ifndef __KTV_PLAYER_WRAPPER_H__
#define __KTV_PLAYER_WRAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tplayer.h"
#include "ktv_player_common.h"

/*
 * 显示区域
 */
typedef struct
{
    int x;
    int y;
    int width;
    int height;
} ktv_player_rect_t;

/*
 * 单个播放器包装上下文
 */
typedef struct
{
    TPlayer *handle;
    int prepared_flag;
    int complete_flag;
    int error_flag;
    int seekable_flag;
    int loop_flag;
    int player_type;
    char url[KTV_PLAYER_MAX_URL_LEN];
} ktv_player_wrapper_t;

/*
 * 初始化单个播放器（默认视频播放器）
 */
int ktv_player_wrapper_init(ktv_player_wrapper_t *player);

/*
 * 初始化单个播放器（可指定类型）
 * CEDARX_PLAYER: 视频播放器
 * AUDIO_PLAYER : 音频播放器
 */
int ktv_player_wrapper_init_with_type(ktv_player_wrapper_t *player, int player_type);

/*
 * 释放单个播放器
 */
int ktv_player_wrapper_deinit(ktv_player_wrapper_t *player);

/*
 * 设置回调
 */
int ktv_player_wrapper_set_callback(ktv_player_wrapper_t *player,
                                    TPlayerNotifyCallback callback,
                                    void *user_data);

/*
 * 设置播放源
 */
int ktv_player_wrapper_set_source(ktv_player_wrapper_t *player, const char *url);

/*
 * 异步准备
 */
int ktv_player_wrapper_prepare_async(ktv_player_wrapper_t *player);

/*
 * 开始播放
 */
int ktv_player_wrapper_start(ktv_player_wrapper_t *player);

/*
 * 暂停播放
 */
int ktv_player_wrapper_pause(ktv_player_wrapper_t *player);

/*
 * 停止播放
 */
int ktv_player_wrapper_stop(ktv_player_wrapper_t *player);

/*
 * 重置播放器
 */
int ktv_player_wrapper_reset(ktv_player_wrapper_t *player);

/*
 * 设置循环播放
 */
int ktv_player_wrapper_set_loop(ktv_player_wrapper_t *player, int loop_flag);

/*
 * 设置音量
 */
int ktv_player_wrapper_set_volume(ktv_player_wrapper_t *player, int volume);

/*
 * 设置显示区域
 */
int ktv_player_wrapper_set_display_rect(ktv_player_wrapper_t *player,
                                        const ktv_player_rect_t *rect);

/*
 * 获取时长
 */
int ktv_player_wrapper_get_duration(ktv_player_wrapper_t *player, int *msec);

/*
 * 获取当前位置
 */
int ktv_player_wrapper_get_position(ktv_player_wrapper_t *player, int *msec);

/*
 * 判断是否正在播放
 */
int ktv_player_wrapper_is_playing(ktv_player_wrapper_t *player);

#ifdef __cplusplus
}
#endif

#endif
