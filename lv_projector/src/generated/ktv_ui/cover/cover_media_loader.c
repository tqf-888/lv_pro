#include "cover_media_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <strings.h>

#include "cJSON.h"

#ifndef COVER_MEDIA_LOG_ENABLE
#define COVER_MEDIA_LOG_ENABLE 1
#endif

#ifndef COVER_MEDIA_LOG_DEBUG
#define COVER_MEDIA_LOG_DEBUG 1
#endif

#ifndef COVER_JSON_URL
#define COVER_JSON_URL "http://media.djyos.com/video_categories.json"
#endif

#ifndef COVER_LOCAL_PIC_DIR
#define COVER_LOCAL_PIC_DIR "/usr/share/lv_projector/pic"
#endif

#ifndef COVER_LOCAL_PIC_MAX
#define COVER_LOCAL_PIC_MAX 20U
#endif

/* UI 一页是 2x4，所以同一页最多显示 8 张封面。
 * 这里按 slot_id 每 8 个为一页，保证同页优先不重复。
 */
#ifndef COVER_UI_PAGE_SIZE
#define COVER_UI_PAGE_SIZE 8U
#endif

#if COVER_MEDIA_LOG_ENABLE
#define COVER_MEDIA_LOGI(fmt, ...) printf("[cover_media] " fmt "\n", ##__VA_ARGS__)
#define COVER_MEDIA_LOGE(fmt, ...) printf("[cover_media][E] " fmt "\n", ##__VA_ARGS__)
#else
#define COVER_MEDIA_LOGI(fmt, ...) ((void)0)
#define COVER_MEDIA_LOGE(fmt, ...) ((void)0)
#endif

#if COVER_MEDIA_LOG_ENABLE && COVER_MEDIA_LOG_DEBUG
#define COVER_MEDIA_LOGD(fmt, ...) printf("[cover_media][D] " fmt "\n", ##__VA_ARGS__)
#else
#define COVER_MEDIA_LOGD(fmt, ...) ((void)0)
#endif

static const KtvReqOps_t g_cover_page_ops = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = cover_media_page_on_complete
};

typedef struct
{
    uint32_t generation;
    uint32_t page_size;
} cover_page_task_ctx_t;

#define COVER_SLOT_META_CAPACITY 4096U

typedef struct
{
    uint8_t valid;
    uint32_t slot_id;
    cover_slot_meta_info_t info;
} cover_slot_meta_t;

static cover_slot_meta_t g_cover_slot_meta[COVER_SLOT_META_CAPACITY];
static int g_cover_category_pos = COVER_CATEGORY_ALL;
static uint32_t g_cover_cached_total = 0U;
static uint8_t g_cover_total_ready = 0U;

static char g_cover_pic_pool[COVER_LOCAL_PIC_MAX][IMG_PATH_MAX];
static uint32_t g_cover_pic_count = 0U;
static uint8_t g_cover_pic_scanned = 0U;

static int cover_media_is_image_file(const char *name)
{
    const char *ext;
    if (name == NULL) return 0;
    ext = strrchr(name, '.');
    if (ext == NULL) return 0;
    return (strcasecmp(ext, ".jpg") == 0 ||
            strcasecmp(ext, ".jpeg") == 0 ||
            strcasecmp(ext, ".png") == 0 ||
            strcasecmp(ext, ".bmp") == 0 ||
            strcasecmp(ext, ".sjpg") == 0 ||
            strcasecmp(ext, ".webp") == 0) ? 1 : 0;
}

