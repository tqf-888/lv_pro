#ifndef COVER_MEDIA_LOADER_H
#define COVER_MEDIA_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "image_manager.h"
#include "ktv_ctrl.h"

#define COVER_CATEGORY_ALL (-1)

#ifndef COVER_TITLE_MAX
#define COVER_TITLE_MAX       96U
#endif

#ifndef COVER_DESC_MAX
#define COVER_DESC_MAX        192U
#endif

#ifndef COVER_PLAY_URL_MAX
#define COVER_PLAY_URL_MAX    512U
#endif

#ifndef COVER_CATEGORY_MAX
#define COVER_CATEGORY_MAX    64U
#endif

typedef struct
{
    uint32_t slot_id;
    uint32_t content_id;
    uint32_t generation;
} cover_image_task_ctx_t;

typedef struct
{
    uint32_t content_id;
    char title[COVER_TITLE_MAX];
    char description[COVER_DESC_MAX];
    char play_url[COVER_PLAY_URL_MAX];
    char category[COVER_CATEGORY_MAX];
} cover_slot_meta_info_t;

/* 设置当前位置：-1 = 全部；0 = 第一个分类；1 = 第二个分类…… */
void cover_media_set_category_position(int category_pos);
int  cover_media_get_category_position(void);

/* JSON 第一次解析完成后可拿到真实总数。 */
uint32_t cover_media_get_cached_total_count(void);
int      cover_media_is_total_ready(void);
void     cover_media_reset_runtime_cache(void);

int cover_media_fetch_page(uint32_t page_index, uint32_t page_size, uint32_t generation);
int cover_media_parse_json_file(uint32_t page_index,
                                uint32_t page_size,
                                uint32_t generation,
                                const char *json_path,
                                uint32_t *out_item_count);

/* cover 使用本地随机封面图，不走网络图片下载。 */
int cover_media_fetch_image(uint32_t slot_id,
                            uint32_t content_id,
                            const char *url,
                            uint32_t generation,
                            int high_priority);

void cover_media_page_on_complete(KtvRequest_t *req,
                                  int result,
                                  const void *data,
                                  size_t data_len);

void cover_media_image_on_complete(KtvRequest_t *req,
                                   int result,
                                   const void *data,
                                   size_t data_len);

int cover_media_get_slot_meta(uint32_t slot_id,
                              uint32_t *out_content_id,
                              const char **out_desc_str);

int cover_media_get_slot_full_meta(uint32_t slot_id,
                                   cover_slot_meta_info_t *out_info);

void cover_media_reset_slot_meta(void);

#ifdef __cplusplus
}
#endif

#endif /* COVER_MEDIA_LOADER_H */
