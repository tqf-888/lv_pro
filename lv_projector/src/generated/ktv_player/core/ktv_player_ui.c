#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include "ktv_player_ui.h"
#include "karaoke_demo.h"

/*
 * 显示区域宏
 * 默认先使用小屏
 */
#define KTV_PLAYER_SMALL_RECT_X               (330)
#define KTV_PLAYER_SMALL_RECT_Y               (330)
#define KTV_PLAYER_SMALL_RECT_W               (320)
#define KTV_PLAYER_SMALL_RECT_H               (600)

/*
 * 双路播放器控制上下文
 * video_player: 负责画面输出
 * audio_player: 仅在“无声 mp4 + 独立 mp3”模式下启用
 */
typedef struct
{
    ktv_player_wrapper_t video_player;
    ktv_player_wrapper_t audio_player;
    sem_t video_prepared_sem;
    sem_t audio_prepared_sem;
    int video_prepared_ok;
    int audio_prepared_ok;
    int video_error_flag;
    int audio_error_flag;
    int audio_enabled;
    int init_flag;
    int state;
    lv_timer_t *subtitle_sync_timer;
    int subtitle_sync_last_pos;
    char current_mv_url[KTV_PLAYER_MAX_URL_LEN];
    char current_audio_url[KTV_PLAYER_MAX_URL_LEN];
    char current_subtitle_path[KTV_PLAYER_MAX_URL_LEN];
    int current_has_audio;
    ktv_player_rect_t current_rect;
    int rect_valid;
} ktv_player_ui_ctx_t;

static ktv_player_ui_ctx_t g_ktv_player_ui_ctx = {0};

static int ktv_player_ui_str_is_empty(const char *s)
{
    return (s == NULL || s[0] == '\0') ? 1 : 0;
}

/*
 * 地址归一化非常重要：
 * 1. 处理 json 转义斜杠
 * 2. 统一把 https 转成 http，兼容现有环境
 */
static void ktv_player_ui_normalize_play_url_to_http(char *dst, size_t dst_size, const char *src)
{
    char tmp[KTV_PLAYER_MAX_URL_LEN];
    size_t si;
    size_t di;

    if (dst == NULL || dst_size == 0)
    {
        return;
    }

    dst[0] = '\0';
    if (src == NULL || src[0] == '\0')
    {
        return;
    }

    memset(tmp, 0, sizeof(tmp));

    si = 0;
    di = 0;
    while (src[si] != '\0' && di + 1 < sizeof(tmp))
    {
        if (src[si] == '\\' && src[si + 1] != '\0')
        {
            si++;
            switch (src[si])
            {
                case '/':
                    tmp[di++] = '/';
                    si++;
                    continue;
                case '\\':
                    tmp[di++] = '\\';
                    si++;
                    continue;
                case '"':
                    tmp[di++] = '"';
                    si++;
                    continue;
                case 'n':
                    tmp[di++] = '\n';
                    si++;
                    continue;
                case 'r':
                    tmp[di++] = '\r';
                    si++;
                    continue;
                case 't':
                    tmp[di++] = '\t';
                    si++;
                    continue;
                default:
                    tmp[di++] = src[si++];
                    continue;
            }
        }

        tmp[di++] = src[si++];
    }

    tmp[di] = '\0';

    if (strncmp(tmp, "https://", 8) == 0)
    {
        snprintf(dst, dst_size, "http://%s", tmp + 8);
        return;
    }

    snprintf(dst, dst_size, "%s", tmp);
}

static int ktv_player_ui_get_subtitle_clock_position(ktv_player_ui_ctx_t *ctx, int *pos)
{
    if (ctx == NULL || pos == NULL)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (ctx->audio_enabled != 0)
    {
        return ktv_player_wrapper_get_position(&ctx->audio_player, pos);
    }

    return ktv_player_wrapper_get_position(&ctx->video_player, pos);
}

static void ktv_player_ui_subtitle_sync_timer_cb(lv_timer_t *timer)
{
    ktv_player_ui_ctx_t *ctx;
    int pos = 0;

    if (timer == NULL)
    {
        return;
    }

    ctx = (ktv_player_ui_ctx_t *)timer->user_data;
    if (ctx == NULL)
    {
        return;
    }

    if (ctx->state != KTV_PLAYER_UI_STATE_PLAYING ||
        ctx->current_subtitle_path[0] == '\0' ||
        !karaoke_demo_is_render_enabled())
    {
        return;
    }

    if (ktv_player_ui_get_subtitle_clock_position(ctx, &pos) != KTV_PLAYER_RET_OK)
    {
        return;
    }

    if (pos < 0)
    {
        pos = 0;
    }

    if (pos == ctx->subtitle_sync_last_pos)
    {
        return;
    }

    karaoke_demo_set_time_ms((uint32_t)pos);
    karaoke_demo_request_refresh_async();
    ctx->subtitle_sync_last_pos = pos;
}

