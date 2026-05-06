#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include "ktv_ctrl.h"
#include "http_api.h"

typedef struct KtvCompletionNode
{
    KtvRequest_t *req;
    int result;
    void *data;
    size_t data_len;
    struct KtvCompletionNode *next;
} KtvCompletionNode_t;

typedef struct
{
    KtvCtrlConfig_t cfg;
    int initialized;
    int shutting_down;
    pthread_mutex_t lock;

    pthread_t completion_thread;
    pthread_mutex_t completion_lock;
    pthread_cond_t completion_cond;
    pthread_cond_t completion_idle_cond;
    KtvCompletionNode_t *completion_head;
    KtvCompletionNode_t *completion_tail;
    uint32_t completion_depth;
    uint32_t outstanding_reqs;
    uint32_t max_outstanding_reqs;
    uint32_t peak_completion_depth;
    uint32_t peak_outstanding_reqs;
    uint64_t submitted_reqs;
    uint64_t completed_reqs;
    uint64_t post_wait_events;
    uint64_t enqueue_fail_events;
    uint64_t diag_last_print_ms;
    int completion_running;
} KtvCtrlContext_t;

static KtvCtrlContext_t g_ktv_ctrl = {
    .initialized = 0,
    .shutting_down = 0,
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .completion_lock = PTHREAD_MUTEX_INITIALIZER,
    .completion_cond = PTHREAD_COND_INITIALIZER,
    .completion_idle_cond = PTHREAD_COND_INITIALIZER,
    .completion_head = NULL,
    .completion_tail = NULL,
    .completion_depth = 0U,
    .outstanding_reqs = 0U,
    .max_outstanding_reqs = 2048U,
    .completion_running = 0,
};


static uint64_t ktv_ctrl_now_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((uint64_t)tv.tv_sec * 1000ULL) + ((uint64_t)tv.tv_usec / 1000ULL);
}

static void ktv_ctrl_diag_maybe_print_locked(const char *reason, int force)
{
#if HTTP_DIAG_ENABLE
    uint64_t now_ms;
    const char *bottleneck = "smooth";

    now_ms = ktv_ctrl_now_ms();
    if (!force && (now_ms - g_ktv_ctrl.diag_last_print_ms) < HTTP_DIAG_INTERVAL_MS) {
        return;
    }
    g_ktv_ctrl.diag_last_print_ms = now_ms;

    if (g_ktv_ctrl.completion_depth > (g_ktv_ctrl.max_outstanding_reqs / 4U)) {
        bottleneck = "completion callback slow";
    } else if (g_ktv_ctrl.outstanding_reqs >= g_ktv_ctrl.max_outstanding_reqs) {
        bottleneck = "post backpressure hit";
    }

    HTTP_DIAG("ktv[%s] completion_depth=%u outstanding=%u/%u | peak_completion=%u peak_outstanding=%u | submitted=%llu completed=%llu post_wait=%llu enqueue_fail=%llu | bottleneck=%s",
              reason,
              g_ktv_ctrl.completion_depth,
              g_ktv_ctrl.outstanding_reqs,
              g_ktv_ctrl.max_outstanding_reqs,
              g_ktv_ctrl.peak_completion_depth,
              g_ktv_ctrl.peak_outstanding_reqs,
              (unsigned long long)g_ktv_ctrl.submitted_reqs,
              (unsigned long long)g_ktv_ctrl.completed_reqs,
              (unsigned long long)g_ktv_ctrl.post_wait_events,
              (unsigned long long)g_ktv_ctrl.enqueue_fail_events,
              bottleneck);
#else
    (void)reason;
    (void)force;
#endif
}

static void ktv_ctrl_fill_default_config(KtvCtrlConfig_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    memset(cfg, 0, sizeof(*cfg));
    cfg->thread_num = 4;
    cfg->queue_size = 1024;
    cfg->timeout_sec = 20;
    cfg->connect_timeout_sec = 5;
    cfg->max_redirects = 5;
}

