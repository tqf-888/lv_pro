#ifndef LV_DATA_CATALOG_H
#define LV_DATA_CATALOG_H

#include <stdbool.h>
#include <stdint.h>
#include "lv_virtual_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LV_DC_TITLE_MAX    64
#define LV_DC_SUBTITLE_MAX 96
#define LV_DC_URL_MAX      256
#define LV_DC_PATH_MAX     160

typedef struct {
    uint16_t id;
    uint8_t data_ready;
    uint8_t image_requested;
    lv_vlist_img_state_t img_state;
    char title[LV_DC_TITLE_MAX];
    char subtitle[LV_DC_SUBTITLE_MAX];
    char pic_url[LV_DC_URL_MAX];
    char img_display_path[LV_DC_PATH_MAX];
    char img_local_path[LV_DC_PATH_MAX];
    uint8_t image_cache_marked;
    uint32_t touch_seq;
} lv_dc_item_t;

typedef struct {
    uint32_t total_items;
    lv_dc_item_t *items;
    uint32_t touch_seq_counter;
} lv_data_catalog_t;

bool lv_dc_init(lv_data_catalog_t *catalog, uint32_t total_items);
void lv_dc_deinit(lv_data_catalog_t *catalog);
uint32_t lv_dc_total_items(const lv_data_catalog_t *catalog);
bool lv_dc_is_item_ready(const lv_data_catalog_t *catalog, uint32_t id);
bool lv_dc_set_item(lv_data_catalog_t *catalog, uint32_t id, const char *title, const char *subtitle, const char *pic_url);
bool lv_dc_get_item_vlist(const lv_data_catalog_t *catalog, uint32_t index, lv_vlist_item_t *out);
bool lv_dc_should_request_image(const lv_data_catalog_t *catalog, uint32_t id);
bool lv_dc_mark_image_loading(lv_data_catalog_t *catalog, uint32_t id);
bool lv_dc_mark_image_ready(lv_data_catalog_t *catalog, uint32_t id, const char *display_path, const char *local_path);
bool lv_dc_mark_image_failed(lv_data_catalog_t *catalog, uint32_t id);
void lv_dc_touch_range(lv_data_catalog_t *catalog, uint32_t first_index, uint32_t count);
void lv_dc_release_image_cache_outside_visible(lv_data_catalog_t *catalog, uint32_t top_index, uint32_t visible_count);
uint32_t lv_dc_evict_outside_window(lv_data_catalog_t *catalog,
                                    uint32_t top_index,
                                    uint32_t visible_count,
                                    uint32_t keep_before,
                                    uint32_t keep_after,
                                    uint32_t max_ready_images);
void lv_dc_cleanup_all_images(lv_data_catalog_t *catalog);

#ifdef __cplusplus
}
#endif

#endif
