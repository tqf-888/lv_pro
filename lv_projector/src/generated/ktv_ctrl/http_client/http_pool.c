#include "http_pool.h"
#include "http_engine.h"
#include "http_mq.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

typedef struct http_pool_task_node
{
    http_task_t task;
    struct http_pool_task_node *next;
} http_pool_task_node_t;

struct http_pool {
    pthread_t *threads;
    uint32_t thread_num;
    http_mq_t *mq;

    volatile int running;

    pthread_mutex_t lock;
    pthread_cond_t idle_cond;

    uint32_t active_threads;
    uint32_t pending_tasks;
    uint32_t soft_limit;

    http_pool_task_node_t *overflow_head;
    http_pool_task_node_t *overflow_tail;
    uint32_t overflow_count;

    uint32_t peak_active_threads;
    uint32_t peak_pending_tasks;
    uint32_t peak_overflow_count;
    uint64_t submitted_tasks;
    uint64_t completed_tasks;
    uint64_t submit_wait_events;
    uint64_t overflow_push_events;
    uint64_t mq_full_retry_events;
    uint64_t diag_last_print_ms;
};


static uint64_t http_pool_now_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((uint64_t)tv.tv_sec * 1000ULL) + ((uint64_t)tv.tv_usec / 1000ULL);
}

static void http_pool_diag_maybe_print_locked(http_pool_t *pool, const char *reason, int force)
{
#if HTTP_DIAG_ENABLE
    uint64_t now_ms;
    const char *bottleneck = "smooth";

    if (pool == NULL) {
        return;
    }

    now_ms = http_pool_now_ms();
    if (!force && (now_ms - pool->diag_last_print_ms) < HTTP_DIAG_INTERVAL_MS) {
        return;
    }
    pool->diag_last_print_ms = now_ms;

    if (pool->overflow_count > 0U) {
        bottleneck = "mq/queue backlog";
    } else if (pool->active_threads >= pool->thread_num && pool->pending_tasks > 0U) {
        bottleneck = "network workers saturated";
    } else if (pool->pending_tasks > (pool->soft_limit / 2U)) {
        bottleneck = "submit pressure high";
    }

    HTTP_DIAG("pool[%s] active=%u/%u pending=%u overflow=%u | peak_active=%u peak_pending=%u peak_overflow=%u | submitted=%llu completed=%llu wait=%llu overflow_push=%llu mq_retry=%llu | bottleneck=%s",
              reason,
              pool->active_threads,
              pool->thread_num,
              pool->pending_tasks,
              pool->overflow_count,
              pool->peak_active_threads,
              pool->peak_pending_tasks,
              pool->peak_overflow_count,
              (unsigned long long)pool->submitted_tasks,
              (unsigned long long)pool->completed_tasks,
              (unsigned long long)pool->submit_wait_events,
              (unsigned long long)pool->overflow_push_events,
              (unsigned long long)pool->mq_full_retry_events,
              bottleneck);
#else
    (void)pool;
    (void)reason;
    (void)force;
#endif
}

static http_pool_task_node_t *http_pool_overflow_pop_locked(http_pool_t *pool)
{
    http_pool_task_node_t *node = pool->overflow_head;
    if (node != NULL) {
        pool->overflow_head = node->next;
        if (pool->overflow_head == NULL) {
            pool->overflow_tail = NULL;
        }
        if (pool->overflow_count > 0U) {
            pool->overflow_count--;
        }
        node->next = NULL;
    }
    return node;
}

static void http_pool_overflow_push_locked(http_pool_t *pool, http_pool_task_node_t *node)
{
    node->next = NULL;
    if (pool->overflow_tail != NULL) {
        pool->overflow_tail->next = node;
    } else {
        pool->overflow_head = node;
    }
    pool->overflow_tail = node;
    pool->overflow_count++;
    pool->overflow_push_events++;
    if (pool->overflow_count > pool->peak_overflow_count) {
        pool->peak_overflow_count = pool->overflow_count;
    }
}

/*
 * 插队到 overflow 队首（HIGH 优先级专用）。
 *
 * 工作流：worker 跑完一条任务 → http_pool_try_flush_overflow_locked() 把
 * overflow 头部的 node 推回 mq → mq 喂给空闲 worker。所以"塞到 overflow
 * head"= 下一个 mq 槽位让出来时第一个进 mq 的就是它。
 *
 * 这并不会打断 worker 正在跑的 LOW 任务（curl 单次请求不可中断），但能保证
 * "排队中"的 LOW 全部要让位给后面来的 HIGH。在 4 worker × ~几百 ms / 张的节奏下，
 * HIGH 的等待上限大约是 1 张 LOW 的剩余下载时间。
 */