static void ktv_player_ui_stop_subtitle_sync_timer(void)
{
    if (g_ktv_player_ui_ctx.subtitle_sync_timer == NULL)
    {
        return;
    }

    lv_timer_del(g_ktv_player_ui_ctx.subtitle_sync_timer);
    g_ktv_player_ui_ctx.subtitle_sync_timer = NULL;
    g_ktv_player_ui_ctx.subtitle_sync_last_pos = -1;
    dbg_print("[KTV_UI] subtitle sync timer stop");
}

static void ktv_player_ui_start_subtitle_sync_timer_if_needed(void)
{
    if (g_ktv_player_ui_ctx.current_subtitle_path[0] == '\0')
    {
        return;
    }

    if (g_ktv_player_ui_ctx.subtitle_sync_timer != NULL)
    {
        return;
    }

    g_ktv_player_ui_ctx.subtitle_sync_last_pos = -1;
    g_ktv_player_ui_ctx.subtitle_sync_timer = lv_timer_create(ktv_player_ui_subtitle_sync_timer_cb,
                                                              15,
                                                              &g_ktv_player_ui_ctx);
    if (g_ktv_player_ui_ctx.subtitle_sync_timer == NULL)
    {
        dbg_print("[KTV_UI] subtitle sync timer create failed");
        return;
    }

    dbg_print("[KTV_UI] subtitle sync timer start");
}

/*
 * 自动同步字幕层
 * 1. 当前有字幕：绑定字幕并启动字幕时钟
 * 2. 当前无字幕：关闭字幕层
 */
static void ktv_player_ui_sync_karaoke_on_play(void)
{
    const char *subtitle_path = g_ktv_player_ui_ctx.current_subtitle_path;

    if (subtitle_path == NULL || subtitle_path[0] == '\0')
    {
        (void)karaoke_demo_bind_subtitle("");
        dbg_print("[KTV_UI] karaoke sync: subtitle empty, renderer disabled");
        return;
    }

    (void)karaoke_demo_bind_subtitle(subtitle_path);
    karaoke_demo_set_time_ms(0);
    karaoke_demo_play();
    karaoke_demo_request_refresh_async();
    ktv_player_ui_start_subtitle_sync_timer_if_needed();
    dbg_print("[KTV_UI] karaoke sync: subtitle=%s", subtitle_path);
}

static void ktv_player_ui_sync_karaoke_pause(void)
{
    karaoke_demo_pause();
    dbg_print("[KTV_UI] karaoke sync: pause");
}

static void ktv_player_ui_sync_karaoke_resume(void)
{
    if (g_ktv_player_ui_ctx.current_subtitle_path[0] == '\0')
    {
        dbg_print("[KTV_UI] karaoke sync: resume skipped, no subtitle");
        return;
    }

    karaoke_demo_play();
    karaoke_demo_request_refresh_async();
    ktv_player_ui_start_subtitle_sync_timer_if_needed();
    dbg_print("[KTV_UI] karaoke sync: resume");
}

static void ktv_player_ui_sync_karaoke_stop(void)
{
    karaoke_demo_stop();
    dbg_print("[KTV_UI] karaoke sync: stop");
}

/*
 * 清空当前媒体信息
 */
static void ktv_player_ui_clear_current_media_info(void)
{
    memset(g_ktv_player_ui_ctx.current_mv_url, 0, sizeof(g_ktv_player_ui_ctx.current_mv_url));
    memset(g_ktv_player_ui_ctx.current_audio_url, 0, sizeof(g_ktv_player_ui_ctx.current_audio_url));
    memset(g_ktv_player_ui_ctx.current_subtitle_path, 0, sizeof(g_ktv_player_ui_ctx.current_subtitle_path));
    g_ktv_player_ui_ctx.current_has_audio = -1;
}

/*
 * 清空旧 sem 计数，避免上一次 prepared 残留导致这一次误判成功
 */
static void ktv_player_ui_drain_sem(sem_t *sem)
{
    if (sem == NULL)
    {
        return;
    }

    while (sem_trywait(sem) == 0)
    {
        ;
    }
}

/*
 * 等待 prepared
 */
