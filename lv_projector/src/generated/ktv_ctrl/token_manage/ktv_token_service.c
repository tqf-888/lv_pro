#include "ktv_token_service.h"

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
#include "ktv_time_service.h"

/* 默认网络请求 URL */
#define TOKEN_URL "https://tuoge.djyos.com/sdkv2/api/channel/getApiToken"

#define KTV_TOKEN_PERSIST_PATH "/usr/share/lv_projector/ktv_token_cache.json"

typedef struct
{
    char token[128];
    uint32_t expire_time_sec; /* Unix 时间戳（秒） */
    uint32_t server_date_sec; /* Token 返回时的服务器当前时间（秒） */
    int initialized;
    pthread_mutex_t lock;
} ktv_token_state_t;

static ktv_token_state_t g_token = {
    .token = {0},
    .expire_time_sec = 0U,
    .server_date_sec = 0U,
    .initialized = 0,
    .lock = PTHREAD_MUTEX_INITIALIZER,
};

typedef struct
{
    uint64_t last_fail_ms;
    uint64_t last_log_ms;
    int offline;
} ktv_token_net_guard_t;

static ktv_token_net_guard_t g_token_net = {
    .last_fail_ms = 0ULL,
    .last_log_ms = 0ULL,
    .offline = 0,
};

static uint64_t ktv_token_wall_now_ms(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        return 0ULL;
    }
    return ((uint64_t)tv.tv_sec * 1000ULL) + ((uint64_t)tv.tv_usec / 1000ULL);
}

static uint32_t ktv_token_wall_now_sec(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        return (uint32_t)ts.tv_sec;
    }

    return 0U;
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

static void ktv_token_persist_snapshot_locked(void)
{
    char json_buf[512];
    int n;

    if (g_token.token[0] == '\0') {
        return;
    }

    n = snprintf(json_buf, sizeof(json_buf),
                 "{\"token\":\"%s\",\"expire_time_sec\":%u,\"server_date_sec\":%u}\n",
                 g_token.token,
                 (unsigned)g_token.expire_time_sec,
                 (unsigned)g_token.server_date_sec);
    if (n <= 0 || (size_t)n >= sizeof(json_buf)) {
        return;
    }

    (void)ktv_persist_write_file_atomic(KTV_TOKEN_PERSIST_PATH, json_buf, (size_t)n);
}

static void ktv_token_try_load_persisted(void)
{
    char buf[768];
    int n;
    cJSON *root;
    cJSON *token_item;
    cJSON *expire_item;
    cJSON *server_date_item;
    const char *token_str;
    uint32_t expire_sec;
    uint32_t server_date_sec;

    memset(buf, 0, sizeof(buf));
    n = ktv_persist_read_file(KTV_TOKEN_PERSIST_PATH, buf, sizeof(buf) - 1U);
    if (n <= 0) {
        return;
    }
    buf[sizeof(buf) - 1U] = '\0';

    root = cJSON_Parse(buf);
    if (root == NULL) {
        return;
    }

    token_item = cJSON_GetObjectItem(root, "token");
    expire_item = cJSON_GetObjectItem(root, "expire_time_sec");
    server_date_item = cJSON_GetObjectItem(root, "server_date_sec");
    token_str = (cJSON_IsString(token_item) && token_item->valuestring != NULL) ? token_item->valuestring : NULL;
    expire_sec = (uint32_t)ktv_json_u64(expire_item);
    server_date_sec = (uint32_t)ktv_json_u64(server_date_item);
    cJSON_Delete(root);

    if (token_str == NULL || token_str[0] == '\0') {
        return;
    }

    pthread_mutex_lock(&g_token.lock);
    snprintf(g_token.token, sizeof(g_token.token), "%s", token_str);
    g_token.expire_time_sec = expire_sec;
    g_token.server_date_sec = server_date_sec;
    g_token.initialized = 1;
    pthread_mutex_unlock(&g_token.lock);
}

void ktv_token_service_init(void)
{
    /* Ensure time service has a base before token expiry checks. */
    ktv_time_service_init();
    ktv_token_try_load_persisted();
}

static int ktv_token_is_expired_locked(void)
{
    uint32_t wall_now_sec;
    uint32_t now_sec;

    if (!g_token.initialized) {
        return 1;
    }
    if (g_token.expire_time_sec == 0U) {
        return 1;
    }

    /*
     * Reboot 后 time_service 的离线基准可能停留在上次缓存时刻，
     * 优先用系统实时时钟；只有当它明显不可信时才退回到 time_service。
     */
    wall_now_sec = ktv_token_wall_now_sec();
    now_sec = ktv_time_service_now_sec();

    if (wall_now_sec >= g_token.server_date_sec && wall_now_sec > now_sec) {
        now_sec = wall_now_sec;
    }

    if (now_sec == 0U) {
        /* 时间不可用时，保守处理：视为过期 */
        return 1;
    }

    return (now_sec >= g_token.expire_time_sec) ? 1 : 0;
}

