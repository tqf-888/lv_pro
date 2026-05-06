/**
 * @file http_common.h
 * @brief HTTP客户端公共定义
 */

#ifndef HTTP_COMMON_H
#define HTTP_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include "ktv.h"

/* ==================== 宏定义管理 ==================== */

/* 调试开关：详细逐条日志，默认关闭 */
#define HTTP_DEBUG_ENABLE       0

/* 压测诊断开关：默认开启，带限频，不会持续刷屏 */
#define HTTP_DIAG_ENABLE        0
#define HTTP_DIAG_INTERVAL_MS   2000U

/* TLS校验开关：当前设备上关闭校验才能稳定下载 */
#define HTTP_TLS_VERIFY         0
#define HTTP_TLS_CAINFO         "/etc/ssl/certs/ca-certificates.crt"

/* 并发配置
 * 注意：50张图“排队提交”可以，但默认不要真开50线程。
 * 对当前设备先用4，稳定后再试6/8。
 */
#define HTTP_MAX_THREADS        8
#define HTTP_DEFAULT_THREADS    6
#define HTTP_MAX_QUEUE_SIZE     2048

#define HTTP_MAX_URL_LEN        512
#define HTTP_MAX_PATH_LEN       256
#define HTTP_MAX_HEADER_LEN     1024
#define HTTP_TIMEOUT_DEFAULT    20
#define HTTP_CONNECT_TIMEOUT    5
#define HTTP_MAX_REDIRECTS      5

/* 内存配置 */
#define HTTP_MEM_POOL_SIZE      (1024 * 1024)
#define HTTP_CHUNK_SIZE         (8 * 1024)

/* 目录缓存 */
#define HTTP_DIR_CACHE_SLOTS    8

/* ==================== 错误码定义 ==================== */

typedef enum {
    HTTP_OK = 0,
    HTTP_ERR_INVALID_PARAM,
    HTTP_ERR_MEMORY_ALLOC,
    HTTP_ERR_QUEUE_FULL,
    HTTP_ERR_QUEUE_EMPTY,
    HTTP_ERR_CURL_INIT,
    HTTP_ERR_NETWORK,
    HTTP_ERR_TIMEOUT,
    HTTP_ERR_FILE_IO,
    HTTP_ERR_THREAD_CREATE,
    HTTP_ERR_ALREADY_INIT,
    HTTP_ERR_NOT_INIT,
    HTTP_ERR_UNKNOWN
} http_err_t;

/* ==================== 任务类型定义 ==================== */

typedef enum {
    HTTP_TASK_NONE = 0,
    HTTP_TASK_DOWNLOAD_FILE,
    HTTP_TASK_FETCH_MEMORY,
    HTTP_TASK_UPLOAD_FILE
} http_task_type_t;

/* ==================== 任务状态定义 ==================== */

typedef enum {
    HTTP_STATE_IDLE = 0,
    HTTP_STATE_QUEUED,
    HTTP_STATE_RUNNING,
    HTTP_STATE_COMPLETED,
    HTTP_STATE_FAILED,
    HTTP_STATE_CANCELLED
} http_state_t;

/* ==================== 任务优先级 ====================
 *
 * LOW（默认）：常规预拉/批量任务，按原 FIFO 顺序排队。
 * HIGH：用户当前可见页的下载，进 overflow 时插队到队首；mq 一有空槽就先于
 *       LOW 任务被消费。仍然不会打断已经在跑的 LOW worker（curl 单条请求
 *       不可中断），所以最坏延迟是"当前 4 路里最慢那一条跑完"。
 */
typedef enum {
    HTTP_PRIORITY_LOW  = 0,
    HTTP_PRIORITY_HIGH = 1
} http_priority_t;

/* ==================== 统计信息结构体 ==================== */

typedef struct {
    uint64_t total_tasks;
    uint64_t success_tasks;
    uint64_t failed_tasks;
    uint64_t total_bytes;
    uint32_t queue_size;
    uint32_t active_threads;
    pthread_mutex_t lock;
} http_stats_t;

/* ==================== 任务结构体 ==================== */

typedef struct http_task http_task_t;
typedef void (*http_callback_t)(http_task_t *task, void *user_data);

struct http_task {
    uint32_t task_id;
    http_task_type_t type;
    http_state_t state;

    char url[HTTP_MAX_URL_LEN];
    char local_path[HTTP_MAX_PATH_LEN];
    char header[HTTP_MAX_HEADER_LEN];
    uint32_t timeout;

    http_callback_t callback;
    void *user_data;

    http_err_t err_code;
    long http_status;
    char *response_data;
    size_t response_size;
    size_t download_size;
    size_t total_size;

    void *curl_handle;
    void *priv_data;