static int ktv_player_ui_wait_prepared(sem_t *sem, int timeout_ms)
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
 * 视频播放器回调
 */
static int ktv_player_ui_video_callback(void *user_data, int msg, int ext1, void *para)
{
    ktv_player_ui_ctx_t *ctx;

    (void)ext1;
    (void)para;

    ctx = (ktv_player_ui_ctx_t *)user_data;
    if (ctx == NULL)
    {
        return 0;
    }

    switch (msg)
    {
        case TPLAYER_NOTIFY_PREPARED:
        {
            ctx->video_prepared_ok = 1;
            sem_post(&ctx->video_prepared_sem);
            dbg_print("[KTV_UI][VIDEO] prepared");
            break;
        }

        case TPLAYER_NOTIFY_MEDIA_ERROR:
        {
            ctx->video_error_flag = 1;
            sem_post(&ctx->video_prepared_sem);
            dbg_print("[KTV_UI][VIDEO] media error");
            break;
        }

        case TPLAYER_NOTIFY_PLAYBACK_COMPLETE:
        {
            dbg_print("[KTV_UI][VIDEO] playback complete");
            break;
        }

        case TPLAYER_NOTIFY_NOT_SEEKABLE:
        {
            ctx->video_player.seekable_flag = 0;
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
 * 音频播放器回调
 */
static int ktv_player_ui_audio_callback(void *user_data, int msg, int ext1, void *para)
{
    ktv_player_ui_ctx_t *ctx;

    (void)ext1;
    (void)para;

    ctx = (ktv_player_ui_ctx_t *)user_data;
    if (ctx == NULL)
    {
        return 0;
    }

    switch (msg)
    {
        case TPLAYER_NOTIFY_PREPARED:
        {
            ctx->audio_prepared_ok = 1;
            sem_post(&ctx->audio_prepared_sem);
            dbg_print("[KTV_UI][AUDIO] prepared");
            break;
        }

        case TPLAYER_NOTIFY_MEDIA_ERROR:
        {
            ctx->audio_error_flag = 1;
            sem_post(&ctx->audio_prepared_sem);
            dbg_print("[KTV_UI][AUDIO] media error");
            break;
        }

        case TPLAYER_NOTIFY_PLAYBACK_COMPLETE:
        {
            dbg_print("[KTV_UI][AUDIO] playback complete");
            break;
        }

        case TPLAYER_NOTIFY_NOT_SEEKABLE:
        {
            ctx->audio_player.seekable_flag = 0;
            break;
        }

        default:
        {
            break;
        }
    }

    return 0;
}

static int ktv_player_ui_recreate_players(void)
{
    if (ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] video deinit before recreate failed");
    }

    if (ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] audio deinit before recreate failed");
    }

    if (ktv_player_wrapper_init_with_type(&g_ktv_player_ui_ctx.video_player, CEDARX_PLAYER) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] recreate video player failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_init_with_type(&g_ktv_player_ui_ctx.audio_player, AUDIO_PLAYER) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] recreate audio player failed");
        (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_set_callback(&g_ktv_player_ui_ctx.video_player,
                                        ktv_player_ui_video_callback,
                                        &g_ktv_player_ui_ctx) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] recreate set video callback failed");
        (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_set_callback(&g_ktv_player_ui_ctx.audio_player,
                                        ktv_player_ui_audio_callback,
                                        &g_ktv_player_ui_ctx) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] recreate set audio callback failed");
        (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerSetG2dRotate(g_ktv_player_ui_ctx.video_player.handle,
                            TPLAYER_VIDEO_ROTATE_DEGREE_90) != 0)
    {
        dbg_print("[KTV_UI] recreate set rotate failed");
        (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.rect_valid != 0)
    {
        if (ktv_player_wrapper_set_display_rect(&g_ktv_player_ui_ctx.video_player,
                                                &g_ktv_player_ui_ctx.current_rect) != KTV_PLAYER_RET_OK)
        {
            dbg_print("[KTV_UI] recreate set rect failed, x=%d, y=%d, w=%d, h=%d",
                      g_ktv_player_ui_ctx.current_rect.x,
                      g_ktv_player_ui_ctx.current_rect.y,
                      g_ktv_player_ui_ctx.current_rect.width,
                      g_ktv_player_ui_ctx.current_rect.height);
            (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
            (void)ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
            return KTV_PLAYER_RET_FAIL;
        }
    }

    dbg_print("[KTV_UI] recreate players success");
    return KTV_PLAYER_RET_OK;
}

static int ktv_player_ui_stop_inner_ex(int recreate_players)
{
    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    /* 先停字幕线程，避免它在 stop/destroy 过程中继续读 position */
    ktv_player_ui_stop_subtitle_sync_timer();
    ktv_player_ui_sync_karaoke_stop();

    (void)ktv_player_wrapper_stop(&g_ktv_player_ui_ctx.video_player);
    (void)ktv_player_wrapper_stop(&g_ktv_player_ui_ctx.audio_player);

    if (recreate_players != 0)
    {
        if (ktv_player_ui_recreate_players() != KTV_PLAYER_RET_OK)
        {
            dbg_print("[KTV_UI] recreate players failed after stop");
            return KTV_PLAYER_RET_FAIL;
        }
    }

    g_ktv_player_ui_ctx.state = KTV_PLAYER_UI_STATE_IDLE;
    g_ktv_player_ui_ctx.video_prepared_ok = 0;
    g_ktv_player_ui_ctx.audio_prepared_ok = 0;
    g_ktv_player_ui_ctx.video_error_flag = 0;
    g_ktv_player_ui_ctx.audio_error_flag = 0;
    g_ktv_player_ui_ctx.audio_enabled = 0;
    ktv_player_ui_clear_current_media_info();
    ktv_player_ui_drain_sem(&g_ktv_player_ui_ctx.video_prepared_sem);
    ktv_player_ui_drain_sem(&g_ktv_player_ui_ctx.audio_prepared_sem);

    return KTV_PLAYER_RET_OK;
}

/*
 * 应用显示区域
 * 只作用于视频播放器
 */
static int ktv_player_ui_apply_rect(int x, int y, int width, int height)
{
    ktv_player_rect_t rect;

    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;

    g_ktv_player_ui_ctx.current_rect = rect;
    g_ktv_player_ui_ctx.rect_valid = 1;

    if (ktv_player_wrapper_set_display_rect(&g_ktv_player_ui_ctx.video_player, &rect) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] set rect failed, x=%d, y=%d, w=%d, h=%d",
                  x,
                  y,
                  width,
                  height);
        return KTV_PLAYER_RET_FAIL;
    }

    dbg_print("[KTV_UI] set rect success, x=%d, y=%d, w=%d, h=%d",
              x,
              y,
              width,
              height);

    return KTV_PLAYER_RET_OK;
}

