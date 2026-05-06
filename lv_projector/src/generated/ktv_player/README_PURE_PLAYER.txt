纯播放器改造说明
================

本次改造目标
------------
1. 播放器层只负责播放，不再负责队列、点歌库、拉取媒体信息。
2. 业务层自己从云端 JSON 提取：mp4_url / mp3_url / subtitle_path / has_audio。
3. 保留地址归一化：会自动把 https:// 归一化成 http://。

核心接口
--------
头文件：ktv_player_ui.h

typedef struct
{
    const char *mp4_url;
    const char *mp3_url;
    const char *subtitle_path;
    int has_audio;
} ktv_player_media_param_t;

int ktv_player_ui_play(const ktv_player_media_param_t *param);
int ktv_player_ui_replay_current(void);
int ktv_player_ui_pause(void);
int ktv_player_ui_resume(void);
int ktv_player_ui_stop(void);

参数规则
--------
1. mp4_url 必填，为空直接失败。
2. has_audio=1：表示 mp4 自带声音，只开视频播放器。
3. has_audio=0：表示 mp4 无声，mp3_url 必填，会启用视频+音频双播放器。
4. subtitle_path 可选：传本地字幕路径才启用字幕，不传则不开字幕。

已修正的行为
------------
1. 旧版本 has_audio=1 时会强制关闭字幕；现在只要传了 subtitle_path 就会启用字幕。
2. 字幕时钟不再只依赖 audio_player：
   - 双路模式：使用 audio_player 时间轴
   - 单路模式：使用 video_player 时间轴

业务分层建议
------------
1. 业务层请求：/song/order/current?deviceSn=xxx
2. 业务层解析 JSON，得到 ai_mv/mp3/本地字幕/has_audio
3. 组装 ktv_player_media_param_t
4. 调用 ktv_player_ui_play(&param)

说明
----
本次仅把 ktv_player_ui 改造成纯播放器内核。
旧的 ktv_song_lib / ktv_song_info_api / ktv_subtitle_fetch_api 文件仍保留在包内，
但播放器 UI 已不再依赖它们，方便你后续逐步继续清理。

参考例子：
ktv_player_media_param_t param;

ktv_player_ui_init();
karaoke_demo_open(parent);

/* 业务层自己请求云端 JSON，然后解析成下面这几个参数 */
memset(&param, 0, sizeof(param));
param.mp4_url = ai_mv_url;          /* 必填 */
param.mp3_url = mp3_url;            /* has_audio=0 时必填 */
param.subtitle_path = subtitle_path;/* 本地字幕路径，没有就 NULL */
param.has_audio = has_audio;        /* 1=mp4自带声音，0=需要独立mp3 */

ktv_player_ui_play(&param);