static void http_pool_overflow_push_front_locked(http_pool_t *pool, http_pool_task_node_t *node)
{
    node->next = pool->overflow_head;
    pool->overflow_head = node;
    if (pool->overflow_tail == NULL) {
        pool->overflow_tail = node;
    }
    pool->overflow_count++;
    pool->overflow_push_events++;
    if (pool->overflow_count > pool->peak_overflow_count) {
        pool->peak_overflow_count = pool->overflow_count;
    }
}

static inline void http_pool_overflow_push_by_priority_locked(http_pool_t *pool,
                                                              http_pool_task_node_t *node,
                                                              http_priority_t priority)
{
    if (priority == HTTP_PRIORITY_HIGH) {
        http_pool_overflow_push_front_locked(pool, node);
    } else {
        http_pool_overflow_push_locked(pool, node);
    }
}

static http_err_t http_pool_try_flush_overflow_locked(http_pool_t *pool)
{
    while (pool->overflow_head != NULL && pool->running) {
        http_pool_task_node_t *node = pool->overflow_head;
        http_err_t err = http_mq_send(pool->mq, &node->task, sizeof(node->task));
        if (err == HTTP_OK) {
            (void)http_pool_overflow_pop_locked(pool);
            free(node);
            continue;
        }
        if (err == HTTP_ERR_QUEUE_FULL) {
            return HTTP_ERR_QUEUE_FULL;
        }
        return err;
    }
    return HTTP_OK;
}

static http_err_t http_pool_send_with_retry(http_pool_t *pool, http_task_t *task)
{
    http_err_t err = HTTP_ERR_UNKNOWN;
    unsigned waited_ms = 0U;

    while (pool->running) {
        err = http_mq_send(pool->mq, task, sizeof(http_task_t));
        if (err == HTTP_OK) {
            return HTTP_OK;
        }
        if (err != HTTP_ERR_QUEUE_FULL) {
            return err;
        }

        usleep(20 * 1000);
        waited_ms += 20U;
        pool->mq_full_retry_events++;
        if (waited_ms >= 1000U) {
            return HTTP_ERR_QUEUE_FULL;
        }
    }

    return HTTP_ERR_NOT_INIT;
}

static void *worker_thread(void *arg)
{
    http_pool_t *pool = (http_pool_t *)arg;
    http_task_t task;

    while (1) {
        memset(&task, 0, sizeof(task));
        if (http_mq_recv(pool->mq, &task, sizeof(task)) != HTTP_OK) {
            if (!pool->running) {
                break;
            }
            continue;
        }

        if (!pool->running &&
            task.type == 0 &&
            task.callback == NULL &&
            task.url[0] == '\0') {
            break;
        }

        pthread_mutex_lock(&pool->lock);
        pool->active_threads++;
        if (pool->active_threads > pool->peak_active_threads) {
            pool->peak_active_threads = pool->active_threads;
        }
        if (pool->pending_tasks > 0U) {
            pool->pending_tasks--;
        }
        pthread_mutex_unlock(&pool->lock);

        (void)http_engine_execute(&task);

        if (task.callback) {
            task.callback(&task, task.user_data);
        }

        if (task.type == HTTP_TASK_FETCH_MEMORY && task.response_data) {
            free(task.response_data);
            task.response_data = NULL;
            task.response_size = 0U;
        }

        pthread_mutex_lock(&pool->lock);
        pool->completed_tasks++;
        if (pool->active_threads > 0U) {
            pool->active_threads--;
        }
        (void)http_pool_try_flush_overflow_locked(pool);
        http_pool_diag_maybe_print_locked(pool, "worker_done", 0);
        pthread_cond_broadcast(&pool->idle_cond);
        if (pool->active_threads == 0U && pool->pending_tasks == 0U && pool->overflow_count == 0U) {
            pthread_cond_broadcast(&pool->idle_cond);
        }
        pthread_mutex_unlock(&pool->lock);
    }

    return NULL;
}