/*
 * 内部停止
 * 最稳策略：切歌时不再复用旧实例，而是停止后重建 video/audio 两个播放器。
 */
static int ktv_player_ui_stop_inner(void)
{
    return ktv_player_ui_stop_inner_ex(1);
}

/*
 * 单路播放：仅视频播放器
 * has_audio=1 时使用本模式。
 * 注意：只要传了 subtitle_path，就照样允许字幕。
 */
static int ktv_player_ui_play_video_only(const char *video_url,
                                         const char *subtitle_path)
{
    char normalized_video_url[KTV_PLAYER_MAX_URL_LEN];

    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        dbg_print("[KTV_UI] not init");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_ui_str_is_empty(video_url))
    {
        dbg_print("[KTV_UI] play video only failed: video empty");
        return KTV_PLAYER_RET_FAIL;
    }

    ktv_player_ui_normalize_play_url_to_http(normalized_video_url,
                                             sizeof(normalized_video_url),
                                             video_url);

    if (ktv_player_ui_str_is_empty(normalized_video_url))
    {
        dbg_print("[KTV_UI] play video only failed: normalized video empty");
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.state == KTV_PLAYER_UI_STATE_PLAYING ||
        g_ktv_player_ui_ctx.state == KTV_PLAYER_UI_STATE_PAUSED)
    {
        if (ktv_player_ui_stop_inner() != KTV_PLAYER_RET_OK)
        {
            dbg_print("[KTV_UI] stop current before video-only play failed");
            return KTV_PLAYER_RET_FAIL;
        }
    }

    ktv_player_ui_drain_sem(&g_ktv_player_ui_ctx.video_prepared_sem);
    g_ktv_player_ui_ctx.video_prepared_ok = 0;
    g_ktv_player_ui_ctx.video_error_flag = 0;
    g_ktv_player_ui_ctx.audio_enabled = 0;
    ktv_player_ui_stop_subtitle_sync_timer();

    if (ktv_player_wrapper_set_source(&g_ktv_player_ui_ctx.video_player, normalized_video_url) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] set video source failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_prepare_async(&g_ktv_player_ui_ctx.video_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] prepare video async failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_ui_wait_prepared(&g_ktv_player_ui_ctx.video_prepared_sem,
                                    KTV_PLAYER_PREPARE_TIMEOUT_MS) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] wait video prepared timeout");
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.video_error_flag != 0 || g_ktv_player_ui_ctx.video_prepared_ok == 0)
    {
        dbg_print("[KTV_UI] video prepared failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_start(&g_ktv_player_ui_ctx.video_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] start video failed");
        return KTV_PLAYER_RET_FAIL;
    }

    g_ktv_player_ui_ctx.state = KTV_PLAYER_UI_STATE_PLAYING;
    snprintf(g_ktv_player_ui_ctx.current_mv_url, sizeof(g_ktv_player_ui_ctx.current_mv_url), "%s", normalized_video_url);
    g_ktv_player_ui_ctx.current_audio_url[0] = '\0';
    snprintf(g_ktv_player_ui_ctx.current_subtitle_path, sizeof(g_ktv_player_ui_ctx.current_subtitle_path), "%s",
             (subtitle_path == NULL) ? "" : subtitle_path);
    g_ktv_player_ui_ctx.current_has_audio = 1;
    ktv_player_ui_sync_karaoke_on_play();
    ktv_player_ui_start_subtitle_sync_timer_if_needed();

    dbg_print("[KTV_UI] video only play success, video=%s, subtitle=%s",
              normalized_video_url,
              g_ktv_player_ui_ctx.current_subtitle_path);
    return KTV_PLAYER_RET_OK;
}

