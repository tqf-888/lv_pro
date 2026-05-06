#include "http_post_api.h"

#include <curl/curl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef HTTP_POST_WORKER_COUNT
#define HTTP_POST_WORKER_COUNT  2
#endif

#ifndef HTTP_POST_MAX_QUEUE
#define HTTP_POST_MAX_QUEUE     256
#endif

typedef struct {
    char *memory;
    size_t size;
    size_t capacity;
} http_post_mem_chunk_t;

typedef struct http_post_job {
    uint32_t task_id;
    char *url;
    char *body;
    size_t body_len;
    char *content_type;
    uint32_t timeout;
    http_callback_t callback;
    void *user_data;
    struct http_post_job *next;
} http_post_job_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_cond_t idle_cond;
    pthread_t workers[HTTP_POST_WORKER_COUNT];
    uint32_t started_workers;
    int started;
    int stop;
    http_post_job_t *head;
    http_post_job_t *tail;
    uint32_t queue_len;
    uint32_t active_workers;
} http_post_runtime_t;

static http_post_runtime_t g_http_post_runtime = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .idle_cond = PTHREAD_COND_INITIALIZER,
    .started_workers = 0,
    .started = 0,
    .stop = 0,
    .head = NULL,
    .tail = NULL,
    .queue_len = 0,
    .active_workers = 0,
};

static pthread_once_t g_http_post_curl_once = PTHREAD_ONCE_INIT;
static int g_http_post_curl_ok = 0;
static uint32_t g_http_post_task_seed = 0;

static void http_post_curl_global_init_once(void)
{
    g_http_post_curl_ok = (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK) ? 1 : 0;
}

static char *http_post_strdup_safe(const char *src)
{
    size_t len;
    char *dst;

    if (src == NULL) {
        src = "";
    }

    len = strlen(src);
    dst = (char *)malloc(len + 1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

static char *http_post_memdup_safe(const void *src, size_t len)
{
    const char *input = (const char *)src;
    char *dst = (char *)malloc(len + 1);
    if (dst == NULL) {
        return NULL;
    }

    if (len > 0 && input != NULL) {
        memcpy(dst, input, len);
    }
    dst[len] = '\0';
    return dst;
}

static int http_post_is_valid_url(const char *url)
{
    const char *p;
    const char *host_begin;
    const char *host_end;

    if (url == NULL) {
        return 0;
    }

    if (!(strncmp(url, "http://", 7) == 0 || strncmp(url, "https://", 8) == 0)) {
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

static void http_post_free_job(http_post_job_t *job)
{
    if (job == NULL) {
        return;
    }

    free(job->url);
    free(job->body);
    free(job->content_type);
    free(job);
}

static size_t http_post_write_memory_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    http_post_mem_chunk_t *mem = (http_post_mem_chunk_t *)userp;
    size_t need;
    size_t new_cap;
    char *p;

    if (mem == NULL) {
        return 0;
    }

    need = mem->size + realsize + 1;
    if (need > mem->capacity) {
        new_cap = (mem->capacity > 0) ? mem->capacity : HTTP_CHUNK_SIZE;
        while (new_cap < need) {
            new_cap *= 2;
        }

        p = (char *)realloc(mem->memory, new_cap);
        if (p == NULL) {
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

static int http_post_progress_cb(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                                 curl_off_t ultotal, curl_off_t ulnow)
{
    http_task_t *task = (http_task_t *)clientp;
    (void)dlnow;
    (void)ulnow;

    if (task != NULL) {
        task->total_size = (dltotal > 0) ? (size_t)dltotal : 0U;
        task->download_size = (ultotal > 0) ? (size_t)ultotal : 0U;
    }

    return 0;
}

static void http_post_fill_task_base(http_task_t *task, const http_post_job_t *job)
{
    memset(task, 0, sizeof(*task));
    task->task_id = job->task_id;
    task->type = HTTP_TASK_UPLOAD_FILE;
    task->state = HTTP_STATE_RUNNING;
    task->timeout = job->timeout;
    task->callback = job->callback;
    task->user_data = job->user_data;
    task->err_code = HTTP_OK;
    task->http_status = 0;
    task->response_data = NULL;
    task->response_size = 0U;
    task->download_size = 0U;
    task->total_size = 0U;
    task->curl_handle = NULL;
    task->priv_data = NULL;

    snprintf(task->url, sizeof(task->url), "%s", job->url ? job->url : "");
    snprintf(task->header, sizeof(task->header), "Content-Type: %s",
             (job->content_type && job->content_type[0] != '\0') ? job->content_type : "application/json");
}

static void http_post_execute_one(const http_post_job_t *job, http_task_t *task)
{
    CURL *curl = NULL;
    CURLcode res;
    struct curl_slist *headers = NULL;
    http_post_mem_chunk_t chunk;
    long http_code = 0;

    memset(&chunk, 0, sizeof(chunk));
    http_post_fill_task_base(task, job);

    if (!http_post_is_valid_url(job->url)) {
        task->state = HTTP_STATE_FAILED;
        task->err_code = HTTP_ERR_INVALID_PARAM;
        return;
    }

    chunk.memory = (char *)malloc(HTTP_CHUNK_SIZE);
    if (chunk.memory == NULL) {
        task->state = HTTP_STATE_FAILED;
        task->err_code = HTTP_ERR_MEMORY_ALLOC;
        return;
    }
    chunk.capacity = HTTP_CHUNK_SIZE;
    chunk.size = 0U;
    chunk.memory[0] = '\0';

    curl = curl_easy_init();
    if (curl == NULL) {
        free(chunk.memory);
        task->state = HTTP_STATE_FAILED;
        task->err_code = HTTP_ERR_CURL_INIT;
        return;
    }

    task->curl_handle = curl;

    headers = curl_slist_append(headers, task->header);
    headers = curl_slist_append(headers, "Expect:");
    headers = curl_slist_append(headers, "Connection: close");
    if (headers == NULL) {
        curl_easy_cleanup(curl);
        free(chunk.memory);
        task->curl_handle = NULL;
        task->state = HTTP_STATE_FAILED;
        task->err_code = HTTP_ERR_MEMORY_ALLOC;
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, job->url);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, job->body ? job->body : "");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)job->body_len);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

#if HTTP_TLS_VERIFY
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, HTTP_TLS_CAINFO);
#else
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

    curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, job->timeout ? job->timeout : HTTP_TIMEOUT_DEFAULT);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, HTTP_CONNECT_TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, HTTP_MAX_REDIRECTS);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, http_post_progress_cb);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, task);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "EmbeddedHTTPPostClient/1.0");
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_post_write_memory_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    task->http_status = http_code;

    if (res == CURLE_OK) {
        task->response_data = chunk.memory;
        task->response_size = chunk.size;

        if (http_code >= 400L) {
            task->state = HTTP_STATE_FAILED;
            task->err_code = HTTP_ERR_NETWORK;
        } else {
            task->state = HTTP_STATE_COMPLETED;
            task->err_code = HTTP_OK;
        }
    } else {
        free(chunk.memory);
        task->response_data = NULL;
        task->response_size = 0U;
        task->state = HTTP_STATE_FAILED;
        task->err_code = (res == CURLE_OPERATION_TIMEDOUT) ? HTTP_ERR_TIMEOUT : HTTP_ERR_NETWORK;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    task->curl_handle = NULL;
}

