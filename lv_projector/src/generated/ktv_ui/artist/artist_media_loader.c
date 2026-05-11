#include "artist_media_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ARTIST_MEDIA_LOG_ENABLE
#define ARTIST_MEDIA_LOG_ENABLE 1
#endif

#ifndef ARTIST_MEDIA_LOG_DEBUG
#define ARTIST_MEDIA_LOG_DEBUG 1
#endif

#if ARTIST_MEDIA_LOG_ENABLE
#define ARTIST_MEDIA_LOGI(fmt, ...) printf("[artist_media] " fmt "\n", ##__VA_ARGS__)
#define ARTIST_MEDIA_LOGE(fmt, ...) printf("[artist_media][E] " fmt "\n", ##__VA_ARGS__)
#else
#define ARTIST_MEDIA_LOGI(fmt, ...) ((void)0)
#define ARTIST_MEDIA_LOGE(fmt, ...) ((void)0)
#endif

#if ARTIST_MEDIA_LOG_ENABLE && ARTIST_MEDIA_LOG_DEBUG
#define ARTIST_MEDIA_LOGD(fmt, ...) printf("[artist_media][D] " fmt "\n", ##__VA_ARGS__)
#else
#define ARTIST_MEDIA_LOGD(fmt, ...) ((void)0)
#endif

#include "cJSON.h"
#include "page_manager.h"
#include "ktv.h"

static const KtvReqOps_t g_artist_page_ops = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = artist_media_page_on_complete
};

static const KtvReqOps_t g_artist_image_ops = {
    .type = KTV_REQ_DOWNLOAD_IMAGE,
    .on_complete = artist_media_image_on_complete
};


typedef struct
{
    uint32_t generation;
    uint32_t page_size;
} artist_page_task_ctx_t;

#define ARTIST_SLOT_META_CAPACITY 4096U
#define ARTIST_SLOT_NAME_MAX      128U

typedef struct
{
    uint8_t  valid;
    uint32_t slot_id;
    uint32_t content_id;
    char     name[ARTIST_SLOT_NAME_MAX];
} artist_slot_meta_t;

static artist_slot_meta_t g_artist_slot_meta[ARTIST_SLOT_META_CAPACITY];

static artist_slot_meta_t *artist_media_find_slot_meta(uint32_t slot_id)
{
    uint32_t i;

    if (slot_id == 0U)
    {
        return NULL;
    }

    for (i = 0U; i < ARTIST_SLOT_META_CAPACITY; ++i)
    {
        if (g_artist_slot_meta[i].valid != 0U && g_artist_slot_meta[i].slot_id == slot_id)
        {
            return &g_artist_slot_meta[i];
        }
    }
    return NULL;
}

static artist_slot_meta_t *artist_media_alloc_slot_meta(uint32_t slot_id)
{
    uint32_t i;
    artist_slot_meta_t *empty = NULL;

    if (slot_id == 0U)
    {
        return NULL;
    }

    for (i = 0U; i < ARTIST_SLOT_META_CAPACITY; ++i)
    {
        if (g_artist_slot_meta[i].valid != 0U && g_artist_slot_meta[i].slot_id == slot_id)
        {
            return &g_artist_slot_meta[i];
        }
        if (empty == NULL && g_artist_slot_meta[i].valid == 0U)
        {
            empty = &g_artist_slot_meta[i];
        }
    }

    if (empty != NULL)
    {
        memset(empty, 0, sizeof(*empty));
        empty->valid = 1U;
        empty->slot_id = slot_id;
    }
    return empty;
}

static void artist_media_store_slot_meta(uint32_t slot_id, uint32_t content_id, const char *name_str)
{
    artist_slot_meta_t *meta = artist_media_alloc_slot_meta(slot_id);

    if (meta == NULL)
    {
        return;
    }

    meta->content_id = content_id;
    if (name_str == NULL)
    {
        meta->name[0] = '\0';
    }
    else
    {
        snprintf(meta->name, sizeof(meta->name), "%s", name_str);
    }
}

int artist_media_get_slot_meta(uint32_t slot_id, uint32_t *out_content_id, const char **out_name_str)
{
    artist_slot_meta_t *meta = artist_media_find_slot_meta(slot_id);

    if (out_content_id != NULL)
    {
        *out_content_id = 0U;
    }
    if (out_name_str != NULL)
    {
        *out_name_str = NULL;
    }

    if (meta == NULL)
    {
        return -1;
    }

    if (out_content_id != NULL)
    {
        *out_content_id = meta->content_id;
    }
    if (out_name_str != NULL)
    {
        *out_name_str = meta->name;
    }
    return 0;
}

