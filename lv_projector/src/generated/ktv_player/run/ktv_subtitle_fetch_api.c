#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "ktv.h"
#include "ktv_ctrl.h"
#include "ktv_subtitle_fetch_api.h"

#define KTV_SUBTITLE_FETCH_TIMEOUT_MS    (15000)
#define KTV_SUBTITLE_LOCAL_PATH_LEN      (512)

typedef struct
{
    sem_t sem;
    pthread_mutex_t lock;
    int ref_count;
    int abandoned;
    int done;
    int success;
    char local_path[KTV_SUBTITLE_LOCAL_PATH_LEN];
} ktv_subtitle_fetch_ctx_t;

static ktv_subtitle_fetch_ctx_t *ktv_subtitle_fetch_ctx_create(void)
{
    ktv_subtitle_fetch_ctx_t *ctx = (ktv_subtitle_fetch_ctx_t *)calloc(1U, sizeof(*ctx));
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
    return ctx;
}

static void ktv_subtitle_fetch_ctx_release(ktv_subtitle_fetch_ctx_t *ctx)
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

static int ktv_subtitle_fetch_ctx_abandon_waiter(ktv_subtitle_fetch_ctx_t *ctx)
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

static int ktv_subtitle_is_http_url(const char *s)
{
    if (s == NULL)
    {
        return 0;
    }

    return (strncmp(s, "http://", 7) == 0 || strncmp(s, "https://", 8) == 0);
}

static void ktv_subtitle_json_unescape_string(const char *src, char *dst, size_t dst_size)
{
    size_t si;
    size_t di;

    if (dst == NULL || dst_size == 0U)
    {
        return;
    }

    dst[0] = '\0';
    if (src == NULL)
    {
        return;
    }

    si = 0U;
    di = 0U;
    while (src[si] != '\0' && di + 1U < dst_size)
    {
        if (src[si] == '\\' && src[si + 1U] != '\0')
        {
            si++;
            switch (src[si])
            {
                case '"': dst[di++] = '"'; si++; continue;
                case '\\': dst[di++] = '\\'; si++; continue;
                case '/': dst[di++] = '/'; si++; continue;
                case 'b': dst[di++] = '\b'; si++; continue;
                case 'f': dst[di++] = '\f'; si++; continue;
                case 'n': dst[di++] = '\n'; si++; continue;
                case 'r': dst[di++] = '\r'; si++; continue;
                case 't': dst[di++] = '\t'; si++; continue;
                default: dst[di++] = src[si++]; continue;
            }
        }

        dst[di++] = src[si++];
    }

    dst[di] = '\0';
}

static unsigned int ktv_subtitle_hash_str(const char *s)
{
    unsigned int hash = 5381U;
    unsigned char c;

    if (s == NULL)
    {
        return 0U;
    }

    while ((c = (unsigned char)*s++) != 0U)
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

static int ktv_subtitle_build_cache_path(int song_id,
                                         const char *subtitle_url,
                                         char *buf,
                                         size_t size)
{
    unsigned int hash;
    int n;

    if (subtitle_url == NULL || buf == NULL || size == 0U)
    {
        return -1;
    }

    hash = ktv_subtitle_hash_str(subtitle_url);
    if (song_id > 0)
    {
        n = snprintf(buf, size, "/tmp/ktv_subtitle_%d_%08x.zrc", song_id, hash);
    }
    else
    {
        n = snprintf(buf, size, "/tmp/ktv_subtitle_%08x.zrc", hash);
    }

    if (n < 0 || (size_t)n >= size)
    {
        return -1;
    }

    return 0;
}

static int ktv_subtitle_wait_done(sem_t *sem, int timeout_ms)
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

static void ktv_subtitle_download_on_complete(KtvRequest_t *req,
                                              int result,
                                              const void *data,
                                              size_t data_len)
{
    ktv_subtitle_fetch_ctx_t *ctx;
    const char *path = NULL;

    (void)data_len;

    if (req == NULL)
    {
        return;
    }

    ctx = (ktv_subtitle_fetch_ctx_t *)req->user_data;
    if (ctx == NULL)
    {
        return;
    }

    pthread_mutex_lock(&ctx->lock);

    if (!ctx->abandoned && result == 0)
    {
        path = (const char *)data;
        if (path != NULL && path[0] != '\0')
        {
            snprintf(ctx->local_path, sizeof(ctx->local_path), "%s", path);
        }
        else
        {
            snprintf(ctx->local_path, sizeof(ctx->local_path), "%s", req->local_path);
        }

        ctx->success = 1;
        printf("[KTV] [SUBTITLE_FETCH] download success, song_id=%d, local=%s\n",
               req->id,
               ctx->local_path);
    }
    else
    {
        if (!ctx->abandoned) {
            ctx->success = 0;
        }
        printf("[KTV] [SUBTITLE_FETCH] download failed, song_id=%d, result=%d, url=%s\n",
               req->id,
               result,
               req->url);
    }

    ctx->done = 1;
    if (!ctx->abandoned) {
        sem_post(&ctx->sem);
    }
    pthread_mutex_unlock(&ctx->lock);
    ktv_subtitle_fetch_ctx_release(ctx);
}

static const KtvReqOps_t g_ops_download_subtitle = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = ktv_subtitle_download_on_complete
};

