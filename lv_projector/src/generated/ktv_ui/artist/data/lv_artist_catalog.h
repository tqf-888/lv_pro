#ifndef LV_ARTIST_CATALOG_H
#define LV_ARTIST_CATALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"

#define LV_ARTIST_NAME_MAX 64
#define LV_ARTIST_AVATAR_PATH_MAX 256

typedef struct {
    uint32_t slot_id;
    uint32_t artist_id;
    uint8_t ready;
    uint8_t loading;
    uint8_t selected;

    uint8_t avatar_ready;
    uint8_t avatar_loading;

    char name[LV_ARTIST_NAME_MAX];
    char avatar_local_path[LV_ARTIST_AVATAR_PATH_MAX];
} lv_artist_item_t;

typedef struct {
    uint32_t total_items;
    lv_artist_item_t *items;
    pthread_mutex_t mutex;
} lv_artist_catalog_t;

bool lv_artist_catalog_init(lv_artist_catalog_t *catalog, uint32_t total_items);
void lv_artist_catalog_deinit(lv_artist_catalog_t *catalog);
bool lv_artist_catalog_reset(lv_artist_catalog_t *catalog, uint32_t total_items);

uint32_t lv_artist_catalog_count(const lv_artist_catalog_t *catalog);
bool lv_artist_catalog_get_item(const lv_artist_catalog_t *catalog, uint32_t index, lv_artist_item_t *out);
bool lv_artist_catalog_set_item_by_slot(lv_artist_catalog_t *catalog, uint32_t slot, const lv_artist_item_t *item);
bool lv_artist_catalog_get_vlist_item(const lv_artist_catalog_t *catalog, uint32_t index, lv_vlist_item_t *out);
void lv_artist_catalog_set_selected(lv_artist_catalog_t *catalog, uint32_t index, bool selected);

#ifdef __cplusplus
}
#endif

#endif