    /* HTTP_PRIORITY_HIGH 在 http_pool 进 overflow 时会插队到队首。
     * 缺省 0 = HTTP_PRIORITY_LOW，保持旧行为。 */
    http_priority_t priority;
};

/* ==================== 配置结构体 ==================== */

typedef struct {
    uint32_t thread_num;
    uint32_t queue_size;
    uint32_t timeout;
    uint32_t connect_timeout;
    uint32_t max_redirects;
    int debug_enable;
} http_config_t;

/* ==================== 调试打印宏 ==================== */

#include <stdio.h>

#if HTTP_DEBUG_ENABLE
#define HTTP_DEBUG(fmt, ...)                                                      \
    do {                                                                          \
        fflush(stderr);                                                           \
        fprintf(stderr, "\033[33m[HTTP_DEBUG] " fmt "\033[0m\n", ##__VA_ARGS__);    \
        fflush(stderr);                                                           \
    } while (0)
#else
#define HTTP_DEBUG(fmt, ...) do { } while (0)
#endif

#if HTTP_DIAG_ENABLE
#define HTTP_DIAG(fmt, ...)                                                       \
    do {                                                                          \
        fflush(stderr);                                                           \
        fprintf(stderr, "[HTTP_DIAG] " fmt "\n", ##__VA_ARGS__);                   \
        fflush(stderr);                                                           \
    } while (0)
#else
#define HTTP_DIAG(fmt, ...) do { } while (0)
#endif

/* ==================== 错误码转字符串 ==================== */

static inline const char* http_err_str(http_err_t err)
{
    switch (err) {
        case HTTP_OK:                return "OK";
        case HTTP_ERR_INVALID_PARAM: return "Invalid parameter";
        case HTTP_ERR_MEMORY_ALLOC:  return "Memory allocation failed";
        case HTTP_ERR_QUEUE_FULL:    return "Queue is full";
        case HTTP_ERR_QUEUE_EMPTY:   return "Queue is empty";
        case HTTP_ERR_CURL_INIT:     return "CURL initialization failed";
        case HTTP_ERR_NETWORK:       return "Network error";
        case HTTP_ERR_TIMEOUT:       return "Timeout";
        case HTTP_ERR_FILE_IO:       return "File I/O error";
        case HTTP_ERR_THREAD_CREATE: return "Thread creation failed";
        case HTTP_ERR_ALREADY_INIT:  return "Already initialized";
        case HTTP_ERR_NOT_INIT:      return "Not initialized";
        case HTTP_ERR_UNKNOWN:       return "Unknown error";
        default:                     return "Undefined error";
    }
}

/* ==================== 默认配置 ==================== */

static inline void http_config_default(http_config_t *config)
{
    if (!config) return;

    config->thread_num = HTTP_DEFAULT_THREADS;
    config->queue_size = HTTP_MAX_QUEUE_SIZE;
    config->timeout = HTTP_TIMEOUT_DEFAULT;
    config->connect_timeout = HTTP_CONNECT_TIMEOUT;
    config->max_redirects = HTTP_MAX_REDIRECTS;
    config->debug_enable = HTTP_DEBUG_ENABLE;
}

/* ==================== 统计信息函数 ==================== */

static inline void http_stats_init(http_stats_t *stats)
{
    if (!stats) return;

    stats->total_tasks = 0;
    stats->success_tasks = 0;
    stats->failed_tasks = 0;
    stats->total_bytes = 0;
    stats->queue_size = 0;
    stats->active_threads = 0;
    pthread_mutex_init(&stats->lock, NULL);
}

static inline void http_stats_update(http_stats_t *stats, int success)
{
    if (!stats) return;

    pthread_mutex_lock(&stats->lock);
    if (success) {
        stats->success_tasks++;
    } else {
        stats->failed_tasks++;
    }
    pthread_mutex_unlock(&stats->lock);
}

static inline void http_stats_print(http_stats_t *stats)
{
    if (!stats) return;

    pthread_mutex_lock(&stats->lock);
    HTTP_DEBUG("=== HTTP Statistics ===");
    HTTP_DEBUG("Total tasks:    %lu", (unsigned long)stats->total_tasks);
    HTTP_DEBUG("Success tasks:  %lu", (unsigned long)stats->success_tasks);
    HTTP_DEBUG("Failed tasks:   %lu", (unsigned long)stats->failed_tasks);
    HTTP_DEBUG("Total bytes:    %lu", (unsigned long)stats->total_bytes);
    HTTP_DEBUG("Queue size:     %u", stats->queue_size);
    HTTP_DEBUG("Active threads: %u", stats->active_threads);
    pthread_mutex_unlock(&stats->lock);
}

#endif /* HTTP_COMMON_H */