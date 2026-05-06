#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "tplayer.h"

static TPlayer *g_video_player = NULL;
static TPlayer *g_audio_player = NULL;
static int g_video_prepared = 0;
static int g_audio_prepared = 0;

static void video_callback(void *user_data, int msg, int ext1, void *para) {
    (void)user_data; (void)ext1; (void)para;
    if (msg == TPLAYER_NOTIFY_PREPARED) {
        printf("[VIDEO] prepared\n");
        g_video_prepared = 1;
    } else if (msg == TPLAYER_NOTIFY_PLAYBACK_COMPLETE) {
        printf("[VIDEO] playback complete\n");
    } else if (msg == TPLAYER_NOTIFY_MEDIA_ERROR) {
        printf("[VIDEO] media error\n");
    }
}

static void audio_callback(void *user_data, int msg, int ext1, void *para) {
    (void)user_data; (void)ext1; (void)para;
    if (msg == TPLAYER_NOTIFY_PREPARED) {
        printf("[AUDIO] prepared\n");
        g_audio_prepared = 1;
    } else if (msg == TPLAYER_NOTIFY_PLAYBACK_COMPLETE) {
        printf("[AUDIO] playback complete\n");
    } else if (msg == TPLAYER_NOTIFY_MEDIA_ERROR) {
        printf("[AUDIO] media error\n");
    }
}

static void wait_prepared(int *flag, int timeout_ms) {
    int waited = 0;
    while (!(*flag) && waited < timeout_ms) {
        usleep(100000);
        waited += 100;
    }
}

int main1(void) {
    // 创建音频播放器
    g_audio_player = TPlayerCreate(AUDIO_PLAYER);
    if (!g_audio_player) {
        printf("Create audio player failed\n");
        return -1;
    }
    // 创建视频播放器
    g_video_player = TPlayerCreate(CEDARX_PLAYER);
    if (!g_video_player) {
        printf("Create video player failed\n");
        TPlayerDestroy(g_audio_player);
        return -1;
    }

    // 设置回调
    TPlayerSetNotifyCallback(g_video_player, video_callback, NULL);
    TPlayerSetNotifyCallback(g_audio_player, audio_callback, NULL);

    // 数据源（必须加 file:// 前缀）
    const char *video_path = "/mnt/SDCARD/222.mp4";
    const char *audio_path = "/mnt/SDCARD/music01.mp3";
    char video_url[256], audio_url[256];
    snprintf(video_url, sizeof(video_url), "%s", video_path);
    snprintf(audio_url, sizeof(audio_url), "%s", audio_path);

    if (TPlayerSetDataSource(g_video_player, video_url, NULL) != 0) {
        printf("Set video data source failed\n");
        goto cleanup;
    }
    if (TPlayerSetDataSource(g_audio_player, audio_url, NULL) != 0) {
        printf("Set audio data source failed\n");
        goto cleanup;
    }

    // 异步准备
    TPlayerPrepareAsync(g_audio_player);
    TPlayerPrepareAsync(g_video_player);

    // 等待 prepared（先音频后视频）
    wait_prepared(&g_audio_prepared, 5000);
    wait_prepared(&g_video_prepared, 5000);
    if (!g_audio_prepared || !g_video_prepared) {
        printf("Prepare timeout\n");
        goto cleanup;
    }

    // 关键：视频静音，音频音量最大
    // TPlayerSetVolume(g_video_player, 0);   // 静音，释放音频设备
    // TPlayerSetVolume(g_audio_player, 40);  // 最大音量

    // 顺序：先启动音频，再启动视频
    printf("Start audio first...\n");
    TPlayerStart(g_audio_player);
    usleep(300000);  // 等待音频输出稳定

    printf("Then start video...\n");
    TPlayerStart(g_video_player);
    usleep(300000); 
ktv_player_ui_set_small_rect();
    printf("Both playing...\n");

    while (1) {
        sleep(1);
    }

cleanup:
    if (g_video_player) {
        TPlayerStop(g_video_player);
        TPlayerDestroy(g_video_player);
    }
    if (g_audio_player) {
        TPlayerStop(g_audio_player);
        TPlayerDestroy(g_audio_player);
    }
    return -1;
}