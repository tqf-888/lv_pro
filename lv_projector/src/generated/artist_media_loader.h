#ifndef ARTIST_MEDIA_LOADER_H
#define ARTIST_MEDIA_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "image_manager.h"
#include "ktv_ctrl.h"

/*
 * 业务文件职责：
 * 1. 下载 artist 页 JSON 到本地
 * 2. 用 cJSON 解析本地 JSON
 * 3. 把 slot_id / content_id / name / avatar 回填给 image_manager
 * 4. 根据 JSON 条数决定 mark_page_ready / mark_last_page
 * 5. 收到 need_image 通知后，投递单张图片下载任务
 * 6. 图片下载成功后，把本地路径回填给 image_manager
 *
 * 本文件只服务“歌手页”场景，因此命名里保留 artist。
 */

typedef struct
{
    uint32_t slot_id;
    uint32_t content_id;
    uint32_t generation;
} artist_image_task_ctx_t;

/* 页面 JSON 下载 */
int artist_media_fetch_page(uint32_t page_index, uint32_t page_size, uint32_t generation);

/* 本地 JSON 解析并回填给图片管理器 */
int artist_media_parse_json_file(uint32_t page_index,
                                 uint32_t page_size,
                                 uint32_t generation,
                                 const char *json_path,
                                 uint32_t *out_item_count);

/*
 * 收到 image_manager 的轻量 need_image 通知后，调用这个函数去下载图片。
 *
 * high_priority：1 = 这个 slot 落在 UI 当前可见范围内，写到 KtvRequest_t.priority
 *                    后进 http_pool 时插队到 overflow 队首；
 *                0 = 走原本的尾部追加。
 */
int artist_media_fetch_image(uint32_t slot_id,
                             uint32_t content_id,
                             const char *url,
                             uint32_t generation,
                             int high_priority);

/* 建议直接挂到 KtvReqOps_t.on_complete */
void artist_media_page_on_complete(KtvRequest_t *req,
                                   int result,
                                   const void *data,
                                   size_t data_len);

/* 建议直接挂到 KtvReqOps_t.on_complete */
void artist_media_image_on_complete(KtvRequest_t *req,
                                    int result,
                                    const void *data,
                                    size_t data_len);

/* 查询某个 slot 对应的 content_id / name_str。
 * 返回 0 表示命中，负数表示未命中。
 * out_name_str 返回的是内部静态缓存指针，调用者不要修改。
 */
int artist_media_get_slot_meta(uint32_t slot_id,
                               uint32_t *out_content_id,
                               const char **out_name_str);

/* 清空 slot 元数据缓存，通常在 reset/deinit 时调用。 */
void artist_media_reset_slot_meta(void);

#ifdef __cplusplus
}
#endif

#endif /* ARTIST_MEDIA_LOADER_H */
