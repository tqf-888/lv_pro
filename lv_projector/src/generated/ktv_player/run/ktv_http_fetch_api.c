#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include "ktv.h"
#include "ktv_ctrl.h"
#include "ktv_http_fetch_api.h"

#define KTV_HTTP_FETCH_DEFAULT_TIMEOUT_MS (15000)

typedef struct
{
    sem_t sem;
    pthread_mutex_t lock;
    int ref_count;
    int abandoned;
    int done;
    int success;
    char *out_data;
    size_t out_size;
} ktv_http_fetch_ctx_t;

static ktv_http_fetch_ctx_t *ktv_http_fetch_ctx_create(char *out_data, size_t out_size)
{
    ktv_http_fetch_ctx_t *ctx = (ktv_http_fetch_ctx_t *)calloc(1U, sizeof(*ctx));
    if (ctx == NULL) {
        return NULL;
    }

    if (sem_init(&ctx->sem, 0, 0) != 0) {
        free(ctx);
        return NULL;
    }
    if (pthread_mutex_init(&ctx->lock, NULL) != 0) {
        sem_destroy(&ctx->sem);
        free(ctx);
        return NULL;
    }

    /* waiter + async callback each own one reference */
    ctx->ref_count = 2;
    ctx->out_data = out_data;
    ctx->out_size = out_size;
    return ctx;
}

static void ktv_http_fetch_ctx_release(ktv_http_fetch_ctx_t *ctx)
{
    int do_free = 0;

    if (ctx == NULL) {
        return;
    }

    pthread_mutex_lock(&ctx->lock);
    ctx->ref_count--;
    if (ctx->ref_count == 0) {
        do_free = 1;
    }
    pthread_mutex_unlock(&ctx->lock);

    if (do_free) {
        sem_destroy(&ctx->sem);
        pthread_mutex_destroy(&ctx->lock);
        free(ctx);
    }
}

static int ktv_http_fetch_ctx_abandon_waiter(ktv_http_fetch_ctx_t *ctx)
{
    int callback_missing = 0;

    if (ctx == NULL) {
        return 0;
    }

    pthread_mutex_lock(&ctx->lock);
    ctx->abandoned = 1;
    callback_missing = !ctx->done;
    pthread_mutex_unlock(&ctx->lock);

    return callback_missing;
}

static int ktv_http_fetch_wait_done(sem_t *sem, int timeout_ms)
{
    struct timespec ts;

    if (sem == NULL)
    {
        return -1;
    }

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
    {
        return -1;
    }

    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000L;
    if (ts.tv_nsec >= 1000000000L)
    {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }

    if (sem_timedwait(sem, &ts) != 0)
    {
        return -1;
    }

    return 0;
}

static void ktv_http_fetch_on_complete(KtvRequest_t *req,
                                       int result,
                                       const void *data,
                                       size_t data_len)
{
    ktv_http_fetch_ctx_t *ctx;
    size_t copy_len;

    (void)req;

    if (req == NULL)
    {
        return;
    }

    ctx = (ktv_http_fetch_ctx_t *)req->user_data;
    if (ctx == NULL)
    {
        return;
    }

    pthread_mutex_lock(&ctx->lock);

    if (!ctx->abandoned &&
        result == 0 &&
        data != NULL &&
        data_len > 0 &&
        ctx->out_data != NULL &&
        ctx->out_size > 1U)
    {
        copy_len = data_len;
        if (copy_len >= ctx->out_size)
        {
            copy_len = ctx->out_size - 1U;
        }

        memcpy(ctx->out_data, data, copy_len);
        ctx->out_data[copy_len] = '\0';
        ctx->success = 1;

        printf("[KTV] [HTTP_FETCH] GET success, url=%s, len=%u\n",
               req->url,
               (unsigned)copy_len);
    }
    else
    {
        if (!ctx->abandoned && ctx->out_data != NULL && ctx->out_size > 0U)
        {
            ctx->out_data[0] = '\0';
        }

        ctx->success = 0;
        printf("[KTV] [HTTP_FETCH] GET failed, result=%d, url=%s, data_len=%u\n",
               result,
               req->url,
               (unsigned)data_len);
    }

    ctx->done = 1;
    if (!ctx->abandoned) {
        sem_post(&ctx->sem);
    }
    pthread_mutex_unlock(&ctx->lock);
    ktv_http_fetch_ctx_release(ctx);
}

static const KtvReqOps_t g_ops_http_fetch_memory = {
    .type = KTV_REQ_FETCH_MEMORY,
    .on_complete = ktv_http_fetch_on_complete
};

int ktv_http_get_to_memory(const char *url,
                           char *out_data,
                           size_t out_size,
                           int timeout_ms)
{
    KtvRequest_t req;
    ktv_http_fetch_ctx_t *ctx;
    int success;

    if (url == NULL || url[0] == '\0' || out_data == NULL || out_size < 2U)
    {
        return 0;
    }

    if (timeout_ms <= 0)
    {
        timeout_ms = KTV_HTTP_FETCH_DEFAULT_TIMEOUT_MS;
    }

    out_data[0] = '\0';
    ctx = ktv_http_fetch_ctx_create(out_data, out_size);
    if (ctx == NULL)
    {
        printf("[KTV] [HTTP_FETCH] sem init failed, url=%s, errno=%d\n", url, errno);
        return 0;
    }

    memset(&req, 0, sizeof(req));
    snprintf(req.url, sizeof(req.url), "%s", url);
    req.ops = &g_ops_http_fetch_memory;
    req.user_data = ctx;

    printf("[KTV] [HTTP_FETCH] GET start, url=%s\n", req.url);
    if (Ktv_Ctrl_PostTask(&req) != 0)
    {
        int callback_missing = ktv_http_fetch_ctx_abandon_waiter(ctx);
        printf("[KTV] [HTTP_FETCH] post task failed, url=%s\n", req.url);
        if (callback_missing) {
            ktv_http_fetch_ctx_release(ctx);
        }
        ktv_http_fetch_ctx_release(ctx);
        return 0;
    }

    if (ktv_http_fetch_wait_done(&ctx->sem, timeout_ms) != 0)
    {
        printf("[KTV] [HTTP_FETCH] wait timeout, url=%s\n", req.url);
        (void)ktv_http_fetch_ctx_abandon_waiter(ctx);
        ktv_http_fetch_ctx_release(ctx);
        return 0;
    }

    pthread_mutex_lock(&ctx->lock);
    success = ctx->success;
    ctx->abandoned = 1;
    pthread_mutex_unlock(&ctx->lock);
    ktv_http_fetch_ctx_release(ctx);
    return (success != 0) ? 1 : 0;
}