static void ktv_ctrl_sanitize_config(KtvCtrlConfig_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    if (cfg->thread_num == 0U || cfg->thread_num > 32U) {
        cfg->thread_num = 4U;
    }
    if (cfg->queue_size == 0U || cfg->queue_size > 4096U) {
        cfg->queue_size = 1024U;
    }
    if (cfg->timeout_sec == 0U || cfg->timeout_sec > 300U) {
        cfg->timeout_sec = 20U;
    }
    if (cfg->connect_timeout_sec == 0U || cfg->connect_timeout_sec > 60U) {
        cfg->connect_timeout_sec = 5U;
    }
    if (cfg->max_redirects == 0U || cfg->max_redirects > 20U) {
        cfg->max_redirects = 5U;
    }
}

static void ktv_ctrl_free_request(KtvRequest_t *req)
{
    if (req != NULL) {
        free(req);
    }
}

static void ktv_ctrl_complete_request(KtvRequest_t *req,
                                      int result,
                                      void *data,
                                      size_t data_len)
{
    if (req != NULL && req->ops != NULL && req->ops->on_complete != NULL) {
        req->ops->on_complete(req, result, data, data_len);
    }

    if (data != NULL) {
        free(data);
    }

    pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
    g_ktv_ctrl.completed_reqs++;
    if (g_ktv_ctrl.outstanding_reqs > 0U) {
        g_ktv_ctrl.outstanding_reqs--;
    }
    if (g_ktv_ctrl.completion_depth == 0U && g_ktv_ctrl.outstanding_reqs == 0U) {
        pthread_cond_broadcast(&g_ktv_ctrl.completion_idle_cond);
    }
    ktv_ctrl_diag_maybe_print_locked("complete", 0);
    pthread_cond_broadcast(&g_ktv_ctrl.completion_cond);
    pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);

    ktv_ctrl_free_request(req);
}

static void *ktv_ctrl_completion_thread(void *arg)
{
    (void)arg;

    while (1) {
        KtvCompletionNode_t *node = NULL;

        pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
        while (g_ktv_ctrl.completion_running && g_ktv_ctrl.completion_head == NULL) {
            pthread_cond_wait(&g_ktv_ctrl.completion_cond, &g_ktv_ctrl.completion_lock);
        }

        if (!g_ktv_ctrl.completion_running && g_ktv_ctrl.completion_head == NULL) {
            pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);
            break;
        }

        node = g_ktv_ctrl.completion_head;
        if (node != NULL) {
            g_ktv_ctrl.completion_head = node->next;
            if (g_ktv_ctrl.completion_head == NULL) {
                g_ktv_ctrl.completion_tail = NULL;
            }
            if (g_ktv_ctrl.completion_depth > 0U) {
                g_ktv_ctrl.completion_depth--;
            }
        }
        pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);

        if (node != NULL) {
            ktv_ctrl_complete_request(node->req, node->result, node->data, node->data_len);
            free(node);
        }
    }

    return NULL;
}

static int ktv_ctrl_enqueue_completion(KtvRequest_t *req,
                                       int result,
                                       const void *data_ptr,
                                       size_t data_len)
{
    KtvCompletionNode_t *node;
    void *data_copy = NULL;

    if (req == NULL) {
        return -1;
    }

    if (data_ptr != NULL && data_len > 0U) {
        data_copy = malloc(data_len + 1U);
        if (data_copy == NULL) {
            return -1;
        }
        memcpy(data_copy, data_ptr, data_len);
        ((char *)data_copy)[data_len] = '\0';
    }

    node = (KtvCompletionNode_t *)calloc(1U, sizeof(*node));
    if (node == NULL) {
        free(data_copy);
        return -1;
    }

    node->req = req;
    node->result = result;
    node->data = data_copy;
    node->data_len = data_len;
    node->next = NULL;

    pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
    if (g_ktv_ctrl.completion_tail != NULL) {
        g_ktv_ctrl.completion_tail->next = node;
    } else {
        g_ktv_ctrl.completion_head = node;
    }
    g_ktv_ctrl.completion_tail = node;
    g_ktv_ctrl.completion_depth++;
    if (g_ktv_ctrl.completion_depth > g_ktv_ctrl.peak_completion_depth) {
        g_ktv_ctrl.peak_completion_depth = g_ktv_ctrl.completion_depth;
    }
    ktv_ctrl_diag_maybe_print_locked("enqueue_completion", 0);
    pthread_cond_signal(&g_ktv_ctrl.completion_cond);
    pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);

    return 0;
}

