#include "http_mq.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Replace POSIX mqueue with in-process blocking queue.
 *
 * Why:
 * - Avoid kernel mqueue limits/config coupling.
 * - Avoid mq_open/mq_timedsend fragile failures on embedded systems.
 * - Keep API unchanged for http_pool.c (send/recv/count/clear).
 *
 * WARNING:
 * - Queue stores raw bytes in ring-buffer slots.
 * - Pointer fields inside copied structs remain pointer-value copies only.
 * - Pointee lifetime must be guaranteed by upper layers.
 */

struct http_mq {
    char name[64];
    uint32_t max_msgs;
    uint32_t msg_size;
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    int closed;
    unsigned char *buf;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};

static void make_abs_timespec_after_ms(struct timespec *ts, long timeout_ms)
{
    clock_gettime(CLOCK_REALTIME, ts);

    ts->tv_nsec += (timeout_ms % 1000) * 1000000L;
    ts->tv_sec += timeout_ms / 1000;

    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000L;
    }
}

http_mq_t *http_mq_create(const char *name, uint32_t max_msgs, uint32_t msg_size)
{
    http_mq_t *mq;

    if (!name || max_msgs == 0U || msg_size == 0U) {
        return NULL;
    }

    mq = (http_mq_t *)calloc(1, sizeof(http_mq_t));
    if (!mq) {
        return NULL;
    }

    snprintf(mq->name, sizeof(mq->name), "%s", name);
    mq->max_msgs = max_msgs;
    mq->msg_size = msg_size;
    mq->head = 0U;
    mq->tail = 0U;
    mq->count = 0U;
    mq->closed = 0;

    mq->buf = (unsigned char *)calloc((size_t)max_msgs, (size_t)msg_size);
    if (mq->buf == NULL) {
        free(mq);
        return NULL;
    }

    if (pthread_mutex_init(&mq->lock, NULL) != 0) {
        free(mq->buf);
        free(mq);
        return NULL;
    }
    if (pthread_cond_init(&mq->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&mq->lock);
        free(mq->buf);
        free(mq);
        return NULL;
    }
    if (pthread_cond_init(&mq->not_full, NULL) != 0) {
        pthread_cond_destroy(&mq->not_empty);
        pthread_mutex_destroy(&mq->lock);
        free(mq->buf);
        free(mq);
        return NULL;
    }

    return mq;
}

void http_mq_destroy(http_mq_t *mq)
{
    if (!mq) {
        return;
    }

    pthread_mutex_lock(&mq->lock);
    mq->closed = 1;
    pthread_cond_broadcast(&mq->not_empty);
    pthread_cond_broadcast(&mq->not_full);
    pthread_mutex_unlock(&mq->lock);

    pthread_cond_destroy(&mq->not_empty);
    pthread_cond_destroy(&mq->not_full);
    pthread_mutex_destroy(&mq->lock);
    free(mq->buf);
    free(mq);
}

http_err_t http_mq_send(http_mq_t *mq, const void *msg, uint32_t size)
{
    if (!mq || !msg) {
        return HTTP_ERR_INVALID_PARAM;
    }
    if (size == 0U || size > mq->msg_size) {
        return HTTP_ERR_INVALID_PARAM;
    }

    pthread_mutex_lock(&mq->lock);
    while (!mq->closed && mq->count >= mq->max_msgs) {
        struct timespec ts;
        make_abs_timespec_after_ms(&ts, 120);
        if (pthread_cond_timedwait(&mq->not_full, &mq->lock, &ts) == ETIMEDOUT) {
            pthread_mutex_unlock(&mq->lock);
            return HTTP_ERR_QUEUE_FULL;
        }
    }

    if (mq->closed) {
        pthread_mutex_unlock(&mq->lock);
        return HTTP_ERR_NOT_INIT;
    }

    memcpy(mq->buf + ((size_t)mq->tail * mq->msg_size), msg, size);
    mq->tail = (mq->tail + 1U) % mq->max_msgs;
    mq->count++;
    pthread_cond_signal(&mq->not_empty);
    pthread_mutex_unlock(&mq->lock);

    return HTTP_OK;
}

http_err_t http_mq_recv(http_mq_t *mq, void *msg, uint32_t size)
{
    if (!mq || !msg) {
        return HTTP_ERR_INVALID_PARAM;
    }
    if (size < mq->msg_size) {
        return HTTP_ERR_INVALID_PARAM;
    }

    pthread_mutex_lock(&mq->lock);
    while (!mq->closed && mq->count == 0U) {
        pthread_cond_wait(&mq->not_empty, &mq->lock);
    }

    if (mq->closed) {
        pthread_mutex_unlock(&mq->lock);
        return HTTP_ERR_NOT_INIT;
    }

    memcpy(msg, mq->buf + ((size_t)mq->head * mq->msg_size), mq->msg_size);
    mq->head = (mq->head + 1U) % mq->max_msgs;
    mq->count--;
    pthread_cond_signal(&mq->not_full);
    pthread_mutex_unlock(&mq->lock);

    return HTTP_OK;
}

uint32_t http_mq_count(http_mq_t *mq)
{
    uint32_t n;
    if (!mq) {
        return 0U;
    }

    pthread_mutex_lock(&mq->lock);
    n = mq->count;
    pthread_mutex_unlock(&mq->lock);
    return n;
}

void http_mq_clear(http_mq_t *mq)
{
    if (!mq) {
        return;
    }

    pthread_mutex_lock(&mq->lock);
    mq->head = 0U;
    mq->tail = 0U;
    mq->count = 0U;
    pthread_cond_broadcast(&mq->not_full);
    pthread_mutex_unlock(&mq->lock);
}

