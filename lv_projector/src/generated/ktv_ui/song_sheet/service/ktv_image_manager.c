#include "ktv_image_manager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#ifndef KTV_IMG_UNUSED
#define KTV_IMG_UNUSED(x) ((void)(x))
#endif

#if defined(_WIN32)
typedef CRITICAL_SECTION ktv_img_mutex_t;
static ktv_img_mutex_t *ktv_img_mutex_create(void)
{
    ktv_img_mutex_t *m = (ktv_img_mutex_t *)malloc(sizeof(*m));
    if (m == NULL) return NULL;
    InitializeCriticalSection(m);
    return m;
}
static void ktv_img_mutex_destroy(ktv_img_mutex_t *m)
{
    if (m == NULL) return;
    DeleteCriticalSection(m);
    free(m);
}
static void ktv_img_mutex_lock(ktv_img_mutex_t *m)
{
    if (m == NULL) return;
    EnterCriticalSection(m);
}
static void ktv_img_mutex_unlock(ktv_img_mutex_t *m)
{
    if (m == NULL) return;
    LeaveCriticalSection(m);
}
#else
typedef pthread_mutex_t ktv_img_mutex_t;
static ktv_img_mutex_t *ktv_img_mutex_create(void)
{
    ktv_img_mutex_t *m = (ktv_img_mutex_t *)malloc(sizeof(*m));
    if (m == NULL) return NULL;
    if (pthread_mutex_init(m, NULL) != 0)
    {
        free(m);
        return NULL;
    }
    return m;
}
static void ktv_img_mutex_destroy(ktv_img_mutex_t *m)
{
    if (m == NULL) return;
    pthread_mutex_destroy(m);
    free(m);
}
static void ktv_img_mutex_lock(ktv_img_mutex_t *m)
{
    if (m == NULL) return;
    pthread_mutex_lock(m);
}
static void ktv_img_mutex_unlock(ktv_img_mutex_t *m)
{
    if (m == NULL) return;
    pthread_mutex_unlock(m);
}
#endif

static ktv_img_entry_t *ktv_img_find_entry_locked(ktv_img_mgr_t *mgr, uint32_t id)
{
    uint32_t i;
    for (i = 0; i < KTV_IMG_MAX_ENTRIES; ++i)
    {
        if (mgr->entries[i].in_use && mgr->entries[i].id == id)
        {
            return &mgr->entries[i];
        }
    }
    return NULL;
}

static ktv_img_entry_t *ktv_img_alloc_entry_locked(ktv_img_mgr_t *mgr)
{
    uint32_t i;
    for (i = 0; i < KTV_IMG_MAX_ENTRIES; ++i)
    {
        if (!mgr->entries[i].in_use)
        {
            memset(&mgr->entries[i], 0, sizeof(mgr->entries[i]));
            mgr->entries[i].in_use = 1;
            mgr->entries[i].state = KTV_IMG_STATE_EMPTY;
            return &mgr->entries[i];
        }
    }
    return NULL;
}

static void ktv_img_reset_entry_locked(ktv_img_entry_t *e)
{
    if (e == NULL) return;
    memset(e, 0, sizeof(*e));
}

static int ktv_img_send_request_snapshot(ktv_img_mgr_t *mgr,
                                         uint32_t id,
                                         const char *url,
                                         const char *save_path)
{
    if (mgr == NULL || mgr->cfg.send_request == NULL)
    {
        return -1;
    }
    return mgr->cfg.send_request(mgr->cfg.user_ctx, id, url, save_path);
}

static int ktv_img_remove_file(ktv_img_mgr_t *mgr, const char *path)
{
    if (path == NULL || path[0] == '\0')
    {
        return 0;
    }

    if (mgr != NULL && mgr->cfg.remove_file != NULL)
    {
        return mgr->cfg.remove_file(mgr->cfg.user_ctx, path);
    }

    return remove(path);
}

int ktv_img_mgr_init(ktv_img_mgr_t *mgr, const ktv_img_mgr_cfg_t *cfg)
{
    if (mgr == NULL || cfg == NULL)
    {
        return -1;
    }
    if (cfg->make_save_path == NULL || cfg->send_request == NULL)
    {
        return -2;
    }

    memset(mgr, 0, sizeof(*mgr));
    memcpy(&mgr->cfg, cfg, sizeof(*cfg));

    mgr->lock = (void *)ktv_img_mutex_create();
    if (mgr->lock == NULL)
    {
        memset(mgr, 0, sizeof(*mgr));
        return -3;
    }

    mgr->started = 1;
    return 0;
}

