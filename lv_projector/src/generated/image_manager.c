#include "image_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef pthread_mutex_t img_mutex_t;

typedef struct
{
    uint8_t in_use;
    uint8_t page_requested;
    uint8_t page_ready;
} img_page_slot_t;

typedef struct
{
    uint8_t in_use;
    uint8_t need_image_sent;
    uint32_t slot_id;
    uint32_t content_id;
    uint32_t generation;
    uint32_t last_touch;
    img_state_t state;
    char name[IMG_NAME_MAX];
    char url[IMG_URL_MAX];
    char local_path[IMG_PATH_MAX];
} img_entry_t;

typedef struct
{
    img_mgr_cfg_t cfg;
    img_mutex_t lock;
    uint8_t inited;

    uint32_t generation;
    uint32_t touch_tick;
    uint32_t focus_slot_id;

    uint32_t page_size;
    uint32_t max_cached_items;
    uint32_t max_cached_pages;

    uint8_t has_more;
    uint32_t last_page_index;

    /* UI 当前可见 slot 范围（含端点）。落在 [visible_start, visible_end] 内的
     * slot 在触发 need_image 时会被标记为高优先级。0/0 = 没设置，全走 LOW。 */
    uint32_t visible_start;
    uint32_t visible_end;

    img_entry_t entries[IMG_MAX_ENTRIES];
    img_page_slot_t pages[IMG_MAX_PAGES];
} img_mgr_ctx_t;

static img_mgr_ctx_t g_img_mgr;
static __thread char g_pull_tls_path[IMG_PATH_MAX];

static uint32_t img_slot_to_page(uint32_t slot_id)
{
    if (g_img_mgr.page_size == 0U || slot_id == 0U)
    {
        return 0U;
    }
    return (slot_id - 1U) / g_img_mgr.page_size;
}

static uint32_t img_item_count_limit(void)
{
    if (g_img_mgr.max_cached_items == 0U || g_img_mgr.max_cached_items > IMG_MAX_ENTRIES)
    {
        return IMG_MAX_ENTRIES;
    }
    return g_img_mgr.max_cached_items;
}

static uint32_t img_page_count_limit(void)
{
    if (g_img_mgr.max_cached_pages == 0U || g_img_mgr.max_cached_pages > IMG_MAX_PAGES)
    {
        return IMG_MAX_PAGES;
    }
    return g_img_mgr.max_cached_pages;
}

static void img_remove_file_locked(const char *path)
{
    if (path == NULL || path[0] == '\0')
    {
        return;
    }

    if (g_img_mgr.cfg.remove_file != NULL)
    {
        (void)g_img_mgr.cfg.remove_file(g_img_mgr.cfg.user_ctx, path);
        return;
    }

    (void)remove(path);
}

static void img_reset_entry_locked(img_entry_t *e)
{
    if (e == NULL)
    {
        return;
    }
    memset(e, 0, sizeof(*e));
}

static img_entry_t *img_find_entry_locked(uint32_t slot_id)
{
    uint32_t i;
    uint32_t limit = img_item_count_limit();

    for (i = 0; i < limit; ++i)
    {
        if (g_img_mgr.entries[i].in_use && g_img_mgr.entries[i].slot_id == slot_id)
        {
            return &g_img_mgr.entries[i];
        }
    }
    return NULL;
}

static uint32_t img_distance(uint32_t a, uint32_t b)
{
    return (a > b) ? (a - b) : (b - a);
}

static img_entry_t *img_alloc_entry_locked(void)
{
    uint32_t i;
    uint32_t limit = img_item_count_limit();
    img_entry_t *victim = NULL;
    uint32_t best_dist = 0U;

    for (i = 0; i < limit; ++i)
    {
        img_entry_t *e = &g_img_mgr.entries[i];
        uint32_t dist;

        if (!e->in_use)
        {
            return e;
        }

        dist = img_distance(e->slot_id, g_img_mgr.focus_slot_id);
        if (victim == NULL || dist > best_dist ||
            (dist == best_dist && e->last_touch < victim->last_touch))
        {
            victim = e;
            best_dist = dist;
        }
    }

    if (victim != NULL)
    {
        img_remove_file_locked(victim->local_path);
        img_reset_entry_locked(victim);
    }
    return victim;
}

