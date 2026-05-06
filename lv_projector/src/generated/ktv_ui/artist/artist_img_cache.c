#include "artist_img_cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cJSON.h"
#include "http_common.h"
#include "ktv.h"
#include "ktv_ctrl.h"
#include "lv_artist_adapter.h"

#define ARTIST_URL_CACHE_MAX 4096U
#define ARTIST_URL_LEN_MAX 512U

typedef struct {
    uint8_t has_url;
    uint8_t page_loading;
    uint8_t wanted;
    char url[ARTIST_URL_LEN_MAX];
} artist_url_cache_entry_t;

static ktv_img_mgr_t g_artist_img_mgr;
static int g_artist_img_mgr_inited = 0;
static uint32_t g_artist_page_size = 50U;
static artist_url_cache_entry_t g_artist_url_cache[ARTIST_URL_CACHE_MAX];

static void artist_img_fix_url(const char *src, char *dst, size_t dst_size)
{
    if (dst == NULL || dst_size == 0U) return;
    dst[0] = '\0';
    if (src == NULL || src[0] == '\0') return;
    if (strncmp(src, "https://", 8) == 0) {
        snprintf(dst, dst_size, "http://%s", src + 8);
        return;
    }
    snprintf(dst, dst_size, "%s", src);
}

static int artist_img_make_save_path(void *user_ctx, uint32_t id, char *buf, size_t buf_size)
{
    (void)user_ctx;
    if (buf == NULL || buf_size == 0U) return -1;
    if (snprintf(buf, buf_size, "/tmp/artist_img_%u.jpg", id) >= (int)buf_size) return -1;
    return 0;
}

static int artist_img_build_page_url(uint32_t page_index, uint32_t page_size, char *buf, size_t size)
{
    char raw_url[2048];
    if (buf == NULL || size == 0U) return -1;
    memset(raw_url, 0, sizeof(raw_url));
    if (ktv_build_base_url(KTV_4_4_SEARCH_ARTIST,
                           raw_url,
                           (int)sizeof(raw_url),
                           "",
                           "",
                           1,
                           IGNORE_NUM,
                           0,
                           "",
                           (int)page_index,
                           (int)page_size) != 0) {
        return -2;
    }
    artist_img_fix_url(raw_url, buf, size);
    return (buf[0] != '\0') ? 0 : -3;
}

static int artist_img_build_page_path(uint32_t page_index, char *buf, size_t size)
{
    if (buf == NULL || size == 0U) return -1;
    if (snprintf(buf, size, "/tmp/artist_page_%u.json", page_index) >= (int)size) return -1;
    return 0;
}

static void artist_img_notify_ready(uint32_t artist_id)
{
    ktv_img_mgr_notify_ok(&g_artist_img_mgr, artist_id);
    lv_artist_adapter_notify_avatar_ready_id(artist_id);
}

static void artist_img_notify_failed(uint32_t artist_id)
{
    ktv_img_mgr_notify_fail(&g_artist_img_mgr, artist_id);
    lv_artist_adapter_notify_avatar_ready_id(artist_id);
}

static void artist_img_req_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    (void)data;
    (void)data_len;
    if (req == NULL) return;
    if (result == 0) artist_img_notify_ready((uint32_t)req->id);
    else artist_img_notify_failed((uint32_t)req->id);
}

static const KtvReqOps_t g_artist_img_download_ops = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = artist_img_req_on_complete
};

static int artist_img_send_request(void *user_ctx, uint32_t id, const char *url, const char *save_path)
{
    KtvRequest_t req;
    char fixed_url[KTV_IMG_URL_MAX];
    (void)user_ctx;
    if (url == NULL || url[0] == '\0' || save_path == NULL || save_path[0] == '\0') return -1;
    memset(&req, 0, sizeof(req));
    memset(fixed_url, 0, sizeof(fixed_url));
    artist_img_fix_url(url, fixed_url, sizeof(fixed_url));
    if (fixed_url[0] == '\0') return -1;
    snprintf(req.url, sizeof(req.url), "%s", fixed_url);
    snprintf(req.local_path, sizeof(req.local_path), "%s", save_path);
    req.id = (int)id;
    req.page_index = 0;
    req.ops = &g_artist_img_download_ops;
    req.user_data = NULL;
    {
        int post_ret = Ktv_Ctrl_PostTask(&req);
        printf("[artist_img] post image: id=%u ret=%d url=%s\n", id, post_ret, fixed_url);
        return post_ret;
    }
}

static int artist_img_push_by_id(uint32_t artist_id)
{
    char save_path[256];
    artist_url_cache_entry_t *e;

    if (artist_id == 0U || artist_id >= ARTIST_URL_CACHE_MAX) return -1;
    e = &g_artist_url_cache[artist_id];
    if (!e->has_url || e->url[0] == '\0') return -1;

    if (artist_img_make_save_path(NULL, artist_id, save_path, sizeof(save_path)) == 0 &&
        access(save_path, F_OK) == 0) {
        artist_img_notify_ready(artist_id);
        return KTV_IMG_PUSH_ALREADY_EXISTS;
    }

    {
        int push_ret = ktv_img_mgr_push(&g_artist_img_mgr, artist_id, e->url);
        printf("[artist_img] push image: id=%u ret=%d\n", artist_id, push_ret);
        return push_ret;
    }
}