void artist_media_reset_slot_meta(void)
{
    memset(g_artist_slot_meta, 0, sizeof(g_artist_slot_meta));
}

static int artist_media_build_page_url(uint32_t page_index,
                                       uint32_t page_size,
                                       char *buf,
                                       size_t size)
{
    if (buf == NULL || size == 0U)
    {
        return -1;
    }

    return ktv_build_base_url(KTV_4_4_SEARCH_ARTIST,
                              buf,
                              size,
                              "",
                              "",
                              subpage_get(),
                              IGNORE_NUM,
                              0,
                              name_get(),
                              (int)page_index,
                              (int)page_size);
    //    return ktv_build_base_url(KTV_4_4_SEARCH_ARTIST,
    //                           buf,
    //                           size,
    //                           "",
    //                           "",
    //                           1,
    //                           IGNORE_NUM,
    //                           0,
    //                          "",
    //                           (int)page_index,
    //                           (int)page_size);
}

static int artist_media_build_page_path(uint32_t page_index,
                                        uint32_t generation,
                                        char *buf,
                                        size_t size)
{
    int n;

    if (buf == NULL || size == 0U || generation == 0U)
    {
        return -1;
    }

    n = snprintf(buf, size, "/tmp/artist_page_%u_gen_%u.json", page_index, generation);
    if (n < 0 || (size_t)n >= size)
    {
        return -2;
    }
    return 0;
}

static int artist_media_build_image_path(uint32_t content_id, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0U || content_id == 0U)
    {
        return -1;
    }

    /* 先落 /tmp，避免依赖不存在的外部目录导致下载失败。 */
    n = snprintf(buf, size, "/tmp/artist_img_%u.jpg", content_id);
    if (n < 0 || (size_t)n >= size)
    {
        return -2;
    }
    return 0;
}

static char *artist_media_read_file_all(const char *path, long *out_size)
{
    FILE *fp;
    long size;
    char *buf;
    size_t read_len;

    if (out_size != NULL)
    {
        *out_size = 0;
    }
    if (path == NULL || path[0] == '\0')
    {
        return NULL;
    }

    fp = fopen(path, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        fclose(fp);
        return NULL;
    }
    size = ftell(fp);
    if (size < 0)
    {
        fclose(fp);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        fclose(fp);
        return NULL;
    }

    buf = (char *)malloc((size_t)size + 1U);
    if (buf == NULL)
    {
        fclose(fp);
        return NULL;
    }

    read_len = fread(buf, 1U, (size_t)size, fp);
    fclose(fp);
    if (read_len != (size_t)size)
    {
        free(buf);
        return NULL;
    }

    buf[size] = '\0';
    if (out_size != NULL)
    {
        *out_size = size;
    }
    return buf;
}


static void artist_media_normalize_http_url(const char *src, char *dst, size_t dst_size)
{
    if (dst == NULL || dst_size == 0U) {
        return;
    }
    dst[0] = '\0';
    if (src == NULL || src[0] == '\0') {
        return;
    }
    if (strncmp(src, "https://", 8) == 0) {
        int n = snprintf(dst, dst_size, "http://%s", src + 8);
        if (n < 0 || (size_t)n >= dst_size) {
            dst[0] = '\0';
        }
        return;
    }
    snprintf(dst, dst_size, "%s", src);
}

static uint32_t artist_media_slot_from_offset(uint32_t page_index,
                                              uint32_t page_size,
                                              uint32_t offset_in_page)
{
    return page_index * page_size + offset_in_page + 1U;
}

int artist_media_fetch_page(uint32_t page_index, uint32_t page_size, uint32_t generation)
{
    KtvRequest_t req;
    artist_page_task_ctx_t *ctx = NULL;
    int post_ret;

    if (page_size == 0U || generation == 0U)
    {
        return -1;
    }

    ctx = (artist_page_task_ctx_t *)calloc(1, sizeof(*ctx));
    if (ctx == NULL)
    {
        return -2;
    }
    ctx->generation = generation;
    ctx->page_size = page_size;

    memset(&req, 0, sizeof(req));

    if (artist_media_build_page_url(page_index, page_size, req.url, sizeof(req.url)) != 0)
    {
        free(ctx);
        return -3;
    }
    if (artist_media_build_page_path(page_index, generation, req.local_path, sizeof(req.local_path)) != 0)
    {
        free(ctx);
        return -4;
    }

    req.ops = &g_artist_page_ops;
    req.id = (int)generation;
    req.page_index = (int)page_index;
    req.user_data = ctx;
    /*
     * page JSON 一律走 HIGH。
     *
     * 它的 body 解析后会一次性 add_page_item 给 image_manager，进而触发
     * 这一页 50 张图的 need_image 通知。如果 JSON 自身被淹在图片下载队列后面，
     * 会出现：
     *   t=0     ：image timer 把 8 张可见图标 HIGH 入 overflow
     *   t=...   ：用户继续滑动，更多页 access 触发新的 page JSON（LOW）
     *             和 image（部分 HIGH 部分 LOW）排队
     *   结果    ：本应"先 JSON 再图"的依赖被打乱，前面页 JSON 卡在后面，
     *             前面页的图根本没机会被启动下载。
     *
     * 强制 page JSON HIGH 后，每页的"解锁信号"总会比它产生的图片任务更早
     * 跑完，依赖链不会反转。
     */
    req.priority = KTV_PRIORITY_HIGH;

    post_ret = Ktv_Ctrl_PostTask(&req);
    if (post_ret != 0)
    {
        free(ctx);
    }
    return post_ret;
}

