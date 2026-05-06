#include "ktv_player_ui.h"
#include "karaoke_demo.h"

/*
 * 这是新的业务接入示例：
 * 1. 云端接口 /song/order/current 由业务层自己请求
 * 2. 业务层自己从 JSON 提取 mp4/mp3/本地字幕/has_audio
 * 3. 播放器只负责播放，不再碰队列/点歌库/拉取 JSON
 */

void my_ktv_page_open(lv_obj_t *parent)
{
    ktv_player_media_param_t param;

    ktv_player_ui_init();
    karaoke_demo_open(parent);

    /*
     * 示例 1：mp4 自带声音，只开视频播放器。
     * 即使有声音，只要传了字幕本地路径，字幕也会照样启用。
     */
    param.mp4_url = "https://tuoge.djyos.com/demo/ai_mv.mp4";
    param.mp3_url = NULL;
    param.subtitle_path = "/mnt/SDCARD/subtitle/demo.zrc";
    param.has_audio = 1;
    (void)ktv_player_ui_play(&param);

    /*
     * 示例 2：mp4 无声音，必须额外传 mp3。
     * 业务层在调用前自己保证 subtitle_path 已经是本地路径。
     */
    param.mp4_url = "https://tuoge.djyos.com/demo/video_silent.mp4";
    param.mp3_url = "https://tuoge.djyos.com/demo/audio.mp3";
    param.subtitle_path = "/mnt/SDCARD/subtitle/demo2.zrc";
    param.has_audio = 0;
    (void)ktv_player_ui_play(&param);
}
