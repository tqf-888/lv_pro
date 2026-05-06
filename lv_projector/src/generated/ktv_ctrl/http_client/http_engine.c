#include "http_engine.h"

#include <curl/curl.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    http_config_t config;
    http_stats_t stats;
    pthread_mutex_t lock;
    int initialized;
} http_engine_t;

typedef struct {
    char *memory;
    size_t size;
    size_t capacity;
} mem_chunk_t;

typedef struct {
    FILE *fp;
    size_t downloaded_size;
    int write_error;
    http_task_t *task;
} file_ctx_t;

typedef struct {
    char dir[HTTP_MAX_PATH_LEN];
    int valid;
} dir_cache_entry_t;

static http_engine_t g_engine = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .initialized = 0,
};

static dir_cache_entry_t g_dir_cache[8];
static pthread_mutex_t g_dir_cache_lock = PTHREAD_MUTEX_INITIALIZER;

/* 每个请求独立 CURL easy handle。
 * 原先线程私有 handle + reset 在当前设备的 libcurl/wolfSSL 组合下，
 * 高并发/高压失败场景容易触发不稳定。
 */

/* ==================== 目录辅助 ==================== */

static int create_directory(const char *path, mode_t mode)
{
    if (!path || !path[0]) {
        return -1;
    }

    char tmp[HTTP_MAX_PATH_LEN];
    snprintf(tmp, sizeof(tmp), "%s", path);

    size_t len = strlen(tmp);
    if (len > 0 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    for (char *p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }

    if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
        return -1;
    }

    return 0;
}

static int extract_directory(const char *filepath, char *dirpath, size_t size)
{
    if (!filepath || !dirpath || size == 0) {
        return -1;
    }

    snprintf(dirpath, size, "%s", filepath);
    char *slash = strrchr(dirpath, '/');
    if (!slash || slash == dirpath) {
        return -1;
    }

    *slash = '\0';
    return 0;
}

