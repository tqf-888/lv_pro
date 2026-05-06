/**
 * @file http_api.c
 * @brief HTTP客户端统一API实现
 */

#include "http_api.h"
#include "http_engine.h"
#include "http_pool.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/* ==================== 下载合并与 tmp 清理 ==================== */

typedef struct http_download_waiter
{
    http_callback_t callback;
    void *user_data;
    struct http_download_waiter *next;
} http_download_waiter_t;

typedef struct http_inflight_download
{
    char url[HTTP_MAX_URL_LEN];
    char local_path[HTTP_MAX_PATH_LEN];
    http_download_waiter_t *waiters;
    struct http_inflight_download *next;
} http_inflight_download_t;

/* 全局上下文 */
typedef struct {
    http_pool_t *pool;
    http_config_t config;
    int initialized;
    pthread_mutex_t lock;

    http_inflight_download_t *downloads;

    uint32_t submit_counter;
    time_t last_tmp_cleanup_ts;
} http_context_t;

static http_context_t g_ctx = {
    .pool = NULL,
    .initialized = 0,
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .downloads = NULL,
    .submit_counter = 0U,
    .last_tmp_cleanup_ts = 0
};

#define HTTP_TMP_CLEANUP_INTERVAL_SEC   10
#define HTTP_TMP_FILE_STALE_SEC         20
#define HTTP_TMP_SCAN_EVERY_N_SUBMITS   8

static void free_waiters(http_download_waiter_t *w)
{
    while (w) {
        http_download_waiter_t *next = w->next;
        free(w);
        w = next;
    }
}

