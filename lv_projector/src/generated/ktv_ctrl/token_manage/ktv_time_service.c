#include "ktv_time_service.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "cJSON.h"
#include "ktv_http_fetch_api.h"
#include "ktv_persist_util.h"

#define TIME_SERVER_URL "http://106.14.47.51:8080/getUrl/getCurrentTime"

#define KTV_TIME_SYNC_INTERVAL_SEC   (60 * 60)  /* 每小时校准一次 */
#define KTV_TIME_LOG_INTERVAL_SEC    (60)       /* 每分钟打印一次 */

/* Persist only what is meaningful across reboot. */
#define KTV_TIME_PERSIST_PATH "/usr/share/lv_projector/ktv_time_cache.json"

typedef struct
{
    uint64_t base_server_sec;   /* 服务器绝对时间（秒） */
    uint64_t base_mono_ms;      /* 设置基准时的单调时钟（毫秒） */
    int base_valid;
    pthread_mutex_t lock;
} ktv_time_state_t;

typedef struct
{
    uint64_t last_sync_fail_ms;
    uint64_t last_sync_log_ms;
    uint64_t last_minute_log_ms;
    int offline;
} ktv_time_net_guard_t;

static ktv_time_state_t g_time = {
    .base_server_sec = 0U,
    .base_mono_ms = 0U,
    .base_valid = 0,
    .lock = PTHREAD_MUTEX_INITIALIZER,
};

static ktv_time_net_guard_t g_time_net = {
    .last_sync_fail_ms = 0ULL,
    .last_sync_log_ms = 0ULL,
    .last_minute_log_ms = 0ULL,
    .offline = 0,
};

static pthread_t g_time_thread;
static volatile int g_time_thread_running = 0;
static int g_time_thread_created = 0;

static ktv_time_service_on_tick_cb g_tick_cb = NULL;
static void *g_tick_user = NULL;

void ktv_time_service_set_tick_hook(ktv_time_service_on_tick_cb cb, void *user)
{
    /* Single writer expected during init; keep it simple. */
    g_tick_cb = cb;
    g_tick_user = user;
}

static uint64_t ktv_time_get_monotonic_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
    }

    /* 降级：使用 gettimeofday（可能不够单调，但用于兜底）。 */
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == 0) {
        return (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000ULL;
    }

    return 0ULL;
}

static uint64_t ktv_time_wall_now_ms(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        return 0ULL;
    }
    return ((uint64_t)tv.tv_sec * 1000ULL) + ((uint64_t)tv.tv_usec / 1000ULL);
}

static uint64_t ktv_json_u64(cJSON *item)
{
    if (item == NULL) {
        return 0ULL;
    }

    if (cJSON_IsNumber(item)) {
        if (item->valuedouble <= 0) {
            return 0ULL;
        }
        return (uint64_t)item->valuedouble;
    }

    if (cJSON_IsString(item) && item->valuestring != NULL) {
        return (uint64_t)strtoull(item->valuestring, NULL, 10);
    }

    return 0ULL;
}

static void ktv_time_set_base(uint64_t server_sec, uint64_t mono_ms)
{
    pthread_mutex_lock(&g_time.lock);
    g_time.base_server_sec = server_sec;
    g_time.base_mono_ms = mono_ms;
    g_time.base_valid = (server_sec > 0ULL && mono_ms > 0ULL) ? 1 : 0;
    pthread_mutex_unlock(&g_time.lock);
}

static void ktv_time_persist_snapshot(uint64_t server_sec)
{
    char json_buf[128];
    int n;

    if (server_sec == 0ULL) {
        return;
    }

    n = snprintf(json_buf, sizeof(json_buf), "{\"server_sec\":%llu}\n",
                 (unsigned long long)server_sec);
    if (n <= 0 || (size_t)n >= sizeof(json_buf)) {
        return;
    }

    (void)ktv_persist_write_file_atomic(KTV_TIME_PERSIST_PATH, json_buf, (size_t)n);
}

