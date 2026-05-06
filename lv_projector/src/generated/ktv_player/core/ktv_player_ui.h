#ifndef __KTV_PLAYER_UI_H__
#define __KTV_PLAYER_UI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ktv_player_wrapper.h"
#include "ktv_player_common.h"

#define KTV_PLAYER_UI_STATE_IDLE              (0)
#define KTV_PLAYER_UI_STATE_PLAYING           (1)
#define KTV_PLAYER_UI_STATE_PAUSED            (2)

/*
 * 纯播放器入参：
 * 1. mp4_url 必填，为空直接失败
 * 2. has_audio=1 表示 mp4 自带声音，只开视频播放器
 * 3. has_audio=0 表示 mp4 无声，必须再传 mp3_url
 * 4. subtitle_path 可选，传本地字幕路径才启用字幕，不传则不开字幕
 */
typedef struct
{
    const char *mp4_url;
    const char *mp3_url;
    const char *subtitle_path;
    int has_audio;
} ktv_player_media_param_t;

/*
 * 初始化播放器控制模块
 * 默认使用小屏显示区域
 */
int ktv_player_ui_init(void);

/*
 * 释放播放器控制模块
 */
int ktv_player_ui_deinit(void);

/*
 * 纯播放接口
 * 1. mp4_url 为空 -> 失败
 * 2. has_audio 只能是 0/1
 * 3. has_audio=0 时 mp3_url 必填
 * 4. subtitle_path 非空则启用字幕
 */
int ktv_player_ui_play(const ktv_player_media_param_t *param);

/*
 * 重播当前资源
 * 使用当前保存的 mp4/mp3/字幕/has_audio 原样重播
 */
int ktv_player_ui_replay_current(void);

/*
 * 获取当前正在播放的字幕本地路径
 * 当前项如果没有字幕，返回空字符串
 */
const char *ktv_player_ui_get_current_subtitle_path(void);

/*
 * 当前播放项是否带字幕
 * 1: 有字幕
 * 0: 无字幕
 */
int ktv_player_ui_current_has_subtitle(void);

/*
 * 暂停播放
 */
int ktv_player_ui_pause(void);

/*
 * 恢复播放
 */
int ktv_player_ui_resume(void);

/*
 * 停止播放
 */
int ktv_player_ui_stop(void);

/*
 * 设置小屏显示区域
 */
int ktv_player_ui_set_small_rect(void);

/*
 * 设置全屏显示区域
 */
int ktv_player_ui_set_full_rect(void);

/*
 * 设置自定义显示区域
 */
int ktv_player_ui_set_rect(int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