static void *http_post_worker_main(void *arg)
{
    http_post_job_t *job;
    http_task_t task;
    (void)arg;

    while (1) {
        pthread_mutex_lock(&g_http_post_runtime.lock);
        while (!g_http_post_runtime.stop && g_http_post_runtime.head == NULL) {
            pthread_cond_wait(&g_http_post_runtime.cond, &g_http_post_runtime.lock);
        }

        if (g_http_post_runtime.stop && g_http_post_runtime.head == NULL) {
            pthread_mutex_unlock(&g_http_post_runtime.lock);
            break;
        }

        job = g_http_post_runtime.head;
        g_http_post_runtime.head = job->next;
        if (g_http_post_runtime.head == NULL) {
            g_http_post_runtime.tail = NULL;
        }
        if (g_http_post_runtime.queue_len > 0U) {
            g_http_post_runtime.queue_len--;
        }
        g_http_post_runtime.active_workers++;
        pthread_mutex_unlock(&g_http_post_runtime.lock);

        http_post_execute_one(job, &task);

        if (job->callback != NULL) {
            job->callback(&task, job->user_data);
        }

        free(task.response_data);
        task.response_data = NULL;
        task.response_size = 0U;
        http_post_free_job(job);

        pthread_mutex_lock(&g_http_post_runtime.lock);
        if (g_http_post_runtime.active_workers > 0U) {
            g_http_post_runtime.active_workers--;
        }
        if (g_http_post_runtime.queue_len == 0U && g_http_post_runtime.active_workers == 0U) {
            pthread_cond_broadcast(&g_http_post_runtime.idle_cond);
        }
        pthread_mutex_unlock(&g_http_post_runtime.lock);
    }

    return NULL;
}

static http_err_t http_post_start_runtime(void)
{
    uint32_t i;
    uint32_t started = 0U;

    pthread_once(&g_http_post_curl_once, http_post_curl_global_init_once);
    if (!g_http_post_curl_ok) {
        return HTTP_ERR_CURL_INIT;
    }

    pthread_mutex_lock(&g_http_post_runtime.lock);
    if (g_http_post_runtime.started) {
        pthread_mutex_unlock(&g_http_post_runtime.lock);
        return HTTP_OK;
    }

    g_http_post_runtime.stop = 0;
    for (i = 0U; i < HTTP_POST_WORKER_COUNT; ++i) {
        if (pthread_create(&g_http_post_runtime.workers[i], NULL, http_post_worker_main, NULL) != 0) {
            g_http_post_runtime.stop = 1;
            g_http_post_runtime.started_workers = started;
            pthread_cond_broadcast(&g_http_post_runtime.cond);
            pthread_mutex_unlock(&g_http_post_runtime.lock);

            for (i = 0U; i < started; ++i) {
                pthread_join(g_http_post_runtime.workers[i], NULL);
            }

            pthread_mutex_lock(&g_http_post_runtime.lock);
            memset(g_http_post_runtime.workers, 0, sizeof(g_http_post_runtime.workers));
            g_http_post_runtime.started_workers = 0U;
            g_http_post_runtime.stop = 0;
            g_http_post_runtime.started = 0;
            pthread_mutex_unlock(&g_http_post_runtime.lock);
            return HTTP_ERR_THREAD_CREATE;
        }
        started++;
    }

    g_http_post_runtime.started_workers = started;
    g_http_post_runtime.started = 1;
    pthread_mutex_unlock(&g_http_post_runtime.lock);
    return HTTP_OK;
}

