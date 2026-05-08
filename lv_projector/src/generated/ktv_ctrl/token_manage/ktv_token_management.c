#include "ktv_token_management.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "gui_guider.h"

#include "ktv_time_service.h"
#include "ktv_token_service.h"

/*
 * This file is a thin facade that preserves the old public API:
 * - ktv_time_thread_start/stop
 * - ktv_get_current_time_sec
 * - ktv_get_token
 *
 * Heavy logic is moved into:
 * - ktv_time_service.[ch]  (time sync, base calculation, persistence)
 * - ktv_token_service.[ch] (token fetch, caching, persistence)
 *
 * UI update is kept here on purpose: it is a presentation concern.
 */

#define KTV_BEIJING_UTC_OFFSET_SEC  (8 * 60 * 60)

static char g_ktv_time_show[16] = {0};

static void format_beijing_time(uint32_t utc_sec, char *buf, size_t buf_size)
{
    if (buf == NULL || buf_size == 0U) {
        return;
    }

    if (utc_sec == 0U) {
        snprintf(buf, buf_size, "time_invalid");
        return;
    }

    /*
     * 固定北京时间：UTC 秒数 + 8 小时，然后用 gmtime_r 展开。
     * 不使用 localtime_r，避免嵌入式 Linux 系统时区未配置导致打印 UTC 或错误时区。
     */
    time_t t = (time_t)((uint64_t)utc_sec + (uint64_t)KTV_BEIJING_UTC_OFFSET_SEC);
    struct tm tm_info;

    if (gmtime_r(&t, &tm_info) != NULL) {
        strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", &tm_info);
    } else {
        snprintf(buf, buf_size, "gmtime_failed");
    }
}

static void ktv_time_ui_update_async(void *arg)
{
    (void)arg;

    if (guider_ui.screen_7 &&
        lv_obj_is_valid(guider_ui.screen_7) &&
        guider_ui.screen_7_label_1 &&
        lv_obj_is_valid(guider_ui.screen_7_label_1)) {
        lv_label_set_text(guider_ui.screen_7_label_1, g_ktv_time_show);
    }

    if (guider_ui.screen_8 &&
        lv_obj_is_valid(guider_ui.screen_8) &&
        guider_ui.screen_8_label_2 &&
        lv_obj_is_valid(guider_ui.screen_8_label_2)) {
        lv_label_set_text(guider_ui.screen_8_label_2, g_ktv_time_show);
    }
}

static void ktv_time_update_show_and_async_ui(uint32_t now_sec)
{
    char time_buf[32] = {0};

    format_beijing_time(now_sec, time_buf, sizeof(time_buf));

    /*
     * format_beijing_time 输出：
     * 2026-04-26 20:35:12
     * 这里只取 20:35
     */
    if (strlen(time_buf) >= 16U) {
        snprintf(g_ktv_time_show, sizeof(g_ktv_time_show), "%.5s", time_buf + 11);
    } else {
        snprintf(g_ktv_time_show, sizeof(g_ktv_time_show), "%s", time_buf);
    }

    lv_async_call(ktv_time_ui_update_async, NULL);
}

static void ktv_time_service_tick_hook(uint32_t now_sec, const char *reason, void *user)
{
    (void)user;

    /*
     * Keep legacy behavior:
     * - print Beijing time for logs (does not rely on system timezone)
     * - update UI label asynchronously
     */
    char time_buf[32] = {0};
    format_beijing_time(now_sec, time_buf, sizeof(time_buf));

    /* Update show string first, then log the same "show". */
    ktv_time_update_show_and_async_ui(now_sec);

    // printf("[KTV_TIME] %s utc=%u beijing=%s show=%s\n",
    //        (reason != NULL) ? reason : "log",
    //        (unsigned)now_sec,
    //        time_buf,
    //        g_ktv_time_show);
}

static pthread_t g_bootstrap_thread;
static int g_bootstrap_thread_started = 0;

static void *ktv_bootstrap_prefetch_main(void *arg)
{
    (void)arg;

    /*
     * Boot-time one-shot actions (best-effort, non-blocking for main thread):
     * - sync time once
     * - fetch token once
     * - refresh label once
     */
    (void)ktv_time_service_sync_now();

    ktv_token_service_init();
    (void)ktv_token_service_refresh_now();

    ktv_time_update_show_and_async_ui(ktv_time_service_now_sec());
    return NULL;
}

void ktv_time_refresh_label_async(void)
{
    ktv_time_update_show_and_async_ui(ktv_time_service_now_sec());
}

int ktv_time_thread_start(void)
{
    ktv_time_service_init();
    ktv_time_service_set_tick_hook(ktv_time_service_tick_hook, NULL);
    if (!g_bootstrap_thread_started) {
        g_bootstrap_thread_started = 1;
        if (pthread_create(&g_bootstrap_thread, NULL, ktv_bootstrap_prefetch_main, NULL) == 0) {
            pthread_detach(g_bootstrap_thread);
        }
    }

    return ktv_time_service_thread_start();
}

void ktv_time_thread_stop(void)
{
    ktv_time_service_thread_stop();
}

uint32_t ktv_get_current_time_sec(void)
{
    ktv_time_service_init();
    return ktv_time_service_now_sec();
}

const char *ktv_get_token(void)
{
    ktv_token_service_init();
    return ktv_token_service_get();
}