int ktv_fetch_subtitle_to_local(int song_id,
                                const char *subtitle_url,
                                char *out_local_path,
                                size_t out_size)
{
    KtvRequest_t req;
    ktv_subtitle_fetch_ctx_t *ctx;
    char local_path[KTV_SUBTITLE_LOCAL_PATH_LEN];
    char normalized_subtitle_url[KTV_SUBTITLE_LOCAL_PATH_LEN];
    int success;

    if (out_local_path == NULL || out_size == 0U)
    {
        return 0;
    }

    out_local_path[0] = '\0';
    normalized_subtitle_url[0] = '\0';

    if (subtitle_url == NULL || subtitle_url[0] == '\0')
    {
        return 1;
    }

    ktv_subtitle_json_unescape_string(subtitle_url,
                                      normalized_subtitle_url,
                                      sizeof(normalized_subtitle_url));

    if (normalized_subtitle_url[0] == '\0')
    {
        return 1;
    }

    if (!ktv_subtitle_is_http_url(normalized_subtitle_url))
    {
        snprintf(out_local_path, out_size, "%s", normalized_subtitle_url);
        printf("[KTV] [SUBTITLE_FETCH] local subtitle direct use, song_id=%d, path=%s\n",
               song_id,
               out_local_path);
        return 1;
    }

    if (ktv_subtitle_build_cache_path(song_id,
                                      normalized_subtitle_url,
                                      local_path,
                                      sizeof(local_path)) != 0)
    {
        printf("[KTV] [SUBTITLE_FETCH] build cache path failed, song_id=%d\n", song_id);
        return 0;
    }

    ctx = ktv_subtitle_fetch_ctx_create();
    if (ctx == NULL)
    {
        printf("[KTV] [SUBTITLE_FETCH] sem init failed, song_id=%d, errno=%d\n",
               song_id,
               errno);
        return 0;
    }

    memset(&req, 0, sizeof(req));
    snprintf(req.url, sizeof(req.url), "%s", normalized_subtitle_url);
    snprintf(req.local_path, sizeof(req.local_path), "%s", local_path);
    req.ops = &g_ops_download_subtitle;
    req.id = song_id;
    req.page_index = song_id;
    req.user_data = ctx;

    printf("[KTV] [SUBTITLE_FETCH] start, song_id=%d, url=%s, local=%s\n",
           song_id,
           req.url,
           req.local_path);

    if (Ktv_Ctrl_PostTask(&req) != 0)
    {
        int callback_missing = ktv_subtitle_fetch_ctx_abandon_waiter(ctx);
        printf("[KTV] [SUBTITLE_FETCH] post task failed, song_id=%d, url=%s\n",
               song_id,
               normalized_subtitle_url);
        if (callback_missing) {
            ktv_subtitle_fetch_ctx_release(ctx);
        }
        ktv_subtitle_fetch_ctx_release(ctx);
        return 0;
    }

    if (ktv_subtitle_wait_done(&ctx->sem, KTV_SUBTITLE_FETCH_TIMEOUT_MS) != 0)
    {
        printf("[KTV] [SUBTITLE_FETCH] wait timeout, song_id=%d, url=%s\n",
               song_id,
               normalized_subtitle_url);
        (void)ktv_subtitle_fetch_ctx_abandon_waiter(ctx);
        ktv_subtitle_fetch_ctx_release(ctx);
        return 0;
    }

    pthread_mutex_lock(&ctx->lock);
    success = ctx->success;
    if (success != 0 && ctx->local_path[0] != '\0') {
        snprintf(out_local_path, out_size, "%s", ctx->local_path);
    }
    ctx->abandoned = 1;
    pthread_mutex_unlock(&ctx->lock);

    if (success == 0 || out_local_path[0] == '\0')
    {
        printf("[KTV] [SUBTITLE_FETCH] request complete but invalid path, song_id=%d\n",
               song_id);
        ktv_subtitle_fetch_ctx_release(ctx);
        return 0;
    }

    ktv_subtitle_fetch_ctx_release(ctx);
    return 1;
}