void ktv_img_mgr_deinit(ktv_img_mgr_t *mgr)
{
    uint32_t i;
    ktv_img_mutex_t *lock;

    if (mgr == NULL || !mgr->started)
    {
        return;
    }

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);

    if (mgr->cfg.cancel_request != NULL)
    {
        for (i = 0; i < KTV_IMG_MAX_ENTRIES; ++i)
        {
            if (mgr->entries[i].in_use && mgr->entries[i].state == KTV_IMG_STATE_DOWNLOADING)
            {
                mgr->cfg.cancel_request(mgr->cfg.user_ctx, mgr->entries[i].id);
            }
        }
    }

    for (i = 0; i < KTV_IMG_MAX_ENTRIES; ++i)
    {
        if (mgr->entries[i].in_use)
        {
            ktv_img_remove_file(mgr, mgr->entries[i].save_path);
            ktv_img_reset_entry_locked(&mgr->entries[i]);
        }
    }

    ktv_img_mutex_unlock(lock);

    if (mgr->cfg.cleanup_session != NULL)
    {
        mgr->cfg.cleanup_session(mgr->cfg.user_ctx);
    }

    ktv_img_mutex_destroy(lock);
    memset(mgr, 0, sizeof(*mgr));
}

int ktv_img_mgr_push(ktv_img_mgr_t *mgr, uint32_t id, const char *url)
{
    ktv_img_mutex_t *lock;
    ktv_img_entry_t *e;
    char save_path[KTV_IMG_PATH_MAX];
    int ret;

    if (mgr == NULL || !mgr->started)
    {
        return KTV_IMG_PUSH_NOT_INIT;
    }
    if (url == NULL || url[0] == '\0')
    {
        return KTV_IMG_PUSH_INVALID_ARG;
    }

    lock = (ktv_img_mutex_t *)mgr->lock;
    memset(save_path, 0, sizeof(save_path));

    ktv_img_mutex_lock(lock);

    e = ktv_img_find_entry_locked(mgr, id);
    if (e != NULL)
    {
        if (strcmp(e->url, url) != 0)
        {
            ktv_img_mutex_unlock(lock);
            return KTV_IMG_PUSH_URL_MISMATCH;
        }
        ktv_img_mutex_unlock(lock);
        return KTV_IMG_PUSH_ALREADY_EXISTS;
    }

    e = ktv_img_alloc_entry_locked(mgr);
    if (e == NULL)
    {
        ktv_img_mutex_unlock(lock);
        return KTV_IMG_PUSH_NO_SPACE;
    }

    ret = mgr->cfg.make_save_path(mgr->cfg.user_ctx, id, save_path, sizeof(save_path));
    if (ret != 0 || save_path[0] == '\0')
    {
        ktv_img_reset_entry_locked(e);
        ktv_img_mutex_unlock(lock);
        return KTV_IMG_PUSH_PATH_FAIL;
    }

    e->id = id;
    e->max_retry = mgr->cfg.max_retry;
    e->state = KTV_IMG_STATE_DOWNLOADING;
    e->request_sent = 1;
    strncpy(e->url, url, sizeof(e->url) - 1);
    e->url[sizeof(e->url) - 1] = '\0';
    strncpy(e->save_path, save_path, sizeof(e->save_path) - 1);
    e->save_path[sizeof(e->save_path) - 1] = '\0';

    ktv_img_mutex_unlock(lock);

    ret = ktv_img_send_request_snapshot(mgr, id, url, save_path);
    if (ret != 0)
    {
        ktv_img_mutex_lock(lock);
        e = ktv_img_find_entry_locked(mgr, id);
        if (e != NULL)
        {
            e->state = KTV_IMG_STATE_FAILED;
            e->request_sent = 0;
        }
        ktv_img_mutex_unlock(lock);
        return KTV_IMG_PUSH_SEND_FAIL;
    }

    return KTV_IMG_PUSH_OK;
}