static void ktv_time_try_load_persisted(void)
{
    char buf[256];
    int n;
    cJSON *root;
    cJSON *server_sec_item;
    uint64_t server_sec;
    uint64_t mono_ms;

    memset(buf, 0, sizeof(buf));
    n = ktv_persist_read_file(KTV_TIME_PERSIST_PATH, buf, sizeof(buf) - 1U);
    if (n <= 0) {
        return;
    }
    buf[sizeof(buf) - 1U] = '\0';

    root = cJSON_Parse(buf);
    if (root == NULL) {
        return;
    }

    server_sec_item = cJSON_GetObjectItem(root, "server_sec");
    server_sec = ktv_json_u64(server_sec_item);
    cJSON_Delete(root);

    if (server_sec == 0ULL) {
        return;
    }

    mono_ms = ktv_time_get_monotonic_ms();
    if (mono_ms == 0ULL) {
        return;
    }

    ktv_time_set_base(server_sec, mono_ms);
}

void ktv_time_service_init(void)
{
    /* Best-effort: allow offline time to tick after reboot. */
    ktv_time_try_load_persisted();
}

int ktv_time_service_sync_now(void)
{
    char response[512];
    cJSON *root;
    cJSON *currentTime;
    uint64_t server_ms;
    uint64_t server_sec;
    uint64_t mono_ms;
    uint64_t now_ms;

    /*
     * Offline/backoff guard:
     * - If last sync failed recently, skip repeated HTTP attempts.
     * - Log failure at most once per interval to avoid console spam when WiFi is down.
     */
    now_ms = ktv_time_wall_now_ms();
    if (g_time_net.last_sync_fail_ms != 0ULL &&
        now_ms != 0ULL &&
        (now_ms - g_time_net.last_sync_fail_ms) < 30000ULL) {
        return -1;
    }

    response[0] = '\0';
    if (ktv_http_get_to_memory(TIME_SERVER_URL, response, sizeof(response), 10000) == 0) {
        g_time_net.last_sync_fail_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
        g_time_net.offline = 1;
        if (now_ms == 0ULL || (now_ms - g_time_net.last_sync_log_ms) >= 30000ULL) {
            g_time_net.last_sync_log_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
            //printf("[KTV_TIME] sync failed: http (offline/backoff)\n");
        }
        return -1;
    }

    root = cJSON_Parse(response);
    if (root == NULL) {
        g_time_net.last_sync_fail_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
        g_time_net.offline = 1;
        if (now_ms == 0ULL || (now_ms - g_time_net.last_sync_log_ms) >= 30000ULL) {
            g_time_net.last_sync_log_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
            //printf("[KTV_TIME] sync failed: json parse (offline/backoff)\n");
        }
        return -1;
    }

    currentTime = cJSON_GetObjectItem(root, "currentTime");
    server_ms = ktv_json_u64(currentTime); /* 13位毫秒时间戳不能用 valueint */
    cJSON_Delete(root);

    if (server_ms == 0ULL) {
        g_time_net.last_sync_fail_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
        g_time_net.offline = 1;
        if (now_ms == 0ULL || (now_ms - g_time_net.last_sync_log_ms) >= 30000ULL) {
            g_time_net.last_sync_log_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
            //printf("[KTV_TIME] sync failed: invalid currentTime (offline/backoff)\n");
        }
        return -1;
    }

    server_sec = server_ms / 1000ULL;
    mono_ms = ktv_time_get_monotonic_ms();
    if (mono_ms == 0ULL) {
        g_time_net.last_sync_fail_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
        g_time_net.offline = 1;
        if (now_ms == 0ULL || (now_ms - g_time_net.last_sync_log_ms) >= 30000ULL) {
            g_time_net.last_sync_log_ms = (now_ms != 0ULL) ? now_ms : ktv_time_wall_now_ms();
            //printf("[KTV_TIME] sync failed: monotonic clock (offline/backoff)\n");
        }
        return -1;
    }

    ktv_time_set_base(server_sec, mono_ms);
    ktv_time_persist_snapshot(server_sec);

    if (g_time_net.offline) {
        //printf("[KTV_TIME] sync recovered\n");
    }
    g_time_net.offline = 0;
    g_time_net.last_sync_fail_ms = 0ULL;

    //printf("[KTV_TIME] sync ok: server_ms=%llu server_sec=%llu mono_ms=%llu\n",
        //    (unsigned long long)server_ms,
        //    (unsigned long long)server_sec,
        //    (unsigned long long)mono_ms);

    return 0;
}