/*
 * 双路播放：无声视频 + 独立音频
 * 这里严格按 log_5.txt 的顺序实现：
 * 1. 两路都 PrepareAsync
 * 2. 先等音频 prepared，再等视频 prepared
 * 3. 视频静音、音频放量
 * 4. 先启动音频，再延时 300ms 启动视频
 */
static int ktv_player_ui_play_video_audio(const char *video_url,
                                          const char *audio_url,
                                          const char *subtitle_path)
{
    char normalized_video_url[KTV_PLAYER_MAX_URL_LEN];
    char normalized_audio_url[KTV_PLAYER_MAX_URL_LEN];

    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        dbg_print("[KTV_UI] not init");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_ui_str_is_empty(video_url) || ktv_player_ui_str_is_empty(audio_url))
    {
        dbg_print("[KTV_UI] play video+audio failed: video or audio empty");
        return KTV_PLAYER_RET_FAIL;
    }

    ktv_player_ui_normalize_play_url_to_http(normalized_video_url,
                                             sizeof(normalized_video_url),
                                             video_url);
    ktv_player_ui_normalize_play_url_to_http(normalized_audio_url,
                                             sizeof(normalized_audio_url),
                                             audio_url);

    if (ktv_player_ui_str_is_empty(normalized_video_url) || ktv_player_ui_str_is_empty(normalized_audio_url))
    {
        dbg_print("[KTV_UI] play video+audio failed: normalized url empty");
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.state == KTV_PLAYER_UI_STATE_PLAYING ||
        g_ktv_player_ui_ctx.state == KTV_PLAYER_UI_STATE_PAUSED)
    {
        if (ktv_player_ui_stop_inner() != KTV_PLAYER_RET_OK)
        {
            dbg_print("[KTV_UI] stop current before video+audio play failed");
            return KTV_PLAYER_RET_FAIL;
        }
    }

    ktv_player_ui_drain_sem(&g_ktv_player_ui_ctx.video_prepared_sem);
    ktv_player_ui_drain_sem(&g_ktv_player_ui_ctx.audio_prepared_sem);

    g_ktv_player_ui_ctx.video_prepared_ok = 0;
    g_ktv_player_ui_ctx.audio_prepared_ok = 0;
    g_ktv_player_ui_ctx.video_error_flag = 0;
    g_ktv_player_ui_ctx.audio_error_flag = 0;
    g_ktv_player_ui_ctx.audio_enabled = 1;
    ktv_player_ui_stop_subtitle_sync_timer();

    if (ktv_player_wrapper_set_source(&g_ktv_player_ui_ctx.video_player, normalized_video_url) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] set video source failed, url=%s", normalized_video_url);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_set_source(&g_ktv_player_ui_ctx.audio_player, normalized_audio_url) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] set audio source failed, url=%s", normalized_audio_url);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_prepare_async(&g_ktv_player_ui_ctx.audio_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] prepare audio async failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_prepare_async(&g_ktv_player_ui_ctx.video_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] prepare video async failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_ui_wait_prepared(&g_ktv_player_ui_ctx.audio_prepared_sem,
                                    KTV_PLAYER_PREPARE_TIMEOUT_MS) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] wait audio prepared timeout");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_ui_wait_prepared(&g_ktv_player_ui_ctx.video_prepared_sem,
                                    KTV_PLAYER_PREPARE_TIMEOUT_MS) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] wait video prepared timeout");
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.audio_error_flag != 0 || g_ktv_player_ui_ctx.audio_prepared_ok == 0)
    {
        dbg_print("[KTV_UI] audio prepared failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.video_error_flag != 0 || g_ktv_player_ui_ctx.video_prepared_ok == 0)
    {
        dbg_print("[KTV_UI] video prepared failed");
        return KTV_PLAYER_RET_FAIL;
    }

    /*
     * 不把设置音量失败当致命错误。
     * 有些板子/SDK 可能不暴露 TPlayerSetVolume，或者视频文件本身已经是无声轨。
     */
    if (ktv_player_wrapper_set_volume(&g_ktv_player_ui_ctx.video_player, 0) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] warning: mute video failed, continue");
    }

    if (ktv_player_wrapper_set_volume(&g_ktv_player_ui_ctx.audio_player, 40) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] warning: set audio volume failed, continue");
    }

    dbg_print("[KTV_UI] start audio first...");
    if (ktv_player_wrapper_start(&g_ktv_player_ui_ctx.audio_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] start audio failed");
        return KTV_PLAYER_RET_FAIL;
    }

    usleep(300000);

    dbg_print("[KTV_UI] then start video...");
    if (ktv_player_wrapper_start(&g_ktv_player_ui_ctx.video_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] start video failed");
        return KTV_PLAYER_RET_FAIL;
    }

    g_ktv_player_ui_ctx.state = KTV_PLAYER_UI_STATE_PLAYING;
    snprintf(g_ktv_player_ui_ctx.current_mv_url, sizeof(g_ktv_player_ui_ctx.current_mv_url), "%s", normalized_video_url);
    snprintf(g_ktv_player_ui_ctx.current_audio_url, sizeof(g_ktv_player_ui_ctx.current_audio_url), "%s", normalized_audio_url);
    snprintf(g_ktv_player_ui_ctx.current_subtitle_path, sizeof(g_ktv_player_ui_ctx.current_subtitle_path), "%s",
             (subtitle_path == NULL) ? "" : subtitle_path);
    g_ktv_player_ui_ctx.current_has_audio = 0;
    ktv_player_ui_sync_karaoke_on_play();
    ktv_player_ui_start_subtitle_sync_timer_if_needed();

    dbg_print("[KTV_UI] video+audio play success, video=%s, audio=%s, subtitle=%s",
              g_ktv_player_ui_ctx.current_mv_url,
              g_ktv_player_ui_ctx.current_audio_url,
              g_ktv_player_ui_ctx.current_subtitle_path);
    return KTV_PLAYER_RET_OK;
}