static void cover_media_scan_pic_pool(void)
{
    DIR *dir;
    struct dirent *ent;

    if (g_cover_pic_scanned != 0U) return;
    g_cover_pic_scanned = 1U;
    g_cover_pic_count = 0U;
    memset(g_cover_pic_pool, 0, sizeof(g_cover_pic_pool));

    dir = opendir(COVER_LOCAL_PIC_DIR);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL && g_cover_pic_count < COVER_LOCAL_PIC_MAX) {
            int n;
            if (ent->d_name[0] == '.') continue;
            if (!cover_media_is_image_file(ent->d_name)) continue;
            n = snprintf(g_cover_pic_pool[g_cover_pic_count],
                         sizeof(g_cover_pic_pool[g_cover_pic_count]),
                         "%s/%s",
                         COVER_LOCAL_PIC_DIR,
                         ent->d_name);
            if (n > 0 && (size_t)n < sizeof(g_cover_pic_pool[g_cover_pic_count])) {
                g_cover_pic_count++;
            }
        }
        closedir(dir);
    }

    if (g_cover_pic_count == 0U) {
        uint32_t i;
        for (i = 0U; i < COVER_LOCAL_PIC_MAX; ++i) {
            int n = snprintf(g_cover_pic_pool[i], sizeof(g_cover_pic_pool[i]),
                             "%s/%02u.jpg", COVER_LOCAL_PIC_DIR, (unsigned)(i + 1U));
            if (n > 0 && (size_t)n < sizeof(g_cover_pic_pool[i])) {
                g_cover_pic_count++;
            }
        }
        COVER_MEDIA_LOGE("pic dir scan empty, fallback to %s/01.jpg..20.jpg", COVER_LOCAL_PIC_DIR);
    } else {
        COVER_MEDIA_LOGI("pic pool ready: dir=%s count=%u", COVER_LOCAL_PIC_DIR, g_cover_pic_count);
    }
}

static const char *cover_media_pick_local_cover(uint32_t content_id, uint32_t slot_id, uint32_t generation)
{
    uint32_t page_index;
    uint32_t page_pos;
    uint32_t base;
    uint32_t idx;

    (void)content_id;

    cover_media_scan_pic_pool();
    if (g_cover_pic_count == 0U || slot_id == 0U) return NULL;

    /*
     * 要求：同一页不要出现同一张图片。
     * 做法：按 UI 页 8 个 slot 分组，每页先算一个 base，页内位置顺序取图。
     * 当本地图片数量 >= 8 时，同一页 8 张必不重复；
     * 当本地图片数量 < 8 时，资源不够，只能自然重复。
     */
    page_index = (slot_id - 1U) / COVER_UI_PAGE_SIZE;
    page_pos = (slot_id - 1U) % COVER_UI_PAGE_SIZE;

    base = (generation * 97U + page_index * 7U + 3U) % g_cover_pic_count;
    idx = (base + page_pos) % g_cover_pic_count;

    return g_cover_pic_pool[idx];
}

static cover_slot_meta_t *cover_media_find_slot_meta(uint32_t slot_id)
{
    uint32_t i;
    if (slot_id == 0U) return NULL;
    for (i = 0U; i < COVER_SLOT_META_CAPACITY; ++i) {
        if (g_cover_slot_meta[i].valid != 0U && g_cover_slot_meta[i].slot_id == slot_id) {
            return &g_cover_slot_meta[i];
        }
    }
    return NULL;
}

static cover_slot_meta_t *cover_media_alloc_slot_meta(uint32_t slot_id)
{
    uint32_t i;
    cover_slot_meta_t *empty = NULL;
    if (slot_id == 0U) return NULL;
    for (i = 0U; i < COVER_SLOT_META_CAPACITY; ++i) {
        if (g_cover_slot_meta[i].valid != 0U && g_cover_slot_meta[i].slot_id == slot_id) {
            return &g_cover_slot_meta[i];
        }
        if (empty == NULL && g_cover_slot_meta[i].valid == 0U) empty = &g_cover_slot_meta[i];
    }
    if (empty != NULL) {
        memset(empty, 0, sizeof(*empty));
        empty->valid = 1U;
        empty->slot_id = slot_id;
    }
    return empty;
}