static int dir_cache_contains(const char *dirpath)
{
    int found = 0;
    pthread_mutex_lock(&g_dir_cache_lock);
    for (int i = 0; i < 8; ++i) {
        if (g_dir_cache[i].valid &&
            strncmp(g_dir_cache[i].dir, dirpath, HTTP_MAX_PATH_LEN) == 0) {
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&g_dir_cache_lock);
    return found;
}

static void dir_cache_add(const char *dirpath)
{
    pthread_mutex_lock(&g_dir_cache_lock);

    for (int i = 0; i < 8; ++i) {
        if (g_dir_cache[i].valid &&
            strncmp(g_dir_cache[i].dir, dirpath, HTTP_MAX_PATH_LEN) == 0) {
            pthread_mutex_unlock(&g_dir_cache_lock);
            return;
        }
    }

    for (int i = 0; i < 8; ++i) {
        if (!g_dir_cache[i].valid) {
            snprintf(g_dir_cache[i].dir, sizeof(g_dir_cache[i].dir), "%s", dirpath);
            g_dir_cache[i].valid = 1;
            pthread_mutex_unlock(&g_dir_cache_lock);
            return;
        }
    }

    snprintf(g_dir_cache[0].dir, sizeof(g_dir_cache[0].dir), "%s", dirpath);
    g_dir_cache[0].valid = 1;

    pthread_mutex_unlock(&g_dir_cache_lock);
}

static http_err_t ensure_directory_exists(const char *filepath)
{
    char dirpath[HTTP_MAX_PATH_LEN];

    if (extract_directory(filepath, dirpath, sizeof(dirpath)) != 0) {
        return HTTP_ERR_INVALID_PARAM;
    }

    if (dir_cache_contains(dirpath)) {
        return HTTP_OK;
    }

    struct stat st;
    if (stat(dirpath, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            return HTTP_ERR_FILE_IO;
        }
        dir_cache_add(dirpath);
        return HTTP_OK;
    }

    if (create_directory(dirpath, 0755) != 0) {
        return HTTP_ERR_FILE_IO;
    }

    dir_cache_add(dirpath);
    return HTTP_OK;
}

/* ==================== URL / PATH 校验 ==================== */

static int is_valid_url_scheme(const char *url)
{
    if (!url || strlen(url) < 8) {
        return 0;
    }

    return (strncmp(url, "http://", 7) == 0 ||
            strncmp(url, "https://", 8) == 0);
}

/*
 * 通用 URL 基础校验：
 * - 必须是 http:// 或 https://
 * - 必须存在 host
 */
static int is_valid_url(const char *url)
{
    const char *p;
    const char *host_begin;
    const char *host_end;

    if (!is_valid_url_scheme(url)) {
        return 0;
    }

    p = strstr(url, "://");
    if (p == NULL) {
        return 0;
    }

    host_begin = p + 3;
    if (*host_begin == '\0') {
        return 0;
    }

    host_end = strpbrk(host_begin, "/?#");
    if (host_end == NULL) {
        host_end = url + strlen(url);
    }

    if (host_end <= host_begin) {
        return 0;
    }

    return 1;
}

/*
 * 下载任务的严格校验：
 * - 必须有资源路径
 * - 不能只是纯域名
 * 例如：
 *   https://songsheetpic-cdn.jzurl.cn          -> 非法
 *   https://songsheetpic-cdn.jzurl.cn/         -> 非法
 *   https://songsheetpic-cdn.jzurl.cn/a.png    -> 合法
 */
static int is_valid_download_url(const char *url)
{
    const char *p;
    const char *path_begin;

    if (!is_valid_url(url)) {
        return 0;
    }

    p = strstr(url, "://");
    if (p == NULL) {
        return 0;
    }

    path_begin = strchr(p + 3, '/');
    if (path_begin == NULL) {
        return 0;
    }

    /* 只有 "/" 也算无效 */
    if (path_begin[1] == '\0') {
        return 0;
    }

    return 1;
}

static int is_valid_path(const char *path)
{
    if (!path || path[0] != '/') {
        return 0;
    }
    if (strlen(path) >= HTTP_MAX_PATH_LEN) {
        return 0;
    }
    if (strstr(path, "..") != NULL) {
        return 0;
    }
    if (strstr(path, "//") != NULL) {
        return 0;
    }
    return 1;
}

/* ==================== CURL 回调 ==================== */

static size_t write_memory_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    mem_chunk_t *mem = (mem_chunk_t *)userp;

    size_t need = mem->size + realsize + 1;
    if (need > mem->capacity) {
        size_t new_cap = mem->capacity ? mem->capacity * 2 : HTTP_CHUNK_SIZE;
        while (new_cap < need) {
            new_cap *= 2;
        }

        char *p = (char *)realloc(mem->memory, new_cap);
        if (!p) {
            return 0;
        }

        mem->memory = p;
        mem->capacity = new_cap;
    }

    memcpy(mem->memory + mem->size, contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';
    return realsize;
}

static size_t write_file_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    file_ctx_t *ctx = (file_ctx_t *)userp;
    if (!ctx || !ctx->fp) {
        return 0;
    }

    size_t written = fwrite(contents, size, nmemb, ctx->fp);
    size_t bytes = written * size;

    ctx->downloaded_size += bytes;
    if (ctx->task) {
        ctx->task->download_size = ctx->downloaded_size;
    }

    if (written != nmemb) {
        ctx->write_error = 1;
        return 0;
    }

    return bytes;
}

static int progress_cb(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                       curl_off_t ultotal, curl_off_t ulnow)
{
    (void)dlnow;
    (void)ultotal;
    (void)ulnow;

    http_task_t *task = (http_task_t *)clientp;
    if (task && task->state == HTTP_STATE_CANCELLED) {
        return 1;
    }
    if (task) {
        task->total_size = (dltotal > 0) ? (size_t)dltotal : 0;
    }
    return 0;
}

/* ==================== 统计 ==================== */

static void stats_task_success(size_t bytes)
{
    pthread_mutex_lock(&g_engine.lock);
    g_engine.stats.success_tasks++;
    g_engine.stats.total_bytes += bytes;
    pthread_mutex_unlock(&g_engine.lock);
}

static void stats_task_failed(void)
{
    pthread_mutex_lock(&g_engine.lock);
    g_engine.stats.failed_tasks++;
    pthread_mutex_unlock(&g_engine.lock);
}

/* ==================== 公共接口 ==================== */

