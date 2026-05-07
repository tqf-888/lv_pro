#ifndef LV_ARTIST_ADAPTER_H
#define LV_ARTIST_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_artist_catalog.h"
#include "lv_artist_view_style.h"

#ifndef LV_ARTIST_DIRTY_MAX
#define LV_ARTIST_DIRTY_MAX 32
#endif

typedef struct {
    uint32_t slot_id;
    uint32_t artist_id;
    uint32_t content_id;
    uint8_t meta_ready;
    uint8_t path_ready;
    uint8_t query_count;
    char name[128];
    char path[LV_ARTIST_AVATAR_PATH_MAX];
} lv_artist_slot_probe_t;

typedef struct {
    lv_vlist_t *vlist;
    lv_artist_catalog_t catalog;
    const lv_artist_view_style_t *view_style;

    uint32_t total_count;
    uint32_t json_page_size;
    uint32_t ui_batch_size;
    uint32_t debug_last_page_index;
    uint32_t debug_last_visible_start;
    uint32_t hint_first_visible;
    uint8_t has_hint_first_visible;

    uint32_t last_top_index;
    uint32_t last_scroll_tick;
    int8_t scroll_direction;
    uint8_t has_last_top_index;

    uint32_t dirty_artist_ids[LV_ARTIST_DIRTY_MAX];
    uint8_t dirty_artist_count;

    lv_artist_slot_probe_t *probe_slots;
    uint32_t probe_slot_count;
    uint32_t probe_page_start;
    uint32_t probe_page_end;

    pthread_mutex_t pending_lock;
    lv_timer_t *ui_timer;
    uint8_t started;
} lv_artist_adapter_t;

int lv_artist_adapter_start(lv_artist_adapter_t *adapter,
                            const lv_artist_view_style_t *view_style,
                            uint32_t total_count,
                            uint32_t json_page_size);

void lv_artist_adapter_stop(lv_artist_adapter_t *adapter);
lv_vlist_t *lv_artist_adapter_create_vlist(lv_artist_adapter_t *adapter, lv_obj_t *parent);

void lv_artist_adapter_reset(lv_artist_adapter_t *adapter, uint32_t total_count);
void lv_artist_adapter_prime_first_screen(lv_artist_adapter_t *adapter);
bool lv_artist_adapter_get_business_item(lv_artist_adapter_t *adapter, uint32_t item_id, lv_artist_item_t *out);
void lv_artist_adapter_toggle_selected(lv_artist_adapter_t *adapter, uint32_t item_id);

void lv_artist_adapter_notify_avatar_ready_id(uint32_t artist_id);
lv_artist_adapter_t *lv_artist_adapter_get_default(void);

#ifdef __cplusplus
}
#endif

#endif