static void cover_media_store_slot_meta(uint32_t slot_id,
                                        uint32_t content_id,
                                        const char *title,
                                        const char *description,
                                        const char *play_url,
                                        const char *category)
{
    cover_slot_meta_t *meta = cover_media_alloc_slot_meta(slot_id);
    if (meta == NULL) return;
    memset(&meta->info, 0, sizeof(meta->info));
    meta->info.content_id = content_id;
    if (title != NULL) snprintf(meta->info.title, sizeof(meta->info.title), "%s", title);
    if (description != NULL) snprintf(meta->info.description, sizeof(meta->info.description), "%s", description);
    if (play_url != NULL) snprintf(meta->info.play_url, sizeof(meta->info.play_url), "%s", play_url);
    if (category != NULL) snprintf(meta->info.category, sizeof(meta->info.category), "%s", category);
}

int cover_media_get_slot_meta(uint32_t slot_id, uint32_t *out_content_id, const char **out_desc_str)
{
    cover_slot_meta_t *meta = cover_media_find_slot_meta(slot_id);
    if (out_content_id != NULL) *out_content_id = 0U;
    if (out_desc_str != NULL) *out_desc_str = NULL;
    if (meta == NULL) return -1;
    if (out_content_id != NULL) *out_content_id = meta->info.content_id;
    if (out_desc_str != NULL) {
        if (meta->info.description[0] != '\0') *out_desc_str = meta->info.description;
        else *out_desc_str = meta->info.title;
    }
    return 0;
}

int cover_media_get_slot_full_meta(uint32_t slot_id, cover_slot_meta_info_t *out_info)
{
    cover_slot_meta_t *meta = cover_media_find_slot_meta(slot_id);
    if (out_info != NULL) memset(out_info, 0, sizeof(*out_info));
    if (meta == NULL || out_info == NULL) return -1;
    *out_info = meta->info;
    return 0;
}

void cover_media_reset_slot_meta(void)
{
    memset(g_cover_slot_meta, 0, sizeof(g_cover_slot_meta));
}

void cover_media_set_category_position(int category_pos)
{
    g_cover_category_pos = category_pos;
    cover_media_reset_runtime_cache();
    COVER_MEDIA_LOGI("category position set: %d (-1 means all)", g_cover_category_pos);
}

int cover_media_get_category_position(void)
{
    return g_cover_category_pos;
}

uint32_t cover_media_get_cached_total_count(void)
{
    return g_cover_cached_total;
}

int cover_media_is_total_ready(void)
{
    return g_cover_total_ready ? 1 : 0;
}

void cover_media_reset_runtime_cache(void)
{
    g_cover_cached_total = 0U;
    g_cover_total_ready = 0U;
    cover_media_reset_slot_meta();
}

static int cover_media_build_page_path(uint32_t page_index, uint32_t generation, char *buf, size_t size)
{
    int n;
    if (buf == NULL || size == 0U) return -1;
    n = snprintf(buf, size, "/tmp/cover_categories_p%u_g%u.json", page_index, generation);
    if (n < 0 || (size_t)n >= size) return -2;
    return 0;
}

static char *cover_media_read_file_all(const char *path, long *out_size)
{
    FILE *fp;
    long size;
    char *buf;
    size_t read_len;

    if (out_size != NULL) *out_size = 0;
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
    if (out_size != NULL) *out_size = size;
    return buf;
}

static void cover_media_normalize_http_url(const char *src, char *dst, size_t dst_size)
{
    if (dst == NULL || dst_size == 0U) return;
    dst[0] = '\0';
    if (src == NULL || src[0] == '\0') return;
    if (strncmp(src, "https://", 8) == 0) {
        int n = snprintf(dst, dst_size, "http://%s", src + 8);
        if (n < 0 || (size_t)n >= dst_size) dst[0] = '\0';
        return;
    }
    snprintf(dst, dst_size, "%s", src);
}

static int cover_media_category_selected(int cat_index)
{
    if (g_cover_category_pos == COVER_CATEGORY_ALL) return 1;
    return (cat_index == g_cover_category_pos) ? 1 : 0;
}

static const char *cover_json_string(cJSON *obj, const char *key)
{
    cJSON *v;
    if (obj == NULL || key == NULL) return NULL;
    v = cJSON_GetObjectItemCaseSensitive(obj, key);
    return (cJSON_IsString(v) && v->valuestring != NULL) ? v->valuestring : NULL;
}