int ktv_img_mgr_pull_copy(ktv_img_mgr_t *mgr, uint32_t id, char *buf, size_t buf_size)
{
    ktv_img_mutex_t *lock;
    ktv_img_entry_t *e;

    if (buf != NULL && buf_size > 0)
    {
        buf[0] = '\0';
    }

    if (mgr == NULL || !mgr->started || buf == NULL || buf_size == 0)
    {
        return -1;
    }

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);

    e = ktv_img_find_entry_locked(mgr, id);
    if (e == NULL || e->state != KTV_IMG_STATE_READY)
    {
        ktv_img_mutex_unlock(lock);
        return -2;
    }

    strncpy(buf, e->save_path, buf_size - 1);
    buf[buf_size - 1] = '\0';

    ktv_img_mutex_unlock(lock);
    return 0;
}

const char *ktv_img_mgr_pull_tls(ktv_img_mgr_t *mgr, uint32_t id)
{
#if defined(_MSC_VER)
    static __declspec(thread) char tls_buf[KTV_IMG_PATH_MAX];
#else
    static __thread char tls_buf[KTV_IMG_PATH_MAX];
#endif
    if (ktv_img_mgr_pull_copy(mgr, id, tls_buf, sizeof(tls_buf)) == 0)
    {
        return tls_buf;
    }
    return NULL;
}

void ktv_img_mgr_notify_ok(ktv_img_mgr_t *mgr, uint32_t id)
{
    ktv_img_mutex_t *lock;
    ktv_img_entry_t *e;

    if (mgr == NULL || !mgr->started)
    {
        return;
    }

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);

    e = ktv_img_find_entry_locked(mgr, id);
    if (e != NULL)
    {
        e->state = KTV_IMG_STATE_READY;
        e->request_sent = 1;

        // printf("[img_mgr] notify_ok: id=%u state=%d retry=%u path=%s url=%s\n",
        //        (unsigned)e->id,
        //        (int)e->state,
        //        (unsigned)e->retry_count,
        //        e->save_path,
        //        e->url);
    }
    else
    {
        printf("[img_mgr] notify_ok: id=%u not found\n", (unsigned)id);
    }

    ktv_img_mutex_unlock(lock);
}

void ktv_img_mgr_notify_fail(ktv_img_mgr_t *mgr, uint32_t id)
{
    ktv_img_mutex_t *lock;
    ktv_img_entry_t *e;
    uint32_t snap_id = 0;
    char snap_url[KTV_IMG_URL_MAX];
    char snap_path[KTV_IMG_PATH_MAX];
    int need_retry = 0;
    int ret;

    if (mgr == NULL || !mgr->started)
    {
        return;
    }

    memset(snap_url, 0, sizeof(snap_url));
    memset(snap_path, 0, sizeof(snap_path));

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);

    e = ktv_img_find_entry_locked(mgr, id);
    if (e != NULL)
    {
        if (e->retry_count < e->max_retry)
        {
            e->retry_count++;
            e->state = KTV_IMG_STATE_DOWNLOADING;
            e->request_sent = 1;

            snap_id = e->id;
            strncpy(snap_url, e->url, sizeof(snap_url) - 1);
            strncpy(snap_path, e->save_path, sizeof(snap_path) - 1);
            need_retry = 1;
        }
        else
        {
            e->state = KTV_IMG_STATE_FAILED;
            e->request_sent = 0;
        }
    }

    ktv_img_mutex_unlock(lock);

    if (!need_retry)
    {
        return;
    }

    ret = ktv_img_send_request_snapshot(mgr, snap_id, snap_url, snap_path);
    if (ret != 0)
    {
        ktv_img_mutex_lock(lock);
        e = ktv_img_find_entry_locked(mgr, id);
        if (e != NULL)
        {
            e->state = KTV_IMG_STATE_FAILED;
            e->request_sent = 0;
        }
        ktv_img_mutex_unlock(lock);
    }
}

ktv_img_state_t ktv_img_mgr_get_state(ktv_img_mgr_t *mgr, uint32_t id)
{
    ktv_img_mutex_t *lock;
    ktv_img_entry_t *e;
    ktv_img_state_t state = KTV_IMG_STATE_EMPTY;

    if (mgr == NULL || !mgr->started)
    {
        return KTV_IMG_STATE_EMPTY;
    }

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);

    e = ktv_img_find_entry_locked(mgr, id);
    if (e != NULL)
    {
        state = e->state;
    }

    ktv_img_mutex_unlock(lock);
    return state;
}