static char *artist_img_read_file_all(const char *path)
{
    FILE *fp;
    long size;
    char *buf;
    size_t read_len;

    if (path == NULL || path[0] == '\0') return NULL;
    fp = fopen(path, "rb");
    if (fp == NULL) return NULL;
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    size = ftell(fp);
    if (size < 0) { fclose(fp); return NULL; }
    if (fseek(fp, 0, SEEK_SET) != 0) { fclose(fp); return NULL; }
    buf = (char *)malloc((size_t)size + 1U);
    if (buf == NULL) { fclose(fp); return NULL; }
    read_len = fread(buf, 1U, (size_t)size, fp);
    fclose(fp);
    if (read_len != (size_t)size) { free(buf); return NULL; }
    buf[size] = '\0';
    return buf;
}

static void artist_img_handle_page_json(uint32_t page_index, const char *json_path)
{
    char *json_text = NULL;
    cJSON *root = NULL;
    cJSON *result = NULL;
    cJSON *data = NULL;
    int item_count;
    int i;
    uint32_t start_id;
    uint32_t end_id;

    json_text = artist_img_read_file_all(json_path);
    if (json_text == NULL) return;
    root = cJSON_Parse(json_text);
    if (root == NULL) goto done;
    result = cJSON_GetObjectItemCaseSensitive(root, "result");
    data = (result != NULL) ? cJSON_GetObjectItemCaseSensitive(result, "data") : NULL;
    if (!cJSON_IsArray(data)) goto done;

    item_count = cJSON_GetArraySize(data);
    for (i = 0; i < item_count; ++i) {
        cJSON *item = cJSON_GetArrayItem(data, i);
        cJSON *id_obj;
        cJSON *avatar_obj;
        uint32_t artist_id;
        char fixed_url[ARTIST_URL_LEN_MAX];

        if (!cJSON_IsObject(item)) continue;
        id_obj = cJSON_GetObjectItemCaseSensitive(item, "id");
        avatar_obj = cJSON_GetObjectItemCaseSensitive(item, "avatar");
        if (!cJSON_IsNumber(id_obj) || !cJSON_IsString(avatar_obj) || avatar_obj->valuestring == NULL) continue;
        artist_id = (uint32_t)id_obj->valuedouble;
        if (artist_id == 0U || artist_id >= ARTIST_URL_CACHE_MAX) continue;
        memset(fixed_url, 0, sizeof(fixed_url));
        artist_img_fix_url(avatar_obj->valuestring, fixed_url, sizeof(fixed_url));
        if (fixed_url[0] == '\0') continue;
        g_artist_url_cache[artist_id].has_url = 1U;
        snprintf(g_artist_url_cache[artist_id].url, sizeof(g_artist_url_cache[artist_id].url), "%s", fixed_url);
        if (g_artist_url_cache[artist_id].wanted) {
            printf("[artist_img] page match: artist_id=%u page=%u\n", artist_id, page_index);
            g_artist_url_cache[artist_id].wanted = 0U;
            if (artist_img_push_by_id(artist_id) < 0) {
                artist_img_notify_failed(artist_id);
            }
        }
    }

    start_id = page_index * g_artist_page_size + 1U;
    end_id = start_id + g_artist_page_size - 1U;
    if (end_id >= ARTIST_URL_CACHE_MAX) end_id = ARTIST_URL_CACHE_MAX - 1U;
    for (i = (int)start_id; i <= (int)end_id; ++i) {
        g_artist_url_cache[i].page_loading = 0U;
        if (g_artist_url_cache[i].wanted && !g_artist_url_cache[i].has_url) {
            g_artist_url_cache[i].wanted = 0U;
            artist_img_notify_failed((uint32_t)i);
        }
    }

done:
    if (root != NULL) cJSON_Delete(root);
    free(json_text);
}

static void artist_img_page_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    uint32_t page_index;
    (void)data;
    (void)data_len;
    if (req == NULL) return;
    page_index = (uint32_t)req->page_index;
    if (result != 0) {
        printf("[artist_img] page failed: page=%u result=%d\n", page_index, result);
        uint32_t start_id = page_index * g_artist_page_size + 1U;
        uint32_t end_id = start_id + g_artist_page_size - 1U;
        uint32_t i;
        if (end_id >= ARTIST_URL_CACHE_MAX) end_id = ARTIST_URL_CACHE_MAX - 1U;
        for (i = start_id; i <= end_id; ++i) {
            g_artist_url_cache[i].page_loading = 0U;
            if (g_artist_url_cache[i].wanted) {
                g_artist_url_cache[i].wanted = 0U;
                artist_img_notify_failed(i);
            }
        }
        return;
    }
    printf("[artist_img] page ok: page=%u path=%s\n", page_index, req->local_path);
    artist_img_handle_page_json(page_index, req->local_path);
}