http_pool_t *http_pool_create(uint32_t thread_num, uint32_t queue_size)
{
    char mq_name[64];
    uint32_t try_queue;

    if (thread_num == 0U || thread_num > HTTP_MAX_THREADS) {
        return NULL;
    }
    if (queue_size == 0U || queue_size > HTTP_MAX_QUEUE_SIZE) {
        return NULL;
    }

    http_pool_t *pool = (http_pool_t *)calloc(1, sizeof(http_pool_t));
    if (!pool) {
        return NULL;
    }

    pool->thread_num = thread_num;
    pool->running = 1;
    pool->soft_limit = queue_size;
    pool->diag_last_print_ms = 0ULL;
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->idle_cond, NULL);

    snprintf(mq_name, sizeof(mq_name), "/http_pool_%ld_%ld",
             (long)getpid(), (long)time(NULL));

    pool->mq = NULL;
    try_queue = queue_size;
    /*
     * Compatibility fallback:
     * Some embedded kernels have very small mqueue msg_max (e.g. 10).
     * Keep shrinking queue depth until 1 so init can still succeed.
     */
    while (try_queue >= 1U) {
        pool->mq = http_mq_create(mq_name, try_queue, sizeof(http_task_t));
        if (pool->mq != NULL) {
            if (try_queue != queue_size) {
                printf("[HTTP_POOL] mq depth downgraded: req=%u actual=%u\n",
                       (unsigned)queue_size,
                       (unsigned)try_queue);
            }
            break;
        }
        if (try_queue == 1U) {
            break;
        }
        try_queue /= 2U;
    }
    if (!pool->mq) {
        printf("[HTTP_POOL] mq create failed: req_depth=%u\n", (unsigned)queue_size);
        pthread_cond_destroy(&pool->idle_cond);
        pthread_mutex_destroy(&pool->lock);
        free(pool);
        return NULL;
    }

    pool->threads = (pthread_t *)calloc(thread_num, sizeof(pthread_t));
    if (!pool->threads) {
        http_mq_destroy(pool->mq);
        pthread_cond_destroy(&pool->idle_cond);
        pthread_mutex_destroy(&pool->lock);
        free(pool);
        return NULL;
    }

    for (uint32_t i = 0U; i < thread_num; ++i) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            pool->running = 0;
            for (uint32_t j = 0U; j < i; ++j) {
                pthread_join(pool->threads[j], NULL);
            }
            free(pool->threads);
            http_mq_destroy(pool->mq);
            pthread_cond_destroy(&pool->idle_cond);
            pthread_mutex_destroy(&pool->lock);
            free(pool);
            return NULL;
        }
    }

    return pool;
}

void http_pool_destroy(http_pool_t *pool)
{
    if (!pool) {
        return;
    }

    pool->running = 0;

    http_task_t dummy;
    memset(&dummy, 0, sizeof(dummy));
    for (uint32_t i = 0U; i < pool->thread_num; ++i) {
        while (http_mq_send(pool->mq, &dummy, sizeof(dummy)) == HTTP_ERR_QUEUE_FULL) {
            usleep(10 * 1000);
        }
    }

    for (uint32_t i = 0U; i < pool->thread_num; ++i) {
        pthread_join(pool->threads[i], NULL);
    }

    while (pool->overflow_head != NULL) {
        http_pool_task_node_t *node = http_pool_overflow_pop_locked(pool);
        free(node);
    }

    http_mq_destroy(pool->mq);
    free(pool->threads);
    pthread_cond_destroy(&pool->idle_cond);
    pthread_mutex_destroy(&pool->lock);
    free(pool);
}