static img_page_slot_t *img_get_page_locked(uint32_t page_index)
{
    uint32_t limit = img_page_count_limit();

    if (page_index >= limit)
    {
        return NULL;
    }
    return &g_img_mgr.pages[page_index];
}

/*
 * 注意：调用方必须在持锁状态外调用，因为 user 的 need_image 回调里通常
 * 会去做 http 提交、文件系统操作甚至重新调回 img_mgr_*。
 *
 * 这里读 visible_start/visible_end 没加锁是有意：set_visible_range 写它们
 * 时也只是单字赋值，最坏情况是临界期内分类错一次（HIGH→LOW 或反过来），
 * 不会破坏数据结构。把 visible 决策放在通知点而不是 access 路径，是为了
 * 让 add_page_item 在 JSON 回来时也能正确分类首次 need_image。
 */
static int img_slot_in_visible_range(uint32_t slot_id)
{
    uint32_t s = g_img_mgr.visible_start;
    uint32_t e = g_img_mgr.visible_end;
    if (s == 0U || e == 0U || e < s) return 0;
    return (slot_id >= s && slot_id <= e) ? 1 : 0;
}

static void img_notify_need_image_snapshot(uint32_t slot_id,
                                           uint32_t content_id,
                                           const char *url,
                                           uint32_t generation)
{
    if (g_img_mgr.cfg.need_image != NULL)
    {
        int high = img_slot_in_visible_range(slot_id);
        g_img_mgr.cfg.need_image(g_img_mgr.cfg.user_ctx,
                                 slot_id,
                                 content_id,
                                 url,
                                 generation,
                                 high);
    }
}

static void img_access_page_locked(uint32_t page_index)
{
    img_page_slot_t *page;
    uint32_t generation;
    uint32_t page_size;

    if (!g_img_mgr.inited || g_img_mgr.cfg.request_page == NULL)
    {
        return;
    }

    if (g_img_mgr.has_more == 0U && page_index > g_img_mgr.last_page_index)
    {
        return;
    }

    page = img_get_page_locked(page_index);
    if (page == NULL)
    {
        return;
    }
    if (page->page_requested || page->page_ready)
    {
        return;
    }

    page->in_use = 1U;
    page->page_requested = 1U;
    generation = g_img_mgr.generation;
    page_size = g_img_mgr.page_size;

    pthread_mutex_unlock(&g_img_mgr.lock);
    (void)g_img_mgr.cfg.request_page(g_img_mgr.cfg.user_ctx, page_index, page_size, generation);
    pthread_mutex_lock(&g_img_mgr.lock);
}

static void img_touch_entry_locked(img_entry_t *e)
{
    if (e == NULL)
    {
        return;
    }
    ++g_img_mgr.touch_tick;
    e->last_touch = g_img_mgr.touch_tick;
}

int img_mgr_init(const img_mgr_cfg_t *cfg)
{
    if (cfg == NULL || cfg->page_size == 0U)
    {
        return -1;
    }

    if (g_img_mgr.inited)
    {
        img_mgr_deinit();
    }

    memset(&g_img_mgr, 0, sizeof(g_img_mgr));
    if (pthread_mutex_init(&g_img_mgr.lock, NULL) != 0)
    {
        memset(&g_img_mgr, 0, sizeof(g_img_mgr));
        return -2;
    }

    memcpy(&g_img_mgr.cfg, cfg, sizeof(*cfg));
    g_img_mgr.page_size = cfg->page_size;
    g_img_mgr.max_cached_items = cfg->max_cached_items;
    g_img_mgr.max_cached_pages = cfg->max_cached_pages;
    g_img_mgr.generation = 1U;
    g_img_mgr.has_more = 1U;
    g_img_mgr.last_page_index = 0U;
    g_img_mgr.inited = 1U;
    return 0;
}