int artist_media_parse_json_file(uint32_t page_index,
                                 uint32_t page_size,
                                 uint32_t generation,
                                 const char *json_path,
                                 uint32_t *out_item_count)
{
    char *json_text = NULL;
    cJSON *root = NULL;
    cJSON *result = NULL;
    cJSON *data = NULL;
    int item_count = 0;
    int i;

    if (out_item_count != NULL)
    {
        *out_item_count = 0U;
    }

    if (page_size == 0U || generation == 0U || json_path == NULL || json_path[0] == '\0')
    {
        return -1;
    }

    json_text = artist_media_read_file_all(json_path, NULL);
    if (json_text == NULL)
    {
        return -2;
    }

    root = cJSON_Parse(json_text);
    if (root == NULL)
    {
        free(json_text);
        return -3;
    }

    result = cJSON_GetObjectItemCaseSensitive(root, "result");
    data = (result != NULL) ? cJSON_GetObjectItemCaseSensitive(result, "data") : NULL;
    if (!cJSON_IsArray(data))
    {
        cJSON_Delete(root);
        free(json_text);
        return -4;
    }

    item_count = cJSON_GetArraySize(data);
    for (i = 0; i < item_count; ++i)
    {
        cJSON *item = cJSON_GetArrayItem(data, i);
        cJSON *id_obj;
        cJSON *name_obj;
        cJSON *avatar_obj;
        uint32_t slot_id;
        uint32_t content_id;
        const char *name_str = NULL;
        const char *avatar_str = NULL;
        char avatar_http_url[512];

        if (!cJSON_IsObject(item))
        {
            continue;
        }

        id_obj = cJSON_GetObjectItemCaseSensitive(item, "id");
        name_obj = cJSON_GetObjectItemCaseSensitive(item, "name");
        avatar_obj = cJSON_GetObjectItemCaseSensitive(item, "avatar");

        if (!cJSON_IsNumber(id_obj) || !cJSON_IsString(avatar_obj) || avatar_obj->valuestring == NULL)
        {
            continue;
        }

        content_id = (uint32_t)id_obj->valuedouble;
        if (content_id == 0U)
        {
            continue;
        }

        if (cJSON_IsString(name_obj) && name_obj->valuestring != NULL)
        {
            name_str = name_obj->valuestring;
        }
        avatar_str = avatar_obj->valuestring;
        artist_media_normalize_http_url(avatar_str, avatar_http_url, sizeof(avatar_http_url));
        if (avatar_http_url[0] == '\0')
        {
            continue;
        }
        slot_id = artist_media_slot_from_offset(page_index, page_size, (uint32_t)i);

        (void)img_mgr_add_page_item(generation,
                                    slot_id,
                                    content_id,
                                    name_str,
                                    avatar_http_url);

        artist_media_store_slot_meta(slot_id, content_id, name_str);
    }

    if (out_item_count != NULL)
    {
        *out_item_count = (uint32_t)item_count;
    }

    if (item_count == 0 || (uint32_t)item_count < page_size)
    {
        img_mgr_mark_last_page(generation, page_index);
    }
    else
    {
        img_mgr_mark_page_ready(generation, page_index);
    }

    cJSON_Delete(root);
    free(json_text);
    return 0;
}