http_err_t http_pool_submit(http_pool_t *pool, http_task_t *task)
{
    static uint32_t s_task_id = 0U;

    if (!pool || !task) {
        return HTTP_ERR_INVALID_PARAM;
    }
    if (!pool->running) {
        return HTTP_ERR_NOT_INIT;
    }

    task->task_id = __sync_add_and_fetch(&s_task_id, 1U);
    task->state = HTTP_STATE_QUEUED;

    pthread_mutex_lock(&pool->lock);
    pool->submitted_tasks++;
    if (pool->pending_tasks + pool->overflow_count >= pool->soft_limit) {
        pool->submit_wait_events++;
        http_pool_diag_maybe_print_locked(pool, "submit_wait", 1);
        while (pool->running &&
               pool->pending_tasks + pool->overflow_count >= pool->soft_limit) {
            pthread_cond_wait(&pool->idle_cond, &pool->lock);
        }
    }

    if (!pool->running) {
        pthread_mutex_unlock(&pool->lock);
        return HTTP_ERR_NOT_INIT;
    }

    if (http_pool_try_flush_overflow_locked(pool) == HTTP_ERR_QUEUE_FULL) {
        http_pool_task_node_t *node = (http_pool_task_node_t *)calloc(1U, sizeof(*node));
        if (node == NULL) {
            pthread_mutex_unlock(&pool->lock);
            return HTTP_ERR_MEMORY_ALLOC;
        }
        /*
         * COPY SEMANTICS (IMPORTANT):
         * - We copy http_task_t struct bytes into overflow node (struct deep copy).
         * - Pointer fields inside http_task_t (callback/user_data/response_data/priv_data)
         *   are pointer-value copies only; their pointees are NOT duplicated.
         * - Producers must keep pointed data valid until task completion callback returns.
         */
        memcpy(&node->task, task, sizeof(*task));
        /* HIGH 优先级直接插队到 overflow 头，下次 worker 完成任务时第一个被
         * flush 进 mq；LOW 走原本的尾部追加。 */
        http_pool_overflow_push_by_priority_locked(pool, node, task->priority);
        pool->pending_tasks++;
        if (pool->pending_tasks > pool->peak_pending_tasks) {
            pool->peak_pending_tasks = pool->pending_tasks;
        }
        http_pool_diag_maybe_print_locked(pool, "submit_overflow_preflush", 0);
        pthread_mutex_unlock(&pool->lock);
        return HTTP_OK;
    }
    pthread_mutex_unlock(&pool->lock);

    http_err_t err = http_pool_send_with_retry(pool, task);
    if (err == HTTP_OK) {
        pthread_mutex_lock(&pool->lock);
        pool->pending_tasks++;
        if (pool->pending_tasks > pool->peak_pending_tasks) {
            pool->peak_pending_tasks = pool->pending_tasks;
        }
        http_pool_diag_maybe_print_locked(pool, "submit_ok", 0);
        pthread_mutex_unlock(&pool->lock);
        return HTTP_OK;
    }

    if (err == HTTP_ERR_QUEUE_FULL) {
        http_pool_task_node_t *node = (http_pool_task_node_t *)calloc(1U, sizeof(*node));
        if (node == NULL) {
            return HTTP_ERR_MEMORY_ALLOC;
        }
        /*
         * Same rule as above: struct bytes copied, pointee memory not duplicated.
         */
        memcpy(&node->task, task, sizeof(*task));
        pthread_mutex_lock(&pool->lock);
        http_pool_overflow_push_by_priority_locked(pool, node, task->priority);
        pool->pending_tasks++;
        if (pool->pending_tasks > pool->peak_pending_tasks) {
            pool->peak_pending_tasks = pool->pending_tasks;
        }
        http_pool_diag_maybe_print_locked(pool, "submit_overflow_retry", 1);
        pthread_mutex_unlock(&pool->lock);
        return HTTP_OK;
    }

    return err;
}

void http_pool_wait(http_pool_t *pool)
{
    if (!pool) {
        return;
    }

    pthread_mutex_lock(&pool->lock);
    http_pool_diag_maybe_print_locked(pool, "wait_enter", 1);
    while (pool->active_threads > 0U || pool->pending_tasks > 0U || pool->overflow_count > 0U) {
        pthread_cond_wait(&pool->idle_cond, &pool->lock);
    }
    http_pool_diag_maybe_print_locked(pool, "wait_exit", 1);
    pthread_mutex_unlock(&pool->lock);
}

uint32_t http_pool_get_active(http_pool_t *pool)
{
    uint32_t v;

    if (!pool) {
        return 0U;
    }

    pthread_mutex_lock(&pool->lock);
    v = pool->active_threads;
    pthread_mutex_unlock(&pool->lock);
    return v;
}

uint32_t http_pool_get_pending(http_pool_t *pool)
{
    uint32_t v;

    if (!pool) {
        return 0U;
    }

    pthread_mutex_lock(&pool->lock);
    v = pool->pending_tasks + pool->overflow_count;
    pthread_mutex_unlock(&pool->lock);
    return v;
}


void http_pool_get_diag(http_pool_t *pool, http_pool_diag_t *diag)
{
    if (!pool || !diag) {
        return;
    }

    memset(diag, 0, sizeof(*diag));
    pthread_mutex_lock(&pool->lock);
    diag->active_threads = pool->active_threads;
    diag->pending_tasks = pool->pending_tasks;
    diag->overflow_count = pool->overflow_count;
    diag->peak_active_threads = pool->peak_active_threads;
    diag->peak_pending_tasks = pool->peak_pending_tasks;
    diag->peak_overflow_count = pool->peak_overflow_count;
    diag->submitted_tasks = pool->submitted_tasks;
    diag->completed_tasks = pool->completed_tasks;
    diag->submit_wait_events = pool->submit_wait_events;
    diag->overflow_push_events = pool->overflow_push_events;
    diag->mq_full_retry_events = pool->mq_full_retry_events;
    pthread_mutex_unlock(&pool->lock);
}