void img_mgr_deinit(void)
{
    uint32_t i;
    uint32_t generation = 0U;
    int need_cleanup = 0;

    if (!g_img_mgr.inited)
    {
        return;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    generation = g_img_mgr.generation;
    need_cleanup = (g_img_mgr.cfg.cleanup_session != NULL);

    for (i = 0; i < img_item_count_limit(); ++i)
    {
        if (g_img_mgr.entries[i].in_use)
        {
            img_remove_file_locked(g_img_mgr.entries[i].local_path);
            img_reset_entry_locked(&g_img_mgr.entries[i]);
        }
    }

    g_img_mgr.inited = 0U;
    pthread_mutex_unlock(&g_img_mgr.lock);

    if (need_cleanup)
    {
        g_img_mgr.cfg.cleanup_session(g_img_mgr.cfg.user_ctx, generation);
    }

    pthread_mutex_destroy(&g_img_mgr.lock);
    memset(&g_img_mgr, 0, sizeof(g_img_mgr));
}

void img_mgr_reset(void)
{
    uint32_t i;
    uint32_t old_generation;
    int need_cleanup = 0;

    if (!g_img_mgr.inited)
    {
        return;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    old_generation = g_img_mgr.generation;
    need_cleanup = (g_img_mgr.cfg.cleanup_session != NULL);

    for (i = 0; i < img_item_count_limit(); ++i)
    {
        if (g_img_mgr.entries[i].in_use)
        {
            img_remove_file_locked(g_img_mgr.entries[i].local_path);
            img_reset_entry_locked(&g_img_mgr.entries[i]);
        }
    }

    memset(g_img_mgr.pages, 0, sizeof(g_img_mgr.pages));
    g_img_mgr.generation++;
    if (g_img_mgr.generation == 0U)
    {
        g_img_mgr.generation = 1U;
    }
    g_img_mgr.touch_tick = 0U;
    g_img_mgr.focus_slot_id = 0U;
    g_img_mgr.has_more = 1U;
    g_img_mgr.last_page_index = 0U;
    pthread_mutex_unlock(&g_img_mgr.lock);

    if (need_cleanup)
    {
        g_img_mgr.cfg.cleanup_session(g_img_mgr.cfg.user_ctx, old_generation);
    }
}

int img_mgr_is_inited(void)
{
    return g_img_mgr.inited ? 1 : 0;
}

uint32_t img_mgr_get_generation(void)
{
    uint32_t generation = 0U;

    if (!g_img_mgr.inited)
    {
        return 0U;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    generation = g_img_mgr.generation;
    pthread_mutex_unlock(&g_img_mgr.lock);
    return generation;
}

void img_mgr_access(uint32_t slot_id)
{
    uint32_t page_index;
    img_entry_t *entry;
    int need_notify = 0;
    uint32_t content_id = 0U;
    uint32_t generation = 0U;
    char url[IMG_URL_MAX];

    if (!g_img_mgr.inited || slot_id == 0U)
    {
        return;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    g_img_mgr.focus_slot_id = slot_id;
    page_index = img_slot_to_page(slot_id);
    img_access_page_locked(page_index);

    entry = img_find_entry_locked(slot_id);
    if (entry != NULL)
    {
        img_touch_entry_locked(entry);
        if (entry->state == IMG_STATE_WAIT_IMAGE && !entry->need_image_sent && entry->url[0] != '\0')
        {
            entry->need_image_sent = 1U;
            need_notify = 1;
            content_id = entry->content_id;
            generation = entry->generation;
            strncpy(url, entry->url, sizeof(url) - 1U);
            url[sizeof(url) - 1U] = '\0';
        }
    }
    pthread_mutex_unlock(&g_img_mgr.lock);

    if (need_notify)
    {
        img_notify_need_image_snapshot(slot_id, content_id, url, generation);
    }
}

void img_mgr_access_range(uint32_t start_slot_id, uint32_t end_slot_id)
{
    uint32_t slot_id;
    uint32_t begin;
    uint32_t end;

    if (start_slot_id == 0U || end_slot_id == 0U)
    {
        return;
    }

    begin = (start_slot_id < end_slot_id) ? start_slot_id : end_slot_id;
    end = (start_slot_id < end_slot_id) ? end_slot_id : start_slot_id;

    for (slot_id = begin; slot_id <= end; ++slot_id)
    {
        img_mgr_access(slot_id);
        if (slot_id == UINT32_MAX)
        {
            break;
        }
    }
}

void img_mgr_set_visible_range(uint32_t start_slot_id, uint32_t end_slot_id)
{
    if (!g_img_mgr.inited)
    {
        return;
    }

    /*
     * 只是给 need_image 通知做"是否高优"的二分判定，不参与下载提交、不参与
     * 缓存淘汰、不影响 focus_slot_id（focus 仍由 access 维护）。所以这里
     * 用最简单的方案：直接覆盖。读侧无锁，竞态最坏只是分类错一次。
     */
    pthread_mutex_lock(&g_img_mgr.lock);
    if (start_slot_id == 0U || end_slot_id == 0U || end_slot_id < start_slot_id)
    {
        g_img_mgr.visible_start = 0U;
        g_img_mgr.visible_end = 0U;
    }
    else
    {
        g_img_mgr.visible_start = start_slot_id;
        g_img_mgr.visible_end = end_slot_id;
    }
    pthread_mutex_unlock(&g_img_mgr.lock);
}

const char *img_mgr_pull_tls(uint32_t slot_id)
{
    if (img_mgr_pull_copy(slot_id, g_pull_tls_path, sizeof(g_pull_tls_path)) != 0)
    {
        return NULL;
    }
    return g_pull_tls_path;
}

int img_mgr_pull_copy(uint32_t slot_id, char *buf, size_t buf_size)
{
    img_entry_t *entry;

    if (buf == NULL || buf_size == 0U || !g_img_mgr.inited)
    {
        return -1;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    entry = img_find_entry_locked(slot_id);
    if (entry == NULL || entry->state != IMG_STATE_READY || entry->local_path[0] == '\0')
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        buf[0] = '\0';
        return -2;
    }

    strncpy(buf, entry->local_path, buf_size - 1U);
    buf[buf_size - 1U] = '\0';
    img_touch_entry_locked(entry);
    pthread_mutex_unlock(&g_img_mgr.lock);
    return 0;
}

img_state_t img_mgr_get_state(uint32_t slot_id)
{
    img_entry_t *entry;
    img_state_t state = IMG_STATE_EMPTY;

    if (!g_img_mgr.inited)
    {
        return IMG_STATE_EMPTY;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    entry = img_find_entry_locked(slot_id);
    if (entry != NULL)
    {
        state = entry->state;
    }
    pthread_mutex_unlock(&g_img_mgr.lock);
    return state;
}

int img_mgr_get_item_info(uint32_t slot_id, img_item_info_t *out_info)
{
    img_entry_t *entry;

    if (out_info == NULL || !g_img_mgr.inited)
    {
        return -1;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    entry = img_find_entry_locked(slot_id);
    if (entry == NULL)
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        memset(out_info, 0, sizeof(*out_info));
        return -2;
    }

    memset(out_info, 0, sizeof(*out_info));
    out_info->slot_id = entry->slot_id;
    out_info->content_id = entry->content_id;
    out_info->state = entry->state;
    strncpy(out_info->name, entry->name, sizeof(out_info->name) - 1U);
    strncpy(out_info->url, entry->url, sizeof(out_info->url) - 1U);
    strncpy(out_info->local_path, entry->local_path, sizeof(out_info->local_path) - 1U);
    img_touch_entry_locked(entry);
    pthread_mutex_unlock(&g_img_mgr.lock);
    return 0;
}

int img_mgr_add_page_item(uint32_t generation,
                          uint32_t slot_id,
                          uint32_t content_id,
                          const char *name,
                          const char *url)
{
    img_entry_t *entry;
    int need_notify = 0;
    char notify_url[IMG_URL_MAX];

    if (!g_img_mgr.inited || generation == 0U || slot_id == 0U || content_id == 0U || url == NULL || url[0] == '\0')
    {
        return -1;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    if (generation != g_img_mgr.generation)
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        return -2;
    }

    entry = img_find_entry_locked(slot_id);
    if (entry == NULL)
    {
        entry = img_alloc_entry_locked();
        if (entry == NULL)
        {
            pthread_mutex_unlock(&g_img_mgr.lock);
            return -3;
        }
    }
    else if (entry->content_id != content_id)
    {
        img_remove_file_locked(entry->local_path);
        img_reset_entry_locked(entry);
    }

    entry->in_use = 1U;
    entry->slot_id = slot_id;
    entry->content_id = content_id;
    entry->generation = generation;
    entry->state = IMG_STATE_WAIT_IMAGE;
    entry->local_path[0] = '\0';
    entry->url[0] = '\0';
    entry->name[0] = '\0';
    entry->need_image_sent = 0U;
    if (name != NULL)
    {
        strncpy(entry->name, name, sizeof(entry->name) - 1U);
    }
    strncpy(entry->url, url, sizeof(entry->url) - 1U);
    img_touch_entry_locked(entry);

    if (!entry->need_image_sent)
    {
        entry->need_image_sent = 1U;
        need_notify = 1;
        strncpy(notify_url, entry->url, sizeof(notify_url) - 1U);
        notify_url[sizeof(notify_url) - 1U] = '\0';
    }

    pthread_mutex_unlock(&g_img_mgr.lock);

    if (need_notify)
    {
        img_notify_need_image_snapshot(slot_id, content_id, notify_url, generation);
    }
    return 0;
}

int img_mgr_set_image_path(uint32_t generation,
                           uint32_t slot_id,
                           uint32_t content_id,
                           const char *local_path)
{
    img_entry_t *entry;

    if (!g_img_mgr.inited || generation == 0U || slot_id == 0U || content_id == 0U || local_path == NULL || local_path[0] == '\0')
    {
        return -1;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    if (generation != g_img_mgr.generation)
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        return -2;
    }

    entry = img_find_entry_locked(slot_id);
    if (entry == NULL || !entry->in_use)
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        return -3;
    }
    if (entry->content_id != content_id)
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        return -4;
    }

    strncpy(entry->local_path, local_path, sizeof(entry->local_path) - 1U);
    entry->local_path[sizeof(entry->local_path) - 1U] = '\0';
    entry->state = IMG_STATE_READY;
    img_touch_entry_locked(entry);
    pthread_mutex_unlock(&g_img_mgr.lock);
    return 0;
}

void img_mgr_mark_page_ready(uint32_t generation, uint32_t page_index)
{
    img_page_slot_t *page;

    if (!g_img_mgr.inited)
    {
        return;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    if (generation != g_img_mgr.generation)
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        return;
    }

    page = img_get_page_locked(page_index);
    if (page != NULL)
    {
        page->in_use = 1U;
        page->page_requested = 1U;
        page->page_ready = 1U;
    }
    pthread_mutex_unlock(&g_img_mgr.lock);
}

void img_mgr_mark_last_page(uint32_t generation, uint32_t page_index)
{
    img_page_slot_t *page;

    if (!g_img_mgr.inited)
    {
        return;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    if (generation != g_img_mgr.generation)
    {
        pthread_mutex_unlock(&g_img_mgr.lock);
        return;
    }

    g_img_mgr.has_more = 0U;
    g_img_mgr.last_page_index = page_index;

    page = img_get_page_locked(page_index);
    if (page != NULL)
    {
        page->in_use = 1U;
        page->page_requested = 1U;
        page->page_ready = 1U;
    }
    pthread_mutex_unlock(&g_img_mgr.lock);
}

int img_mgr_is_last(void)
{
    int ret;

    if (!g_img_mgr.inited)
    {
        return 0;
    }

    pthread_mutex_lock(&g_img_mgr.lock);
    ret = g_img_mgr.has_more ? 1 : 0;
    pthread_mutex_unlock(&g_img_mgr.lock);
    return ret;
}