static const KtvReqOps_t g_artist_page_ops = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = artist_img_page_on_complete
};

static int artist_img_request_page_for_id(uint32_t artist_id)
{
    KtvRequest_t req;
    uint32_t page_index;
    uint32_t start_id;
    uint32_t end_id;
    uint32_t i;

    if (artist_id == 0U || artist_id >= ARTIST_URL_CACHE_MAX) return -1;
    page_index = (artist_id - 1U) / g_artist_page_size;
    start_id = page_index * g_artist_page_size + 1U;
    end_id = start_id + g_artist_page_size - 1U;
    if (end_id >= ARTIST_URL_CACHE_MAX) end_id = ARTIST_URL_CACHE_MAX - 1U;

    for (i = start_id; i <= end_id; ++i) {
        if (g_artist_url_cache[i].page_loading) return KTV_IMG_PUSH_OK;
    }
    for (i = start_id; i <= end_id; ++i) {
        g_artist_url_cache[i].page_loading = 1U;
    }

    memset(&req, 0, sizeof(req));
    if (artist_img_build_page_url(page_index, g_artist_page_size, req.url, sizeof(req.url)) != 0) return -1;
    if (artist_img_build_page_path(page_index, req.local_path, sizeof(req.local_path)) != 0) return -1;
    req.id = (int)artist_id;
    req.page_index = (int)page_index;
    req.ops = &g_artist_page_ops;
    req.user_data = NULL;
    {
        int post_ret = Ktv_Ctrl_PostTask(&req);
        printf("[artist_img] request page: id=%u page=%u ret=%d\n", artist_id, page_index, post_ret);
        return post_ret;
    }
}

int artist_img_cache_init(void)
{
    ktv_img_mgr_cfg_t cfg;
    if (g_artist_img_mgr_inited) return 0;
    memset(&cfg, 0, sizeof(cfg));
    memset(g_artist_url_cache, 0, sizeof(g_artist_url_cache));
    cfg.max_retry = 1;
    cfg.make_save_path = artist_img_make_save_path;
    cfg.send_request = artist_img_send_request;
    cfg.cancel_request = NULL;
    cfg.remove_file = NULL;
    cfg.cleanup_session = NULL;
    cfg.user_ctx = NULL;
    if (ktv_img_mgr_init(&g_artist_img_mgr, &cfg) != 0) return -1;
    g_artist_img_mgr_inited = 1;
    return 0;
}

void artist_img_cache_deinit(void)
{
    if (!g_artist_img_mgr_inited) return;
    ktv_img_mgr_deinit(&g_artist_img_mgr);
    g_artist_img_mgr_inited = 0;
    memset(g_artist_url_cache, 0, sizeof(g_artist_url_cache));
}

void artist_img_cache_clear(void)
{
    if (!g_artist_img_mgr_inited) return;
    ktv_img_mgr_clear(&g_artist_img_mgr);
    memset(g_artist_url_cache, 0, sizeof(g_artist_url_cache));
}

void artist_img_cache_set_page_size(uint32_t page_size)
{
    (void)page_size;
    g_artist_page_size = 50U;
}

int artist_img_cache_prefetch(uint32_t artist_id)
{
    char save_path[256];
    int ret;

    if (!g_artist_img_mgr_inited || artist_id == 0U || artist_id >= ARTIST_URL_CACHE_MAX) return -1;
    if (artist_img_make_save_path(NULL, artist_id, save_path, sizeof(save_path)) == 0 &&
        access(save_path, F_OK) == 0) {
        artist_img_notify_ready(artist_id);
        return KTV_IMG_PUSH_ALREADY_EXISTS;
    }
    if (g_artist_url_cache[artist_id].has_url) {
        ret = artist_img_push_by_id(artist_id);
        if (ret >= 0) return ret;
    }
    g_artist_url_cache[artist_id].wanted = 1U;
    return artist_img_request_page_for_id(artist_id);
}

ktv_img_state_t artist_img_cache_state(uint32_t artist_id)
{
    if (!g_artist_img_mgr_inited) return KTV_IMG_STATE_EMPTY;
    if (artist_id > 0U && artist_id < ARTIST_URL_CACHE_MAX) {
        char save_path[256];
        if (artist_img_make_save_path(NULL, artist_id, save_path, sizeof(save_path)) == 0 &&
            access(save_path, F_OK) == 0) {
            return KTV_IMG_STATE_READY;
        }
    }
    return ktv_img_mgr_get_state(&g_artist_img_mgr, artist_id);
}

const char *artist_img_cache_pull_tls(uint32_t artist_id)
{
    static __thread char fallback_path[256];
    const char *path;
    if (!g_artist_img_mgr_inited) return NULL;
    path = ktv_img_mgr_pull_tls(&g_artist_img_mgr, artist_id);
    if (path != NULL && path[0] != '\0') return path;
    memset(fallback_path, 0, sizeof(fallback_path));
    if (artist_img_make_save_path(NULL, artist_id, fallback_path, sizeof(fallback_path)) == 0 &&
        access(fallback_path, F_OK) == 0) {
        return fallback_path;
    }
    return NULL;
}