/*
 * 初始化
 * 默认先设置小屏显示
 */
int ktv_player_ui_init(void)
{
    if (g_ktv_player_ui_ctx.init_flag != 0)
    {
        return KTV_PLAYER_RET_OK;
    }

    memset(&g_ktv_player_ui_ctx, 0, sizeof(g_ktv_player_ui_ctx));

    if (sem_init(&g_ktv_player_ui_ctx.video_prepared_sem, 0, 0) != 0)
    {
        dbg_print("[KTV_UI] video sem_init failed");
        return KTV_PLAYER_RET_FAIL;
    }

    if (sem_init(&g_ktv_player_ui_ctx.audio_prepared_sem, 0, 0) != 0)
    {
        dbg_print("[KTV_UI] audio sem_init failed");
        sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_init_with_type(&g_ktv_player_ui_ctx.video_player, CEDARX_PLAYER) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] init video player failed");
        sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
        sem_destroy(&g_ktv_player_ui_ctx.audio_prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_init_with_type(&g_ktv_player_ui_ctx.audio_player, AUDIO_PLAYER) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] init audio player failed");
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
        sem_destroy(&g_ktv_player_ui_ctx.audio_prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_set_callback(&g_ktv_player_ui_ctx.video_player,
                                        ktv_player_ui_video_callback,
                                        &g_ktv_player_ui_ctx) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] set video callback failed");
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
        sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
        sem_destroy(&g_ktv_player_ui_ctx.audio_prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_wrapper_set_callback(&g_ktv_player_ui_ctx.audio_player,
                                        ktv_player_ui_audio_callback,
                                        &g_ktv_player_ui_ctx) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] set audio callback failed");
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
        sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
        sem_destroy(&g_ktv_player_ui_ctx.audio_prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    if (TPlayerSetG2dRotate(g_ktv_player_ui_ctx.video_player.handle,
                            TPLAYER_VIDEO_ROTATE_DEGREE_90) != 0)
    {
        dbg_print("[KTV_UI] TPlayerSetG2dRotate 90 failed");
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
        sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
        sem_destroy(&g_ktv_player_ui_ctx.audio_prepared_sem);
        return KTV_PLAYER_RET_FAIL;
    }

    g_ktv_player_ui_ctx.state = KTV_PLAYER_UI_STATE_IDLE;
    g_ktv_player_ui_ctx.init_flag = 1;
    g_ktv_player_ui_ctx.subtitle_sync_timer = NULL;
    g_ktv_player_ui_ctx.subtitle_sync_last_pos = -1;
    ktv_player_ui_clear_current_media_info();

    if (ktv_player_ui_set_small_rect() != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] default small rect failed");
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
        ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
        sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
        sem_destroy(&g_ktv_player_ui_ctx.audio_prepared_sem);
        memset(&g_ktv_player_ui_ctx, 0, sizeof(g_ktv_player_ui_ctx));
        return KTV_PLAYER_RET_FAIL;
    }

    dbg_print("[KTV_UI] init success");
    return KTV_PLAYER_RET_OK;
}