static int cover_item_visible(cJSON *item)
{
    cJSON *visible;
    if (!cJSON_IsObject(item)) return 0;
    visible = cJSON_GetObjectItemCaseSensitive(item, "is_visible");
    if (cJSON_IsFalse(visible)) return 0;
    return 1;
}

int cover_media_fetch_page(uint32_t page_index, uint32_t page_size, uint32_t generation)
{
    KtvRequest_t req;
    cover_page_task_ctx_t *ctx;
    int post_ret;

    if (page_size == 0U || generation == 0U) return -1;
    ctx = (cover_page_task_ctx_t *)calloc(1, sizeof(*ctx));
    if (ctx == NULL) return -2;
    ctx->generation = generation;
    ctx->page_size = page_size;

    memset(&req, 0, sizeof(req));
    snprintf(req.url, sizeof(req.url), "%s", COVER_JSON_URL);
    if (cover_media_build_page_path(page_index, generation, req.local_path, sizeof(req.local_path)) != 0) {
        free(ctx);
        return -3;
    }

    req.ops = &g_cover_page_ops;
    req.id = (int)generation;
    req.page_index = (int)page_index;
    req.user_data = ctx;
    req.priority = KTV_PRIORITY_HIGH;

    COVER_MEDIA_LOGI("fetch page json: page=%u gen=%u url=%s path=%s category_pos=%d",
                     page_index, generation, req.url, req.local_path, g_cover_category_pos);

    post_ret = Ktv_Ctrl_PostTask(&req);
    if (post_ret != 0) free(ctx);
    return post_ret;
}

int cover_media_parse_json_file(uint32_t page_index,
                                uint32_t page_size,
                                uint32_t generation,
                                const char *json_path,
                                uint32_t *out_item_count)
{
    char *json_text;
    cJSON *root;
    uint32_t range_start;
    uint32_t range_end;
    uint32_t selected_total = 0U;
    uint32_t added_count = 0U;
    int cat_count;
    int cat_i;

    if (out_item_count != NULL) *out_item_count = 0U;
    if (page_size == 0U || generation == 0U || json_path == NULL || json_path[0] == '\0') return -1;

    json_text = cover_media_read_file_all(json_path, NULL);
    if (json_text == NULL) return -2;

    root = cJSON_Parse(json_text);
    if (root == NULL) { free(json_text); return -3; }
    if (!cJSON_IsArray(root)) {
        cJSON_Delete(root);
        free(json_text);
        return -4;
    }

    range_start = page_index * page_size;
    range_end = range_start + page_size;
    cat_count = cJSON_GetArraySize(root);

    for (cat_i = 0; cat_i < cat_count; ++cat_i) {
        cJSON *cat = cJSON_GetArrayItem(root, cat_i);
        cJSON *list;
        const char *cat_name;
        int item_count;
        int item_i;

        if (!cover_media_category_selected(cat_i)) continue;
        if (!cJSON_IsObject(cat)) continue;
        list = cJSON_GetObjectItemCaseSensitive(cat, "list");
        if (!cJSON_IsArray(list)) continue;
        cat_name = cover_json_string(cat, "category");
        if (cat_name == NULL) cat_name = cover_json_string(cat, "type");
        if (cat_name == NULL) cat_name = "";

        item_count = cJSON_GetArraySize(list);
        for (item_i = 0; item_i < item_count; ++item_i) {
            cJSON *item = cJSON_GetArrayItem(list, item_i);
            cJSON *id_obj;
            uint32_t filtered_index;
            uint32_t slot_id;
            uint32_t content_id;
            const char *title;
            const char *desc;
            const char *play_url_src;
            char play_url[COVER_PLAY_URL_MAX];
            const char *cover_path;
            const char *display_text;

            if (!cover_item_visible(item)) continue;

            filtered_index = selected_total;
            selected_total++;

            if (filtered_index < range_start || filtered_index >= range_end) continue;

            id_obj = cJSON_GetObjectItemCaseSensitive(item, "id");
            if (!cJSON_IsNumber(id_obj)) continue;
            content_id = (uint32_t)id_obj->valuedouble;
            if (content_id == 0U) continue;

            title = cover_json_string(item, "title");
            desc = cover_json_string(item, "description");
            play_url_src = cover_json_string(item, "play_url");
            cover_media_normalize_http_url(play_url_src, play_url, sizeof(play_url));
            cover_path = cover_media_pick_local_cover(content_id, filtered_index + 1U, generation);
            if (cover_path == NULL || cover_path[0] == '\0') continue;

            slot_id = filtered_index + 1U;
            display_text = (desc != NULL && desc[0] != '\0') ? desc : title;
            if (display_text == NULL) display_text = "";

            (void)img_mgr_add_page_item(generation, slot_id, content_id, display_text, cover_path);
            cover_media_store_slot_meta(slot_id, content_id, title, desc, play_url, cat_name);
            added_count++;
        }
    }

    g_cover_cached_total = selected_total;
    g_cover_total_ready = 1U;

    if (out_item_count != NULL) *out_item_count = added_count;

    if (selected_total == 0U || range_end >= selected_total || added_count < page_size) {
        img_mgr_mark_last_page(generation, page_index);
    } else {
        img_mgr_mark_page_ready(generation, page_index);
    }

    COVER_MEDIA_LOGI("page parsed: page=%u add=%u total=%u category_pos=%d",
                     page_index, added_count, selected_total, g_cover_category_pos);

    cJSON_Delete(root);
    free(json_text);
    return 0;
}