http_err_t http_post(const char *url,
                     const char *body,
                     const char *content_type,
                     http_callback_t callback,
                     void *user_data)
{
    http_post_job_t *job;
    http_err_t err;
    size_t body_len;

    if (!http_post_is_valid_url(url)) {
        return HTTP_ERR_INVALID_PARAM;
    }
    if (strlen(url) >= HTTP_MAX_URL_LEN) {
        return HTTP_ERR_INVALID_PARAM;
    }

    if (body == NULL) {
        body = "";
    }
    if (content_type == NULL || content_type[0] == '\0') {
        content_type = "application/json";
    }
    if (strlen(content_type) + strlen("Content-Type: ") >= HTTP_MAX_HEADER_LEN) {
        return HTTP_ERR_INVALID_PARAM;
    }

    err = http_post_start_runtime();
    if (err != HTTP_OK) {
        return err;
    }

    body_len = strlen(body);
    job = (http_post_job_t *)calloc(1, sizeof(http_post_job_t));
    if (job == NULL) {
        return HTTP_ERR_MEMORY_ALLOC;
    }

    job->url = http_post_strdup_safe(url);
    job->body = http_post_memdup_safe(body, body_len);
    job->content_type = http_post_strdup_safe(content_type);
    if (job->url == NULL || job->body == NULL || job->content_type == NULL) {
        http_post_free_job(job);
        return HTTP_ERR_MEMORY_ALLOC;
    }

    job->task_id = __sync_add_and_fetch(&g_http_post_task_seed, 1U);
    job->body_len = body_len;
    job->timeout = HTTP_TIMEOUT_DEFAULT;
    job->callback = callback;
    job->user_data = user_data;
    job->next = NULL;

    pthread_mutex_lock(&g_http_post_runtime.lock);
    if (!g_http_post_runtime.started || g_http_post_runtime.stop) {
        pthread_mutex_unlock(&g_http_post_runtime.lock);
        http_post_free_job(job);
        return HTTP_ERR_NOT_INIT;
    }

    if (g_http_post_runtime.queue_len >= HTTP_POST_MAX_QUEUE) {
        pthread_mutex_unlock(&g_http_post_runtime.lock);
        http_post_free_job(job);
        return HTTP_ERR_QUEUE_FULL;
    }

    if (g_http_post_runtime.tail != NULL) {
        g_http_post_runtime.tail->next = job;
    } else {
        g_http_post_runtime.head = job;
    }
    g_http_post_runtime.tail = job;
    g_http_post_runtime.queue_len++;
    pthread_cond_signal(&g_http_post_runtime.cond);
    pthread_mutex_unlock(&g_http_post_runtime.lock);

    return HTTP_OK;
}

void http_post_wait_all(void)
{
    pthread_mutex_lock(&g_http_post_runtime.lock);
    while (g_http_post_runtime.started &&
           (g_http_post_runtime.queue_len > 0U || g_http_post_runtime.active_workers > 0U)) {
        pthread_cond_wait(&g_http_post_runtime.idle_cond, &g_http_post_runtime.lock);
    }
    pthread_mutex_unlock(&g_http_post_runtime.lock);
}

void http_post_shutdown(void)
{
    uint32_t i;
    http_post_job_t *job;

    pthread_mutex_lock(&g_http_post_runtime.lock);
    if (!g_http_post_runtime.started) {
        pthread_mutex_unlock(&g_http_post_runtime.lock);
        return;
    }

    g_http_post_runtime.stop = 1;
    pthread_cond_broadcast(&g_http_post_runtime.cond);
    pthread_mutex_unlock(&g_http_post_runtime.lock);

    for (i = 0U; i < g_http_post_runtime.started_workers; ++i) {
        pthread_join(g_http_post_runtime.workers[i], NULL);
    }

    pthread_mutex_lock(&g_http_post_runtime.lock);
    while (g_http_post_runtime.head != NULL) {
        job = g_http_post_runtime.head;
        g_http_post_runtime.head = job->next;
        http_post_free_job(job);
    }
    g_http_post_runtime.tail = NULL;
    g_http_post_runtime.queue_len = 0U;
    g_http_post_runtime.active_workers = 0U;
    g_http_post_runtime.started_workers = 0U;
    g_http_post_runtime.started = 0;
    g_http_post_runtime.stop = 0;
    memset(g_http_post_runtime.workers, 0, sizeof(g_http_post_runtime.workers));
    pthread_mutex_unlock(&g_http_post_runtime.lock);
}