http_err_t http_engine_init(const http_config_t *config)
{
    pthread_mutex_lock(&g_engine.lock);

    if (g_engine.initialized) {
        pthread_mutex_unlock(&g_engine.lock);
        return HTTP_ERR_ALREADY_INIT;
    }

    if (config) {
        memcpy(&g_engine.config, config, sizeof(g_engine.config));
    } else {
        http_config_default(&g_engine.config);
    }

    CURLcode rc = curl_global_init(CURL_GLOBAL_ALL);
    if (rc != CURLE_OK) {
        pthread_mutex_unlock(&g_engine.lock);
        return HTTP_ERR_CURL_INIT;
    }

    http_stats_init(&g_engine.stats);
    memset(g_dir_cache, 0, sizeof(g_dir_cache));
    g_engine.initialized = 1;

    pthread_mutex_unlock(&g_engine.lock);
    return HTTP_OK;
}

void http_engine_deinit(void)
{
    pthread_mutex_lock(&g_engine.lock);

    if (!g_engine.initialized) {
        pthread_mutex_unlock(&g_engine.lock);
        return;
    }

    http_stats_print(&g_engine.stats);
    curl_global_cleanup();
    g_engine.initialized = 0;

    pthread_mutex_unlock(&g_engine.lock);
}

http_err_t http_engine_execute(http_task_t *task)
{
    if (!task) {
        return HTTP_ERR_INVALID_PARAM;
    }

    if (!g_engine.initialized) {
        task->err_code = HTTP_ERR_NOT_INIT;
        return HTTP_ERR_NOT_INIT;
    }

    /* 基础 URL 校验 */
    if (!is_valid_url(task->url)) {
        task->state = HTTP_STATE_FAILED;
        task->err_code = HTTP_ERR_INVALID_PARAM;
        stats_task_failed();
        return HTTP_ERR_INVALID_PARAM;
    }

    /* 下载任务额外做严格 URL 校验 */
    if (task->type == HTTP_TASK_DOWNLOAD_FILE && !is_valid_download_url(task->url)) {
        fprintf(stderr, "[HTTP_ENGINE] invalid download url: %s\n", task->url);
        task->state = HTTP_STATE_FAILED;
        task->err_code = HTTP_ERR_INVALID_PARAM;
        stats_task_failed();
        return HTTP_ERR_INVALID_PARAM;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        task->err_code = HTTP_ERR_CURL_INIT;
        return HTTP_ERR_CURL_INIT;
    }

    task->curl_handle = curl;
    task->state = HTTP_STATE_RUNNING;
    task->err_code = HTTP_OK;
    task->http_status = 0;
    task->download_size = 0;
    task->total_size = 0;
    task->response_data = NULL;
    task->response_size = 0;

    curl_easy_setopt(curl, CURLOPT_URL, task->url);
    /* 设备侧要求固定走 HTTP/1.1，避免协商分支和协议升级带来的额外不稳定性。 */
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

    /* 当前设备环境，保守处理。 */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    /* wolfSSL 高压场景下关闭 session cache，避免会话复用相关不稳定。 */
    curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);
    /* 每个请求独立连接，避免连接/句柄复用引发的并发踩踏。 */
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT,
                     task->timeout ? task->timeout : g_engine.config.timeout);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, g_engine.config.connect_timeout);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, g_engine.config.max_redirects);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_cb);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, task);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "EmbeddedHTTPClient/2.0");
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    CURLcode res = CURLE_OK;
    http_err_t err = HTTP_OK;

    if (task->type == HTTP_TASK_DOWNLOAD_FILE) {
        if (!is_valid_path(task->local_path)) {
            task->state = HTTP_STATE_FAILED;
            task->err_code = HTTP_ERR_INVALID_PARAM;
            stats_task_failed();
            err = HTTP_ERR_INVALID_PARAM;
            goto cleanup;
        }

        err = ensure_directory_exists(task->local_path);
        if (err != HTTP_OK) {
            task->state = HTTP_STATE_FAILED;
            task->err_code = err;
            stats_task_failed();
            goto cleanup;
        }

        char tmp_path[HTTP_MAX_PATH_LEN];
        unsigned long tid = (unsigned long)pthread_self();
        snprintf(tmp_path,
                 sizeof(tmp_path),
                 "%s.tmp.%u.%lu",
                 task->local_path,
                 task->task_id,
                 tid);

        FILE *fp = fopen(tmp_path, "wb");
        if (!fp) {
            task->state = HTTP_STATE_FAILED;
            task->err_code = HTTP_ERR_FILE_IO;
            stats_task_failed();
            err = HTTP_ERR_FILE_IO;
            goto cleanup;
        }

        file_ctx_t ctx;
        memset(&ctx, 0, sizeof(ctx));
        ctx.fp = fp;
        ctx.task = task;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);

        res = curl_easy_perform(curl);

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        task->http_status = http_code;

        if (fflush(fp) != 0) {
            ctx.write_error = 1;
        }
        if (fclose(fp) != 0) {
            ctx.write_error = 1;
        }

        if (res == CURLE_OK && !ctx.write_error) {
            if (rename(tmp_path, task->local_path) != 0) {
                unlink(tmp_path);
                task->state = HTTP_STATE_FAILED;
                task->err_code = HTTP_ERR_FILE_IO;
                stats_task_failed();
                err = HTTP_ERR_FILE_IO;
                goto cleanup;
            }

            task->download_size = ctx.downloaded_size;
            task->state = HTTP_STATE_COMPLETED;
            task->err_code = HTTP_OK;
            stats_task_success(ctx.downloaded_size);
        } else {
            unlink(tmp_path);
            task->state = HTTP_STATE_FAILED;
            task->err_code =
                (res == CURLE_OPERATION_TIMEDOUT) ? HTTP_ERR_TIMEOUT : HTTP_ERR_NETWORK;
            if (ctx.write_error) {
                task->err_code = HTTP_ERR_FILE_IO;
            }
            stats_task_failed();
        }

        err = task->err_code;
        goto cleanup;
    }

    if (task->type == HTTP_TASK_FETCH_MEMORY) {
        mem_chunk_t chunk;
        memset(&chunk, 0, sizeof(chunk));

        chunk.memory = (char *)malloc(HTTP_CHUNK_SIZE);
        if (!chunk.memory) {
            task->state = HTTP_STATE_FAILED;
            task->err_code = HTTP_ERR_MEMORY_ALLOC;
            stats_task_failed();
            err = HTTP_ERR_MEMORY_ALLOC;
            goto cleanup;
        }
        chunk.capacity = HTTP_CHUNK_SIZE;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

        res = curl_easy_perform(curl);

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        task->http_status = http_code;

        if (res == CURLE_OK) {
            task->response_data = chunk.memory;
            task->response_size = chunk.size;
            task->state = HTTP_STATE_COMPLETED;
            task->err_code = HTTP_OK;
            stats_task_success(chunk.size);
        } else {
            free(chunk.memory);
            task->response_data = NULL;
            task->response_size = 0;
            task->state = HTTP_STATE_FAILED;
            task->err_code =
                (res == CURLE_OPERATION_TIMEDOUT) ? HTTP_ERR_TIMEOUT : HTTP_ERR_NETWORK;
            stats_task_failed();
        }

        err = task->err_code;
        goto cleanup;
    }

    task->state = HTTP_STATE_FAILED;
    task->err_code = HTTP_ERR_INVALID_PARAM;
    stats_task_failed();
    err = HTTP_ERR_INVALID_PARAM;

cleanup:
    if (curl != NULL) {
        curl_easy_cleanup(curl);
    }
    task->curl_handle = NULL;
    return err;
}

void http_engine_cancel(http_task_t *task)
{
    if (!task) {
        return;
    }
    task->state = HTTP_STATE_CANCELLED;
}

void http_engine_get_stats(http_stats_t *stats)
{
    if (!stats) {
        return;
    }

    /*
     * 不能直接 memcpy 整个 http_stats_t：
     * - 里面含有 pthread_mutex_t
     * - 复制 mutex 再去加锁/解锁属于未定义行为，容易随机崩或卡死
     * 这里改成只拷贝数值字段，并给输出副本单独初始化一把锁。
     */
    pthread_mutex_lock(&g_engine.lock);
    stats->total_tasks = g_engine.stats.total_tasks;
    stats->success_tasks = g_engine.stats.success_tasks;
    stats->failed_tasks = g_engine.stats.failed_tasks;
    stats->total_bytes = g_engine.stats.total_bytes;
    stats->queue_size = g_engine.stats.queue_size;
    stats->active_threads = g_engine.stats.active_threads;
    pthread_mutex_unlock(&g_engine.lock);

    pthread_mutex_init(&stats->lock, NULL);
}