/*
 * 释放
 */
int ktv_player_ui_deinit(void)
{
    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        return KTV_PLAYER_RET_OK;
    }

    (void)ktv_player_ui_stop_inner_ex(0);
    ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.video_player);
    ktv_player_wrapper_deinit(&g_ktv_player_ui_ctx.audio_player);
    sem_destroy(&g_ktv_player_ui_ctx.video_prepared_sem);
    sem_destroy(&g_ktv_player_ui_ctx.audio_prepared_sem);

    memset(&g_ktv_player_ui_ctx, 0, sizeof(g_ktv_player_ui_ctx));

    dbg_print("[KTV_UI] deinit success");
    return KTV_PLAYER_RET_OK;
}

/*
 * 纯播放接口
 */
int ktv_player_ui_play(const ktv_player_media_param_t *param)
{
    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        dbg_print("[KTV_UI] play failed: not init");
        return KTV_PLAYER_RET_FAIL;
    }

    if (param == NULL)
    {
        dbg_print("[KTV_UI] play failed: param null");
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_ui_str_is_empty(param->mp4_url))
    {
        dbg_print("[KTV_UI] play failed: mp4 empty");
        return KTV_PLAYER_RET_FAIL;
    }

    if (param->has_audio != 0 && param->has_audio != 1)
    {
        dbg_print("[KTV_UI] play failed: has_audio invalid=%d", param->has_audio);
        return KTV_PLAYER_RET_FAIL;
    }

    if (param->has_audio == 0 && ktv_player_ui_str_is_empty(param->mp3_url))
    {
        dbg_print("[KTV_UI] play failed: has_audio=0 but mp3 empty");
        return KTV_PLAYER_RET_FAIL;
    }

    if (param->has_audio != 0)
    {
        return ktv_player_ui_play_video_only(param->mp4_url,
                                             param->subtitle_path);
    }

    return ktv_player_ui_play_video_audio(param->mp4_url,
                                          param->mp3_url,
                                          param->subtitle_path);
}

int ktv_player_ui_replay_current(void)
{
    ktv_player_media_param_t replay_param;

    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        dbg_print("[KTV_UI] replay current failed: not init");
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.current_mv_url[0] == '\0')
    {
        dbg_print("[KTV_UI] replay current failed: current mv empty");
        return KTV_PLAYER_RET_FAIL;
    }

    replay_param.mp4_url = g_ktv_player_ui_ctx.current_mv_url;
    replay_param.mp3_url = (g_ktv_player_ui_ctx.current_audio_url[0] != '\0') ? g_ktv_player_ui_ctx.current_audio_url : NULL;
    replay_param.subtitle_path = (g_ktv_player_ui_ctx.current_subtitle_path[0] != '\0') ? g_ktv_player_ui_ctx.current_subtitle_path : NULL;
    replay_param.has_audio = g_ktv_player_ui_ctx.current_has_audio;

    return ktv_player_ui_play(&replay_param);
}

/*
 * 获取当前字幕路径
 */
const char *ktv_player_ui_get_current_subtitle_path(void)
{
    return g_ktv_player_ui_ctx.current_subtitle_path;
}