static http_inflight_download_t *find_inflight_by_path(const char *local_path)
{
    http_inflight_download_t *cur = g_ctx.downloads;
    while (cur) {
        if (strncmp(cur->local_path, local_path, HTTP_MAX_PATH_LEN) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

static void remove_inflight_download(http_inflight_download_t *entry)
{
    http_inflight_download_t **pp = &g_ctx.downloads;
    while (*pp) {
        if (*pp == entry) {
            *pp = entry->next;
            return;
        }
        pp = &(*pp)->next;
    }
}

static void dispatch_download_waiters(http_task_t *task, http_inflight_download_t *entry)
{
    http_download_waiter_t *waiters = NULL;

    pthread_mutex_lock(&g_ctx.lock);
    remove_inflight_download(entry);
    waiters = entry->waiters;
    entry->waiters = NULL;
    pthread_mutex_unlock(&g_ctx.lock);

    while (waiters) {
        http_download_waiter_t *next = waiters->next;
        if (waiters->callback) {
            waiters->callback(task, waiters->user_data);
        }
        free(waiters);
        waiters = next;
    }

    free(entry);
}

static void download_complete_bridge(http_task_t *task, void *user_data)
{
    http_inflight_download_t *entry = (http_inflight_download_t *)user_data;
    if (!entry) {
        return;
    }

    dispatch_download_waiters(task, entry);
}

static int extract_dirpath(const char *path, char *dirbuf, size_t size)
{
    char *slash;
    if (!path || !dirbuf || size == 0) {
        return -1;
    }

    snprintf(dirbuf, size, "%s", path);
    slash = strrchr(dirbuf, '/');
    if (!slash || slash == dirbuf) {
        return -1;
    }
    *slash = '\0';
    return 0;
}

static int is_tmp_name(const char *name)
{
    return (name != NULL && strstr(name, ".tmp.") != NULL);
}

static int tmp_belongs_to_active_download(const char *fullpath)
{
    int active = 0;
    http_inflight_download_t *cur;

    pthread_mutex_lock(&g_ctx.lock);
    cur = g_ctx.downloads;
    while (cur) {
        size_t n = strlen(cur->local_path);
        if (strncmp(fullpath, cur->local_path, n) == 0 &&
            strncmp(fullpath + n, ".tmp.", 5) == 0) {
            active = 1;
            break;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&g_ctx.lock);

    return active;
}

static void cleanup_tmp_files_in_dir(const char *dirpath)
{
    DIR *dir;
    struct dirent *de;
    time_t now = time(NULL);

    if (!dirpath || dirpath[0] == '\0') {
        return;
    }

    dir = opendir(dirpath);
    if (!dir) {
        return;
    }

    while ((de = readdir(dir)) != NULL) {
        char fullpath[HTTP_MAX_PATH_LEN];
        struct stat st;

        if (de->d_name[0] == '.') {
            continue;
        }
        if (!is_tmp_name(de->d_name)) {
            continue;
        }

        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, de->d_name);

        if (tmp_belongs_to_active_download(fullpath)) {
            continue;
        }

        if (stat(fullpath, &st) != 0) {
            continue;
        }

        if ((now - st.st_mtime) < HTTP_TMP_FILE_STALE_SEC) {
            continue;
        }

        unlink(fullpath);
    }

    closedir(dir);
}

static void maybe_cleanup_tmp_for_path(const char *local_path)
{
    char dirpath[HTTP_MAX_PATH_LEN];
    time_t now = time(NULL);

    if (!local_path || local_path[0] == '\0') {
        return;
    }

    pthread_mutex_lock(&g_ctx.lock);
    g_ctx.submit_counter++;
    if ((g_ctx.submit_counter % HTTP_TMP_SCAN_EVERY_N_SUBMITS) != 0 &&
        (now - g_ctx.last_tmp_cleanup_ts) < HTTP_TMP_CLEANUP_INTERVAL_SEC) {
        pthread_mutex_unlock(&g_ctx.lock);
        return;
    }
    g_ctx.last_tmp_cleanup_ts = now;
    pthread_mutex_unlock(&g_ctx.lock);

    if (extract_dirpath(local_path, dirpath, sizeof(dirpath)) == 0) {
        cleanup_tmp_files_in_dir(dirpath);
    }
}

/* ==================== 公共接口 ==================== */

http_err_t http_init(const http_config_t *config)
{
    pthread_mutex_lock(&g_ctx.lock);

    if (g_ctx.initialized) {
        pthread_mutex_unlock(&g_ctx.lock);
        HTTP_DEBUG("Already initialized");
        return HTTP_ERR_ALREADY_INIT;
    }

    if (config) {
        memcpy(&g_ctx.config, config, sizeof(http_config_t));
    } else {
        http_config_default(&g_ctx.config);
    }

    /*
     * 正式工程修复：限制 worker 并发上限。
     * 该板端在图片大量并发下载时，libcurl/wolfSSL 稳定性明显下降。
     * 这里统一收敛到 4 路以内，避免瞬时并发把底层网络库打崩。
     */
    if (g_ctx.config.thread_num == 0 || g_ctx.config.thread_num > 4) {
        g_ctx.config.thread_num = 4;
    }

    http_err_t err = http_engine_init(&g_ctx.config);
    if (err != HTTP_OK) {
        pthread_mutex_unlock(&g_ctx.lock);
        return err;
    }

    g_ctx.pool = http_pool_create(g_ctx.config.thread_num, g_ctx.config.queue_size);
    if (!g_ctx.pool) {
        http_engine_deinit();
        pthread_mutex_unlock(&g_ctx.lock);
        return HTTP_ERR_UNKNOWN;
    }

    g_ctx.downloads = NULL;
    g_ctx.submit_counter = 0U;
    g_ctx.last_tmp_cleanup_ts = time(NULL);
    g_ctx.initialized = 1;

    pthread_mutex_unlock(&g_ctx.lock);

    HTTP_DEBUG("HTTP client initialized");
    return HTTP_OK;
}

void http_deinit(void)
{
    http_inflight_download_t *downloads = NULL;

    pthread_mutex_lock(&g_ctx.lock);

    if (!g_ctx.initialized) {
        pthread_mutex_unlock(&g_ctx.lock);
        return;
    }

    http_pool_t *pool = g_ctx.pool;
    g_ctx.pool = NULL;
    g_ctx.initialized = 0;

    downloads = g_ctx.downloads;
    g_ctx.downloads = NULL;

    pthread_mutex_unlock(&g_ctx.lock);

    if (pool) {
        http_pool_wait(pool);
        http_pool_destroy(pool);
    }

    while (downloads) {
        http_inflight_download_t *next = downloads->next;
        free_waiters(downloads->waiters);
        free(downloads);
        downloads = next;
    }

    http_engine_deinit();
    HTTP_DEBUG("HTTP client deinitialized");
}

http_err_t http_download(const char *url, const char *local_path,
                         http_callback_t callback, void *user_data)
{
    return http_download_priority(url, local_path, HTTP_PRIORITY_LOW, callback, user_data);
}

http_err_t http_download_priority(const char *url, const char *local_path,
                                  http_priority_t priority,
                                  http_callback_t callback, void *user_data)
{
    http_err_t err;
    http_inflight_download_t *entry;
    http_task_t task;

    if (!g_ctx.initialized) {
        HTTP_DEBUG("Not initialized");
        return HTTP_ERR_NOT_INIT;
    }

    if (!url || !local_path) {
        return HTTP_ERR_INVALID_PARAM;
    }

    maybe_cleanup_tmp_for_path(local_path);

    pthread_mutex_lock(&g_ctx.lock);

    entry = find_inflight_by_path(local_path);
    if (entry != NULL) {
        http_download_waiter_t *waiter =
            (http_download_waiter_t *)calloc(1, sizeof(http_download_waiter_t));
        if (!waiter) {
            pthread_mutex_unlock(&g_ctx.lock);
            return HTTP_ERR_MEMORY_ALLOC;
        }

        /*
         * 同一路径下载合并：
         * - URL 不同直接拒绝，避免脏覆盖
         * - priority 不重新提交（任务已经在跑），但 LOW 升 HIGH 我们也无能为力
         *   （curl 单次请求不可中断），保持原行为即可。
         */
        if (strncmp(entry->url, url, HTTP_MAX_URL_LEN) != 0) {
            free(waiter);
            pthread_mutex_unlock(&g_ctx.lock);
            return HTTP_ERR_INVALID_PARAM;
        }

        waiter->callback = callback;
        waiter->user_data = user_data;
        waiter->next = entry->waiters;
        entry->waiters = waiter;

        pthread_mutex_unlock(&g_ctx.lock);
        return HTTP_OK;
    }

    entry = (http_inflight_download_t *)calloc(1, sizeof(http_inflight_download_t));
    if (!entry) {
        pthread_mutex_unlock(&g_ctx.lock);
        return HTTP_ERR_MEMORY_ALLOC;
    }

    strncpy(entry->url, url, HTTP_MAX_URL_LEN - 1);
    strncpy(entry->local_path, local_path, HTTP_MAX_PATH_LEN - 1);

    if (callback) {
        http_download_waiter_t *owner =
            (http_download_waiter_t *)calloc(1, sizeof(http_download_waiter_t));
        if (!owner) {
            free(entry);
            pthread_mutex_unlock(&g_ctx.lock);
            return HTTP_ERR_MEMORY_ALLOC;
        }
        owner->callback = callback;
        owner->user_data = user_data;
        owner->next = NULL;
        entry->waiters = owner;
    }

    entry->next = g_ctx.downloads;
    g_ctx.downloads = entry;

    pthread_mutex_unlock(&g_ctx.lock);

    memset(&task, 0, sizeof(task));
    task.type = HTTP_TASK_DOWNLOAD_FILE;
    task.callback = download_complete_bridge;
    task.user_data = entry;
    task.timeout = g_ctx.config.timeout;
    task.priority = priority;
    strncpy(task.url, url, HTTP_MAX_URL_LEN - 1);
    strncpy(task.local_path, local_path, HTTP_MAX_PATH_LEN - 1);

    err = http_pool_submit(g_ctx.pool, &task);
    if (err != HTTP_OK) {
        pthread_mutex_lock(&g_ctx.lock);
        remove_inflight_download(entry);
        pthread_mutex_unlock(&g_ctx.lock);
        free_waiters(entry->waiters);
        free(entry);
        return err;
    }

    return HTTP_OK;
}

http_err_t http_fetch(const char *url, http_callback_t callback, void *user_data)
{
    return http_fetch_priority(url, HTTP_PRIORITY_LOW, callback, user_data);
}

http_err_t http_fetch_priority(const char *url,
                               http_priority_t priority,
                               http_callback_t callback,
                               void *user_data)
{
    if (!g_ctx.initialized) {
        HTTP_DEBUG("Not initialized");
        return HTTP_ERR_NOT_INIT;
    }

    if (!url) {
        return HTTP_ERR_INVALID_PARAM;
    }

    http_task_t task;
    memset(&task, 0, sizeof(task));
    task.type = HTTP_TASK_FETCH_MEMORY;
    task.callback = callback;
    task.user_data = user_data;
    task.timeout = g_ctx.config.timeout;
    task.priority = priority;

    strncpy(task.url, url, HTTP_MAX_URL_LEN - 1);

    return http_pool_submit(g_ctx.pool, &task);
}

void http_wait(void)
{
    if (!g_ctx.initialized || !g_ctx.pool) return;
    http_pool_wait(g_ctx.pool);
}

void http_get_stats(http_stats_t *stats)
{
    if (!stats) return;
    http_engine_get_stats(stats);
}

void http_print_stats(void)
{
    http_stats_t stats;
    http_pool_diag_t diag;
    const char *bottleneck = "smooth";

    http_get_stats(&stats);
    http_stats_print(&stats);

    if (g_ctx.pool != NULL) {
        memset(&diag, 0, sizeof(diag));
        http_pool_get_diag(g_ctx.pool, &diag);
        if (diag.overflow_count > 0U || diag.peak_overflow_count > 0U) {
            bottleneck = "mq/queue backlog";
        } else if (diag.active_threads >= g_ctx.config.thread_num && diag.pending_tasks > 0U) {
            bottleneck = "network workers saturated";
        } else if (diag.submit_wait_events > 0U || diag.mq_full_retry_events > 0U) {
            bottleneck = "submit pressure / mq full";
        }
        HTTP_DIAG("api snapshot active=%u pending=%u overflow=%u | peak_active=%u peak_pending=%u peak_overflow=%u | submitted=%llu completed=%llu wait=%llu overflow_push=%llu mq_retry=%llu | bottleneck=%s",
                  diag.active_threads,
                  diag.pending_tasks,
                  diag.overflow_count,
                  diag.peak_active_threads,
                  diag.peak_pending_tasks,
                  diag.peak_overflow_count,
                  (unsigned long long)diag.submitted_tasks,
                  (unsigned long long)diag.completed_tasks,
                  (unsigned long long)diag.submit_wait_events,
                  (unsigned long long)diag.overflow_push_events,
                  (unsigned long long)diag.mq_full_retry_events,
                  bottleneck);
    }
}