static void internal_http_bridge(http_task_t *task, void *user_data)
{
    KtvRequest_t *req;
    int result;
    const void *data_ptr = NULL;
    size_t data_len = 0U;

    if (task == NULL || user_data == NULL) {
        return;
    }

    req = (KtvRequest_t *)user_data;
    result = (task->err_code == HTTP_OK) ? 0 : -1;

    switch (task->type)
    {
    case HTTP_TASK_FETCH_MEMORY:
        data_ptr = task->response_data;
        data_len = task->response_size;
        break;

    case HTTP_TASK_DOWNLOAD_FILE:
        data_ptr = task->local_path;
        data_len = (task->local_path[0] != '\0') ? strlen(task->local_path) : 0U;
        break;

    default:
        break;
    }

    if (ktv_ctrl_enqueue_completion(req, result, data_ptr, data_len) != 0) {
        pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
        g_ktv_ctrl.enqueue_fail_events++;
        ktv_ctrl_diag_maybe_print_locked("enqueue_fail", 1);
        pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);
        printf("[KTV_CTRL] completion enqueue failed, req id=%d url=%s\n", req->id, req->url);
        ktv_ctrl_complete_request(req, -1, NULL, 0U);
    }
}

int Ktv_Ctrl_Init(void)
{
    return Ktv_Ctrl_InitEx(NULL);
}

int Ktv_Ctrl_InitEx(const KtvCtrlConfig_t *cfg)
{
    http_config_t http_cfg;
    http_err_t err;

    pthread_mutex_lock(&g_ktv_ctrl.lock);

    if (g_ktv_ctrl.initialized) {
        pthread_mutex_unlock(&g_ktv_ctrl.lock);
        return 0;
    }

    if (cfg != NULL) {
        memcpy(&g_ktv_ctrl.cfg, cfg, sizeof(KtvCtrlConfig_t));
    } else {
        ktv_ctrl_fill_default_config(&g_ktv_ctrl.cfg);
    }

    ktv_ctrl_sanitize_config(&g_ktv_ctrl.cfg);

    http_config_default(&http_cfg);
    http_cfg.thread_num = g_ktv_ctrl.cfg.thread_num;
    http_cfg.queue_size = g_ktv_ctrl.cfg.queue_size;
    http_cfg.timeout = g_ktv_ctrl.cfg.timeout_sec;
    http_cfg.connect_timeout = g_ktv_ctrl.cfg.connect_timeout_sec;
    http_cfg.max_redirects = g_ktv_ctrl.cfg.max_redirects;

    printf("[KTV_CTRL] init: thread_num=%u queue_size=%u timeout=%u connect_timeout=%u max_redirects=%u\n",
           http_cfg.thread_num,
           http_cfg.queue_size,
           http_cfg.timeout,
           http_cfg.connect_timeout,
           http_cfg.max_redirects);

    err = http_init(&http_cfg);
    printf("[KTV_CTRL] http_init ret=%d\n", (int)err);

    if (err != HTTP_OK && err != HTTP_ERR_ALREADY_INIT) {
        pthread_mutex_unlock(&g_ktv_ctrl.lock);
        return -1;
    }

    g_ktv_ctrl.max_outstanding_reqs = (g_ktv_ctrl.cfg.queue_size >= 64U)
                                      ? (g_ktv_ctrl.cfg.queue_size * 2U)
                                      : 128U;
    g_ktv_ctrl.completion_head = NULL;
    g_ktv_ctrl.completion_tail = NULL;
    g_ktv_ctrl.completion_depth = 0U;
    g_ktv_ctrl.outstanding_reqs = 0U;
    g_ktv_ctrl.peak_completion_depth = 0U;
    g_ktv_ctrl.peak_outstanding_reqs = 0U;
    g_ktv_ctrl.submitted_reqs = 0ULL;
    g_ktv_ctrl.completed_reqs = 0ULL;
    g_ktv_ctrl.post_wait_events = 0ULL;
    g_ktv_ctrl.enqueue_fail_events = 0ULL;
    g_ktv_ctrl.diag_last_print_ms = 0ULL;
    g_ktv_ctrl.shutting_down = 0;
    g_ktv_ctrl.completion_running = 1;

    if (pthread_create(&g_ktv_ctrl.completion_thread, NULL, ktv_ctrl_completion_thread, NULL) != 0) {
        g_ktv_ctrl.completion_running = 0;
        http_deinit();
        pthread_mutex_unlock(&g_ktv_ctrl.lock);
        return -1;
    }

    g_ktv_ctrl.initialized = 1;
    pthread_mutex_unlock(&g_ktv_ctrl.lock);
    return 0;
}