int cover_media_fetch_image(uint32_t slot_id,
                            uint32_t content_id,
                            const char *url,
                            uint32_t generation,
                            int high_priority)
{
    (void)high_priority;
    if (slot_id == 0U || content_id == 0U || url == NULL || url[0] == '\0' || generation == 0U) return -1;

    /* 本业务的 url 实际就是本地随机封面路径。直接回填，不下载、不复制、不删原图。 */
    if (img_mgr_set_image_path(generation, slot_id, content_id, url) != 0) {
        COVER_MEDIA_LOGD("local cover dropped: slot=%u content=%u gen=%u path=%s",
                         slot_id, content_id, generation, url);
        return -2;
    }

    COVER_MEDIA_LOGD("local cover ready: slot=%u content=%u gen=%u path=%s",
                     slot_id, content_id, generation, url);
    return 0;
}

void cover_media_page_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    uint32_t item_count = 0U;
    uint32_t page_size = 0U;
    uint32_t generation = 0U;
    cover_page_task_ctx_t *ctx;

    (void)data;
    (void)data_len;
    if (req == NULL) return;

    ctx = (cover_page_task_ctx_t *)req->user_data;
    if (ctx != NULL) {
        page_size = ctx->page_size;
        generation = ctx->generation;
    }
    if (generation == 0U) generation = (uint32_t)req->id;

    if (result != 0) {
        COVER_MEDIA_LOGE("page download failed: page=%d gen=%u", req->page_index, generation);
        free(ctx);
        req->user_data = NULL;
        return;
    }

    if (cover_media_parse_json_file((uint32_t)req->page_index,
                                    page_size,
                                    generation,
                                    req->local_path,
                                    &item_count) != 0) {
        COVER_MEDIA_LOGE("json parse failed: page=%d path=%s", req->page_index, req->local_path);
        free(ctx);
        req->user_data = NULL;
        return;
    }

    COVER_MEDIA_LOGI("page ready: page=%d items=%u gen=%u page_size=%u",
                     req->page_index, item_count, generation, page_size);
    free(ctx);
    req->user_data = NULL;
}

void cover_media_image_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    (void)req;
    (void)result;
    (void)data;
    (void)data_len;
    /* 当前 cover 业务不用网络图片下载。保留这个函数是为了接口和 artist 对齐。 */
}