uint32_t ktv_time_service_now_sec(void)
{
    uint64_t base_server;
    uint64_t base_mono;
    int valid;
    uint64_t now_mono;
    int64_t diff_ms;
    int64_t est_sec;

    pthread_mutex_lock(&g_time.lock);
    base_server = g_time.base_server_sec;
    base_mono = g_time.base_mono_ms;
    valid = g_time.base_valid;
    pthread_mutex_unlock(&g_time.lock);

    if (!valid) {
        /* 未校准：回退系统时间（可能不准，但不阻塞）。 */
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
            return (uint32_t)ts.tv_sec;
        }
        return 0U;
    }

    now_mono = ktv_time_get_monotonic_ms();
    diff_ms = (int64_t)now_mono - (int64_t)base_mono;
    est_sec = (int64_t)base_server + (diff_ms / 1000);

    if (est_sec < 0) {
        est_sec = 0;
    }
    return (uint32_t)est_sec;
}

static void ktv_time_log_minute(const char *tag)
{
    uint32_t now_sec = ktv_time_service_now_sec();
    uint64_t now_ms = ktv_time_wall_now_ms();

    /*
     * Reduce console spam when offline:
     * - Keep hook callbacks (UI refresh) at minute cadence.
     * - Throttle minute_log printing to at most once per 10 minutes while offline.
     */
    if (tag != NULL && strcmp(tag, "minute_log") == 0 && g_time_net.offline) {
        if (g_time_net.last_minute_log_ms != 0ULL &&
            now_ms != 0ULL &&
            (now_ms - g_time_net.last_minute_log_ms) < 600000ULL) {
            if (g_tick_cb != NULL) {
                g_tick_cb(now_sec, tag, g_tick_user);
            }
            return;
        }
        g_time_net.last_minute_log_ms = now_ms;
    }

    //printf("[KTV_TIME] %s utc=%u\n", (tag != NULL) ? tag : "log", now_sec);

    if (g_tick_cb != NULL) {
        g_tick_cb(now_sec, tag, g_tick_user);
    }
}

static void *ktv_time_thread_main(void *arg)
{
    unsigned int log_tick = 0U;
    unsigned int sync_tick = 0U;

    (void)arg;

    //printf("[KTV_TIME] thread start\n");

    /* 启动后立即校准一次 */
    if (ktv_time_service_sync_now() == 0) {
        ktv_time_log_minute("first_sync");
    } else {
        ktv_time_log_minute("first_sync_failed_fallback");
    }

    while (g_time_thread_running) {
        sleep(1);

        if (!g_time_thread_running) {
            break;
        }

        log_tick++;
        sync_tick++;

        if (sync_tick >= KTV_TIME_SYNC_INTERVAL_SEC) {
            sync_tick = 0U;
            if (ktv_time_service_sync_now() == 0) {
                ktv_time_log_minute("hour_sync");
            } else {
                ktv_time_log_minute("hour_sync_failed_fallback");
            }
        }

        if (log_tick >= KTV_TIME_LOG_INTERVAL_SEC) {
            log_tick = 0U;
            ktv_time_log_minute("minute_log");
        }
    }

    //printf("[KTV_TIME] thread exit\n");
    return NULL;
}

int ktv_time_service_thread_start(void)
{
    if (g_time_thread_running) {
        return 0;
    }

    g_time_thread_running = 1;
    if (pthread_create(&g_time_thread, NULL, ktv_time_thread_main, NULL) != 0) {
        g_time_thread_running = 0;
        g_time_thread_created = 0;
        return -1;
    }

    g_time_thread_created = 1;
    return 0;
}

void ktv_time_service_thread_stop(void)
{
    if (!g_time_thread_created) {
        return;
    }

    g_time_thread_running = 0;
    pthread_join(g_time_thread, NULL);
    g_time_thread_created = 0;
}

