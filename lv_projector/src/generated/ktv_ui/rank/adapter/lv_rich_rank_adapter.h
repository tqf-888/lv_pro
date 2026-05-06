#ifndef LV_RICH_RANK_ADAPTER_H
#define LV_RICH_RANK_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_rich_song_catalog.h"
#include "lv_rich_song_view_style.h"

/*
 * 排行榜业务复用 song 的 catalog / renderer / style：
 * - 复用 lv_rich_song_item_t 作为通用行数据承载结构
 * - 复用现有 renderer，不再重复造 UI 轮子
 * - 只在 adapter 层替换“请求函数 + JSON 解析规则”
 */
typedef struct {
    lv_vlist_t *vlist;
    lv_rich_song_catalog_t catalog;
    const lv_rich_song_view_style_t *view_style;

    uint32_t total_count;
    uint32_t batch_size;
    uint32_t batch_count;

    uint8_t end_reached;
    uint32_t last_batch_index;

    uint8_t *batch_ready;
    uint8_t *batch_loading;

    uint8_t *pending_batch_valid;
    uint8_t *pending_batch_success;
    uint32_t *pending_batch_epoch;
    uint8_t has_pending;

    pthread_mutex_t pending_lock;
    lv_timer_t *ui_timer;
    uint8_t started;
    uint32_t request_epoch;
} lv_rich_rank_adapter_t;

int lv_rich_rank_adapter_start(lv_rich_rank_adapter_t *adapter,
                               const lv_rich_song_view_style_t *view_style,
                               uint32_t total_count,
                               uint32_t batch_size);

void lv_rich_rank_adapter_stop(lv_rich_rank_adapter_t *adapter);

lv_vlist_t *lv_rich_rank_adapter_create_vlist(lv_rich_rank_adapter_t *adapter, lv_obj_t *parent);

void lv_rich_rank_adapter_reset(lv_rich_rank_adapter_t *adapter, uint32_t total_count);

bool lv_rich_rank_adapter_get_business_item(lv_rich_rank_adapter_t *adapter,
                                            uint32_t item_id,
                                            lv_rich_song_item_t *out);

void lv_rich_rank_adapter_toggle_selected(lv_rich_rank_adapter_t *adapter, uint32_t item_id);

void lv_rich_rank_adapter_notify_batch_ready_with_epoch(lv_rich_rank_adapter_t *adapter,
                                                        uint32_t batch_index,
                                                        bool success,
                                                        uint32_t request_epoch);

lv_rich_rank_adapter_t *lv_rich_rank_adapter_get_default(void);
uint32_t lv_rich_rank_adapter_get_request_epoch(const lv_rich_rank_adapter_t *adapter);
uint32_t lv_rich_rank_adapter_get_default_request_epoch(void);

void lv_rich_rank_adapter_notify_default_batch_ready_with_epoch(uint32_t batch_index,
                                                                bool success,
                                                                uint32_t request_epoch);

/* 直接复用用户现有的排行榜取数函数，不再新写 fetch 文件 */
extern void fetch_song_by_rank_list_4_15(int page);

#ifdef __cplusplus
}
#endif

#endif