int ktv_player_ui_current_has_subtitle(void)
{
    return (g_ktv_player_ui_ctx.current_subtitle_path[0] != '\0') ? 1 : 0;
}

/*
 * 暂停播放
 * 双路模式下，视频和音频需要一起暂停
 */
int ktv_player_ui_pause(void)
{
    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.state != KTV_PLAYER_UI_STATE_PLAYING)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.audio_enabled != 0)
    {
        if (ktv_player_wrapper_pause(&g_ktv_player_ui_ctx.audio_player) != KTV_PLAYER_RET_OK)
        {
            dbg_print("[KTV_UI] pause audio failed");
            return KTV_PLAYER_RET_FAIL;
        }
    }

    if (ktv_player_wrapper_pause(&g_ktv_player_ui_ctx.video_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] pause video failed");
        return KTV_PLAYER_RET_FAIL;
    }

    g_ktv_player_ui_ctx.state = KTV_PLAYER_UI_STATE_PAUSED;
    ktv_player_ui_sync_karaoke_pause();

    dbg_print("[KTV_UI] pause success, audio_enabled=%d", g_ktv_player_ui_ctx.audio_enabled);
    return KTV_PLAYER_RET_OK;
}

/*
 * 恢复播放
 * 双路模式下，视频和音频需要一起恢复
 */
int ktv_player_ui_resume(void)
{
    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.state == KTV_PLAYER_UI_STATE_PLAYING)
    {
        return KTV_PLAYER_RET_OK;
    }

    if (g_ktv_player_ui_ctx.state != KTV_PLAYER_UI_STATE_PAUSED)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (g_ktv_player_ui_ctx.audio_enabled != 0)
    {
        if (ktv_player_wrapper_start(&g_ktv_player_ui_ctx.audio_player) != KTV_PLAYER_RET_OK)
        {
            dbg_print("[KTV_UI] resume audio failed");
            return KTV_PLAYER_RET_FAIL;
        }
    }

    if (ktv_player_wrapper_start(&g_ktv_player_ui_ctx.video_player) != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] resume video failed");
        return KTV_PLAYER_RET_FAIL;
    }

    g_ktv_player_ui_ctx.state = KTV_PLAYER_UI_STATE_PLAYING;
    ktv_player_ui_sync_karaoke_resume();

    dbg_print("[KTV_UI] resume success, audio_enabled=%d", g_ktv_player_ui_ctx.audio_enabled);
    return KTV_PLAYER_RET_OK;
}

/*
 * 停止播放
 */
int ktv_player_ui_stop(void)
{
    if (g_ktv_player_ui_ctx.init_flag == 0)
    {
        return KTV_PLAYER_RET_FAIL;
    }

    if (ktv_player_ui_stop_inner() != KTV_PLAYER_RET_OK)
    {
        dbg_print("[KTV_UI] stop failed");
        return KTV_PLAYER_RET_FAIL;
    }

    dbg_print("[KTV_UI] stop success");
    return KTV_PLAYER_RET_OK;
}

/*
 * 设置小屏显示区域
 */
int ktv_player_ui_set_small_rect(void)
{
    return ktv_player_ui_apply_rect(KTV_PLAYER_SMALL_RECT_X,
                                    KTV_PLAYER_SMALL_RECT_Y,
                                    KTV_PLAYER_SMALL_RECT_W,
                                    KTV_PLAYER_SMALL_RECT_H);
}

int ktv_player_ui_set_right_rect(void)
{
    return ktv_player_ui_apply_rect(115,
                                    KTV_PLAYER_SMALL_RECT_Y,
                                    570,
                                    1280 - KTV_PLAYER_SMALL_RECT_Y);
}

/*
 * 设置全屏显示区域
 */
int ktv_player_ui_set_full_rect(void)
{
    return ktv_player_ui_apply_rect(0,
                                    0,
                                    KTV_PLAYER_SCREEN_WIDTH,
                                    KTV_PLAYER_SCREEN_HEIGHT);
}

/*
 * 设置自定义显示区域
 */
int ktv_player_ui_set_rect(int x, int y, int width, int height)
{
    return ktv_player_ui_apply_rect(x, y, width, height);
}

/*************************************************
 * KTV播放器UI接口（纯播放器）
 * 初始化：ktv_player_ui_init();
 * 播放：ktv_player_ui_play(&param);
 * 重播：ktv_player_ui_replay_current();
 * 暂停/恢复/停止：ktv_player_ui_pause/resume/stop();
 * 当前字幕路径：ktv_player_ui_get_current_subtitle_path();
 * 退出：ktv_player_ui_deinit();
 *************************************************/
