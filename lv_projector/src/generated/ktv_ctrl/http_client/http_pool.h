#ifndef HTTP_POOL_H
#define HTTP_POOL_H

#include "http_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct http_pool http_pool_t;

typedef struct {
    uint32_t active_threads;
    uint32_t pending_tasks;
    uint32_t overflow_count;
    uint32_t peak_active_threads;
    uint32_t peak_pending_tasks;
    uint32_t peak_overflow_count;
    uint64_t submitted_tasks;
    uint64_t completed_tasks;
    uint64_t submit_wait_events;
    uint64_t overflow_push_events;
    uint64_t mq_full_retry_events;
} http_pool_diag_t;

/*
 * 说明：
 * - HTTP_TASK_FETCH_MEMORY 的 response_data 只在 callback 期间有效
 * - callback 返回后，线程池自动释放 response_data
 */

http_pool_t *http_pool_create(uint32_t thread_num, uint32_t queue_size);
void http_pool_destroy(http_pool_t *pool);

http_err_t http_pool_submit(http_pool_t *pool, http_task_t *task);
void http_pool_wait(http_pool_t *pool);

uint32_t http_pool_get_active(http_pool_t *pool);
uint32_t http_pool_get_pending(http_pool_t *pool);
void http_pool_get_diag(http_pool_t *pool, http_pool_diag_t *diag);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_POOL_H */