int artist_media_fetch_image(uint32_t slot_id,
                             uint32_t content_id,
                             const char *url,
                             uint32_t generation,
                             int high_priority)
{
    KtvRequest_t req;
    artist_image_task_ctx_t *ctx;

    if (slot_id == 0U || content_id == 0U || url == NULL || url[0] == '\0' || generation == 0U)
    {
        return -1;
    }

    ctx = (artist_image_task_ctx_t *)calloc(1U, sizeof(*ctx));
    if (ctx == NULL)
    {
        return -2;
    }

    ctx->slot_id = slot_id;
    ctx->content_id = content_id;
    ctx->generation = generation;

    memset(&req, 0, sizeof(req));
    artist_media_normalize_http_url(url, req.url, sizeof(req.url));
    if (req.url[0] == '\0')
    {
        free(ctx);
        return -3;
    }

    if (artist_media_build_image_path(content_id, req.local_path, sizeof(req.local_path)) != 0)
    {
        free(ctx);
        return -3;
    }

    req.id = (int)slot_id;
    req.page_index = -1;
    req.user_data = ctx;
    req.ops = &g_artist_image_ops;
    req.priority = high_priority ? KTV_PRIORITY_HIGH : KTV_PRIORITY_LOW;

    ARTIST_MEDIA_LOGD("image queued: slot=%u content=%u gen=%u prio=%s path=%s",
                      slot_id,
                      content_id,
                      generation,
                      high_priority ? "HIGH" : "LOW",
                      req.local_path);

    if (Ktv_Ctrl_PostTask(&req) != 0)
    {
        free(ctx);
        return -4;
    }
    return 0;
}

void artist_media_page_on_complete(KtvRequest_t *req,
                                   int result,
                                   const void *data,
                                   size_t data_len)
{
    uint32_t item_count = 0U;
    uint32_t page_size = 0U;
    uint32_t generation = 0U;
    artist_page_task_ctx_t *ctx = NULL;

    (void)data;
    (void)data_len;

    if (req == NULL)
    {
        return;
    }

    ctx = (artist_page_task_ctx_t *)req->user_data;
    if (ctx != NULL)
    {
        page_size = ctx->page_size;
        generation = ctx->generation;
    }
    if (page_size == 0U)
    {
        ARTIST_MEDIA_LOGE("page ctx missing page_size: page=%d path=%s",
                      req != NULL ? req->page_index : -1,
                      (req != NULL && req->local_path != NULL) ? req->local_path : "");
        return;
    }
    if (generation == 0U)
    {
        generation = (uint32_t)req->id;
    }

    if (result != 0)
    {
        ARTIST_MEDIA_LOGE("page download failed: page=%d gen=%u", req->page_index, generation);
        free(ctx);
        req->user_data = NULL;
        return;
    }

    if (artist_media_parse_json_file((uint32_t)req->page_index,
                                     page_size,
                                     generation,
                                     req->local_path,
                                     &item_count) != 0)
    {
        ARTIST_MEDIA_LOGE("json parse failed: page=%d page_size=%u path=%s",
                      req->page_index,
                      page_size,
                      req->local_path);
        free(ctx);
        req->user_data = NULL;
        return;
    }

    ARTIST_MEDIA_LOGI("page ready: page=%d items=%u gen=%u page_size=%u",
                      req->page_index,
                      item_count,
                      generation,
                      page_size);

    free(ctx);
    req->user_data = NULL;
}

void artist_media_image_on_complete(KtvRequest_t *req,
                                    int result,
                                    const void *data,
                                    size_t data_len)
{
    artist_image_task_ctx_t *ctx;
    const char *path = (const char *)data;

    (void)data_len;

    if (req == NULL)
    {
        return;
    }

    ctx = (artist_image_task_ctx_t *)req->user_data;
    if (ctx == NULL)
    {
        return;
    }

    if (result == 0)
    {
        if (path == NULL || path[0] == '\0')
        {
            path = req->local_path;
        }
        if (img_mgr_set_image_path(ctx->generation,
                                   ctx->slot_id,
                                   ctx->content_id,
                                   path) != 0)
        {
            /* 图片回来时 slot 可能已经被 reset/淘汰/换代。管理器接不住时，
             * 下载文件就没有 owner，必须立刻删掉，避免 /tmp 留孤儿图。 */
            (void)remove(path);
            ARTIST_MEDIA_LOGD("image dropped: slot=%u content=%u gen=%u path=%s",
                          ctx->slot_id,
                          ctx->content_id,
                          ctx->generation,
                          path);
        }
        else
        {
            ARTIST_MEDIA_LOGD("image download ok: slot=%u content=%u gen=%u",
                          ctx->slot_id,
                          ctx->content_id,
                          ctx->generation);
        }
    }
    else
    {
        ARTIST_MEDIA_LOGE("image download failed: slot=%u content=%u gen=%u",
                      ctx->slot_id,
                      ctx->content_id,
                      ctx->generation);
    }

    free(ctx);
    req->user_data = NULL;
}