void Ktv_Ctrl_Deinit(void)
{
    // pthread_mutex_lock(&g_ktv_ctrl.lock);
    // if (!g_ktv_ctrl.initialized) {
    //     pthread_mutex_unlock(&g_ktv_ctrl.lock);
    //     return;
    // }
    // g_ktv_ctrl.initialized = 0;
    // g_ktv_ctrl.shutting_down = 1;
    // pthread_mutex_unlock(&g_ktv_ctrl.lock);

    // Ktv_Ctrl_WaitAll();
    // http_deinit();

    // pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
    // g_ktv_ctrl.completion_running = 0;
    // pthread_cond_broadcast(&g_ktv_ctrl.completion_cond);
    // pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);

    // pthread_join(g_ktv_ctrl.completion_thread, NULL);
}

int Ktv_Ctrl_PostTask(const KtvRequest_t *req)
{
    KtvRequest_t *task_req;
    http_err_t err;

    if (req == NULL || req->ops == NULL) {
        printf("[KTV_CTRL] PostTask fail: req or ops is NULL\n");
        return -1;
    }

    /*
     * Lazy init:
     * - Do NOT couple controller init to WiFi state.
     * - When WiFi is not connected, requests will naturally fail at HTTP layer,
     *   but the controller itself should still be able to accept tasks.
     *
     * This also avoids "PostTask fail: controller not initialized" after app
     * relaunches where init ordering changes.
     */
    if (!g_ktv_ctrl.initialized) {
        if (Ktv_Ctrl_Init() != 0) {
            printf("[KTV_CTRL] PostTask fail: auto init failed\n");
            return -1;
        }
    }

    if (req->url[0] == '\0') {
        printf("[KTV_CTRL] PostTask fail: empty url\n");
        return -1;
    }

    task_req = (KtvRequest_t *)malloc(sizeof(KtvRequest_t));
    if (task_req == NULL) {
        printf("[KTV_CTRL] PostTask fail: malloc failed\n");
        return -1;
    }

    /*
     * COPY SEMANTICS (IMPORTANT):
     * - Here we do a DEEP COPY of KtvRequest_t struct bytes into heap memory.
     * - Fields inside KtvRequest_t are fixed arrays/pointers:
     *   * url/local_path arrays are copied by value (safe).
     *   * user_data pointer is copied as pointer value only (NOT deep-copied).
     * - Caller must ensure req->user_data lifetime is valid until on_complete.
     */
    memcpy(task_req, req, sizeof(KtvRequest_t));

    pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
    g_ktv_ctrl.submitted_reqs++;
    while (!g_ktv_ctrl.shutting_down &&
           g_ktv_ctrl.outstanding_reqs >= g_ktv_ctrl.max_outstanding_reqs) {
        g_ktv_ctrl.post_wait_events++;
        ktv_ctrl_diag_maybe_print_locked("post_wait", 1);
        pthread_cond_wait(&g_ktv_ctrl.completion_cond, &g_ktv_ctrl.completion_lock);
    }
    if (g_ktv_ctrl.shutting_down) {
        pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);
        free(task_req);
        return -1;
    }
    g_ktv_ctrl.outstanding_reqs++;
    if (g_ktv_ctrl.outstanding_reqs > g_ktv_ctrl.peak_outstanding_reqs) {
        g_ktv_ctrl.peak_outstanding_reqs = g_ktv_ctrl.outstanding_reqs;
    }
    ktv_ctrl_diag_maybe_print_locked("post_submit", 0);
    pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);

    /* Ktv 层和 http 层各有一份 enum，避免循环依赖。这里做显式映射。 */
    http_priority_t hp = (task_req->priority == KTV_PRIORITY_HIGH)
                         ? HTTP_PRIORITY_HIGH : HTTP_PRIORITY_LOW;

    switch (task_req->ops->type)
    {
    case KTV_REQ_FETCH_JSON:
    case KTV_REQ_FETCH_MEMORY:
        /* 关键：歌手页 page JSON 必须用 HIGH。它的 body 里装着这一页 50 张
         * 图的真实 URL，JSON 不到，下面所有 image 任务都没法启动。如果它和
         * image 一起走 LOW FIFO，就会出现"50 张图早就排在队里、JSON 卡在
         * 后面"的恶性场景。 */
        err = http_fetch_priority(task_req->url, hp, internal_http_bridge, task_req);
        if (err != HTTP_OK) {
            /*
             * Return-value contract:
             * - If Ktv_Ctrl_PostTask() returns non-zero, no async callback is pending.
             * - So submission failures are completed synchronously here.
             */
            ktv_ctrl_complete_request(task_req, -1, NULL, 0U);
            return -1;
        }
        break;

    case KTV_REQ_DOWNLOAD_FILE:
    case KTV_REQ_DOWNLOAD_IMAGE:
        if (task_req->local_path[0] == '\0') {
            printf("[KTV_CTRL] PostTask fail: empty local_path for download task\n");
            ktv_ctrl_complete_request(task_req, -1, NULL, 0U);
            return -1;
        }

        err = http_download_priority(task_req->url,
                                     task_req->local_path,
                                     hp,
                                     internal_http_bridge,
                                     task_req);
        if (err != HTTP_OK) {
            /*
             * Keep same contract as fetch: post failure means callback has already
             * been completed synchronously and no async callback will arrive later.
             */
            ktv_ctrl_complete_request(task_req, -1, NULL, 0U);
            return -1;
        }
        break;

    default:
        printf("[KTV_CTRL] PostTask fail: unknown req type=%d\n", (int)task_req->ops->type);
        ktv_ctrl_complete_request(task_req, -1, NULL, 0U);
        return -1;
    }

    return 0;
}

void Ktv_Ctrl_WaitAll(void)
{
    if (!g_ktv_ctrl.initialized && !g_ktv_ctrl.shutting_down) {
        return;
    }

    http_wait();

    pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
    ktv_ctrl_diag_maybe_print_locked("wait_enter", 1);
    while (g_ktv_ctrl.completion_depth > 0U || g_ktv_ctrl.outstanding_reqs > 0U) {
        pthread_cond_wait(&g_ktv_ctrl.completion_idle_cond, &g_ktv_ctrl.completion_lock);
    }
    ktv_ctrl_diag_maybe_print_locked("wait_exit", 1);
    pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);
}

void Ktv_Ctrl_PrintStats(void)
{
    pthread_mutex_lock(&g_ktv_ctrl.completion_lock);
    ktv_ctrl_diag_maybe_print_locked("manual_snapshot", 1);
    pthread_mutex_unlock(&g_ktv_ctrl.completion_lock);
    http_print_stats();
}
