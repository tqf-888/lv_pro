#ifndef LV_SONGSHEET_ADAPTER_H
#define LV_SONGSHEET_ADAPTER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "lv_virtual_list.h"
#include "lv_songsheet_view_style.h"
#include "lv_data_catalog.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    lv_obj_t *parent;
    lv_vlist_t *vlist;
    const lv_songsheet_view_style_t *view_style;
    const lv_vlist_renderer_ops_t *renderer_ops;
    const void *renderer_style;
    const void *placeholder_img_src;
    const void *failed_img_src;
    lv_data_catalog_t catalog;
    lv_timer_t *ui_timer;
    pthread_mutex_t pending_lock;
    uint32_t total_count;
    uint32_t json_batch_size;
    uint32_t json_batch_count;
    uint8_t *json_batch_state;          /* 0 idle, 1 loading, 2 ready */
    uint8_t *pending_batch_valid;
    uint8_t *pending_batch_success;
    uint8_t *pending_image_valid;
    uint8_t *pending_image_success;
    uint8_t has_pending;
    uint8_t started;
} lv_songsheet_adapter_t;

int lv_songsheet_adapter_start(lv_songsheet_adapter_t *adapter,
                               lv_obj_t *parent,
                               const lv_songsheet_view_style_t *view_style,
                               uint32_t total_count,
                               uint32_t json_batch_size,
                               const void *placeholder_img_src,
                               const void *failed_img_src,
                               const lv_vlist_renderer_ops_t *renderer_ops,
                               const void *renderer_style);

void lv_songsheet_adapter_stop(lv_songsheet_adapter_t *adapter);

lv_songsheet_adapter_t *lv_songsheet_adapter_get_default(void);
void lv_songsheet_adapter_notify_page_ready(lv_songsheet_adapter_t *adapter,
                                            uint32_t page_index,
                                            bool success);
void lv_songsheet_adapter_notify_image_ready(lv_songsheet_adapter_t *adapter,
                                             uint32_t item_id,
                                             bool success);
void lv_songsheet_adapter_notify_default_page_ready(uint32_t page_index, bool success);
void lv_songsheet_adapter_notify_default_image_ready(uint32_t item_id, bool success);

/* 外部业务实现 */
extern void fetch_song_sheet(int page_index);
extern void fetch_song_sheet_img(int id);

#ifdef __cplusplus
}
#endif

#endif