int ktv_img_mgr_has(ktv_img_mgr_t *mgr, uint32_t id)
{
    ktv_img_mutex_t *lock;
    int has = 0;

    if (mgr == NULL || !mgr->started)
    {
        return 0;
    }

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);
    has = (ktv_img_find_entry_locked(mgr, id) != NULL) ? 1 : 0;
    ktv_img_mutex_unlock(lock);
    return has;
}

int ktv_img_mgr_retry(ktv_img_mgr_t *mgr, uint32_t id)
{
    ktv_img_mutex_t *lock;
    ktv_img_entry_t *e;
    uint32_t snap_id;
    char snap_url[KTV_IMG_URL_MAX];
    char snap_path[KTV_IMG_PATH_MAX];
    int ret;

    if (mgr == NULL || !mgr->started)
    {
        return -1;
    }

    memset(snap_url, 0, sizeof(snap_url));
    memset(snap_path, 0, sizeof(snap_path));

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);

    e = ktv_img_find_entry_locked(mgr, id);
    if (e == NULL)
    {
        ktv_img_mutex_unlock(lock);
        return -2;
    }
    if (e->state != KTV_IMG_STATE_FAILED)
    {
        ktv_img_mutex_unlock(lock);
        return -3;
    }

    e->retry_count = 0;
    e->state = KTV_IMG_STATE_DOWNLOADING;
    e->request_sent = 1;

    snap_id = e->id;
    strncpy(snap_url, e->url, sizeof(snap_url) - 1);
    strncpy(snap_path, e->save_path, sizeof(snap_path) - 1);

    ktv_img_mutex_unlock(lock);

    ret = ktv_img_send_request_snapshot(mgr, snap_id, snap_url, snap_path);
    if (ret != 0)
    {
        ktv_img_mutex_lock(lock);
        e = ktv_img_find_entry_locked(mgr, id);
        if (e != NULL)
        {
            e->state = KTV_IMG_STATE_FAILED;
            e->request_sent = 0;
        }
        ktv_img_mutex_unlock(lock);
        return -4;
    }

    return 0;
}

void ktv_img_mgr_remove(ktv_img_mgr_t *mgr, uint32_t id)
{
    ktv_img_mutex_t *lock;
    ktv_img_entry_t snap;
    ktv_img_entry_t *e;

    if (mgr == NULL || !mgr->started)
    {
        return;
    }

    memset(&snap, 0, sizeof(snap));

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);

    e = ktv_img_find_entry_locked(mgr, id);
    if (e != NULL)
    {
        snap = *e;
        ktv_img_reset_entry_locked(e);
    }

    ktv_img_mutex_unlock(lock);

    if (!snap.in_use)
    {
        return;
    }

    if (mgr->cfg.cancel_request != NULL && snap.state == KTV_IMG_STATE_DOWNLOADING)
    {
        mgr->cfg.cancel_request(mgr->cfg.user_ctx, snap.id);
    }

    ktv_img_remove_file(mgr, snap.save_path);
}

void ktv_img_mgr_clear(ktv_img_mgr_t *mgr)
{
    uint32_t ids[KTV_IMG_MAX_ENTRIES];
    uint32_t cnt = 0;
    uint32_t i;
    ktv_img_mutex_t *lock;

    if (mgr == NULL || !mgr->started)
    {
        return;
    }

    lock = (ktv_img_mutex_t *)mgr->lock;
    ktv_img_mutex_lock(lock);
    for (i = 0; i < KTV_IMG_MAX_ENTRIES; ++i)
    {
        if (mgr->entries[i].in_use)
        {
            ids[cnt++] = mgr->entries[i].id;
        }
    }
    ktv_img_mutex_unlock(lock);

    for (i = 0; i < cnt; ++i)
    {
        ktv_img_mgr_remove(mgr, ids[i]);
    }

    if (mgr->cfg.cleanup_session != NULL)
    {
        mgr->cfg.cleanup_session(mgr->cfg.user_ctx);
    }
}