static int ktv_token_parse_and_update_locked(const char *json_str)
{
    cJSON *root;
    cJSON *code;
    cJSON *data;
    cJSON *token_obj;
    cJSON *expire_date;
    cJSON *server_date;
    const char *token_str;
    uint32_t expire_sec;
    uint32_t server_date_sec;

    if (json_str == NULL) {
        return -1;
    }

    root = cJSON_Parse(json_str);
    if (root == NULL) {
        return -1;
    }

    code = cJSON_GetObjectItem(root, "code");
    if (code == NULL || code->valueint != 0) {
        cJSON_Delete(root);
        return -1;
    }

    data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        cJSON_Delete(root);
        return -1;
    }

    token_obj = cJSON_GetObjectItem(data, "token");
    expire_date = cJSON_GetObjectItem(data, "expir_date");
    server_date = cJSON_GetObjectItem(data, "server_date");
    if (server_date == NULL) {
        server_date = cJSON_GetObjectItem(root, "server_date");
    }

    token_str = (cJSON_IsString(token_obj) && token_obj->valuestring != NULL) ? token_obj->valuestring : NULL;
    expire_sec = (uint32_t)ktv_json_u64(expire_date);
    server_date_sec = (uint32_t)ktv_json_u64(server_date);
    cJSON_Delete(root);

    if (token_str == NULL || token_str[0] == '\0') {
        return -1;
    }

    snprintf(g_token.token, sizeof(g_token.token), "%s", token_str);
    g_token.expire_time_sec = expire_sec;
    g_token.server_date_sec = server_date_sec;
    g_token.initialized = 1;

    ktv_token_persist_snapshot_locked();
    return 0;
}

int ktv_token_service_refresh_now(void)
{
    int retry_count = 3;
    int retry_delay_ms = 500;
    uint64_t now_ms;

    /*
     * Offline/backoff guard:
     * If token fetch failed recently (likely WiFi down), skip repeated HTTP attempts.
     */
    now_ms = ktv_token_wall_now_ms();
    if (g_token_net.last_fail_ms != 0ULL &&
        now_ms != 0ULL &&
        (now_ms - g_token_net.last_fail_ms) < 30000ULL) {
        return -1;
    }

    for (int attempt = 0; attempt < retry_count; attempt++) {
        char response[1024];

        /* 每次尝试前同步时间（尽量确保过期判断可靠） */
        (void)ktv_time_service_sync_now();

        response[0] = '\0';
        if (ktv_http_get_to_memory(TOKEN_URL, response, sizeof(response), 10000) != 0) {
            pthread_mutex_lock(&g_token.lock);
            if (ktv_token_parse_and_update_locked(response) == 0) {
                pthread_mutex_unlock(&g_token.lock);
                if (g_token_net.offline) {
                    printf("[KTV_TOKEN] fetch recovered\n");
                }
                g_token_net.offline = 0;
                g_token_net.last_fail_ms = 0ULL;
                return 0;
            }
            pthread_mutex_unlock(&g_token.lock);
        }

        /*
         * Treat as offline failure: set backoff immediately to avoid a burst of retries
         * when WiFi is not connected.
         */
        g_token_net.last_fail_ms = (now_ms != 0ULL) ? now_ms : ktv_token_wall_now_ms();
        g_token_net.offline = 1;
        if (now_ms == 0ULL || (now_ms - g_token_net.last_log_ms) >= 30000ULL) {
            g_token_net.last_log_ms = (now_ms != 0ULL) ? now_ms : ktv_token_wall_now_ms();
            printf("[KTV_TOKEN] fetch failed (offline/backoff)\n");
        }
        break;
    }

    return -1;
}

const char *ktv_token_service_get(void)
{
    int need_refresh = 0;
    int refresh_ok;

    pthread_mutex_lock(&g_token.lock);
    if (!g_token.initialized) {
        need_refresh = 1;
    } else if (ktv_token_is_expired_locked()) {
        need_refresh = 1;
    }
    pthread_mutex_unlock(&g_token.lock);

    if (need_refresh) {
        refresh_ok = (ktv_token_service_refresh_now() == 0) ? 1 : 0;
        (void)refresh_ok;
    }

    pthread_mutex_lock(&g_token.lock);
    /* Never return NULL: callers often embed token into URL signing. */
    if (g_token.token[0] == '\0') {
        g_token.token[0] = '\0';
    }
    pthread_mutex_unlock(&g_token.lock);

    return g_token.token;
}

uint32_t ktv_token_service_expire_time_sec(void)
{
    uint32_t exp;
    pthread_mutex_lock(&g_token.lock);
    exp = g_token.expire_time_sec;
    pthread_mutex_unlock(&g_token.lock);
    return exp;
}

