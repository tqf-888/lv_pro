#ifndef LV_COVER_CATALOG_H
#define LV_COVER_CATALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"

#define LV_COVER_NAME_MAX 128
#define LV_COVER_PATH_MAX 256

typedef struct {
    uint32_t slot_id;
    uint32_t cover_id;
    uint8_t ready;
    uint8_t loading;
    uint8_t selected;

    uint8_t cover_ready;
    uint8_t cover_loading;

    char name[LV_COVER_NAME_MAX];
    char cover_local_path[LV_COVER_PATH_MAX];
} lv_cover_item_t;

typedef struct {
    uint32_t total_items;
    lv_cover_item_t *items;
    pthread_mutex_t mutex;
} lv_cover_catalog_t;

bool lv_cover_catalog_init(lv_cover_catalog_t *catalog, uint32_t total_items);
void lv_cover_catalog_deinit(lv_cover_catalog_t *catalog);
bool lv_cover_catalog_reset(lv_cover_catalog_t *catalog, uint32_t total_items);

uint32_t lv_cover_catalog_count(const lv_cover_catalog_t *catalog);
bool lv_cover_catalog_get_item(const lv_cover_catalog_t *catalog, uint32_t index, lv_cover_item_t *out);
bool lv_cover_catalog_set_item_by_slot(lv_cover_catalog_t *catalog, uint32_t slot, const lv_cover_item_t *item);
bool lv_cover_catalog_get_vlist_item(const lv_cover_catalog_t *catalog, uint32_t index, lv_vlist_item_t *out);
void lv_cover_catalog_set_selected(lv_cover_catalog_t *catalog, uint32_t index, bool selected);

#ifdef __cplusplus
}
#endif

#endif
