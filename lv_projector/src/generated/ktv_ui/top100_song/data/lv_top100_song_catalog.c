#include "lv_top100_song_catalog.h"
#include "../lv_top100_song_debug.h"
#include <stdlib.h>
#include <string.h>

static void rs_init_item(lv_top100_song_item_t *item, uint32_t slot_id)
{
    if (item == NULL) return;
    memset(item, 0, sizeof(*item));
    item->slot_id = slot_id;
    item->song_id = 0;
}

static void rs_init_items_range(lv_top100_song_item_t *items, uint32_t total_items)
{
    uint32_t i;
    if (items == NULL) return;
    for (i = 0U; i < total_items; ++i) {
        rs_init_item(&items[i], i);
    }
}

bool lv_top100_song_catalog_init(lv_top100_song_catalog_t *catalog, uint32_t total_items)
{
    if (catalog == NULL || total_items == 0U) return false;
    memset(catalog, 0, sizeof(*catalog));

    catalog->items = (lv_top100_song_item_t *)RS_CALLOC(total_items, sizeof(lv_top100_song_item_t), "catalog.items");
    if (catalog->items == NULL) return false;

    rs_init_items_range(catalog->items, total_items);
    catalog->total_items = total_items;
    catalog->capacity_items = total_items;

    if (pthread_mutex_init(&catalog->mutex, NULL) != 0) {
        RS_FLOW_LOG("catalog mutex init failed: total_items=%u", total_items);
        RS_FREE(catalog->items, "catalog.items");
        memset(catalog, 0, sizeof(*catalog));
        return false;
    }

    catalog->mutex_inited = 1U;
    return true;
}

void lv_top100_song_catalog_deinit(lv_top100_song_catalog_t *catalog)
{
    if (catalog == NULL) return;

    if (catalog->mutex_inited) {
        pthread_mutex_destroy(&catalog->mutex);
        catalog->mutex_inited = 0U;
    }

    RS_FREE(catalog->items, "catalog.items");
    memset(catalog, 0, sizeof(*catalog));
}

bool lv_top100_song_catalog_reset(lv_top100_song_catalog_t *catalog, uint32_t total_items)
{
    lv_top100_song_item_t *new_items;

    if (catalog == NULL || total_items == 0U) return false;

    if (catalog->items != NULL && catalog->mutex_inited && catalog->capacity_items >= total_items) {
        pthread_mutex_lock(&catalog->mutex);
        memset(catalog->items, 0, (size_t)total_items * sizeof(lv_top100_song_item_t));
        rs_init_items_range(catalog->items, total_items);
        catalog->total_items = total_items;
        pthread_mutex_unlock(&catalog->mutex);
        RS_HOT_LOG("catalog reset reuse: total=%u capacity=%u", total_items, catalog->capacity_items);
        return true;
    }

    new_items = (lv_top100_song_item_t *)RS_CALLOC(total_items, sizeof(lv_top100_song_item_t), "catalog.items.reset");
    if (new_items == NULL) {
        RS_FLOW_LOG("catalog reset alloc failed: total_items=%u", total_items);
        return false;
    }

    rs_init_items_range(new_items, total_items);

    if (catalog->mutex_inited) {
        pthread_mutex_lock(&catalog->mutex);
        RS_FREE(catalog->items, "catalog.items.old");
        catalog->items = new_items;
        catalog->total_items = total_items;
        catalog->capacity_items = total_items;
        pthread_mutex_unlock(&catalog->mutex);
    } else {
        memset(catalog, 0, sizeof(*catalog));
        catalog->items = new_items;
        catalog->total_items = total_items;
        catalog->capacity_items = total_items;
        if (pthread_mutex_init(&catalog->mutex, NULL) != 0) {
            RS_FLOW_LOG("catalog reset mutex init failed: total_items=%u", total_items);
            RS_FREE(catalog->items, "catalog.items.reset");
            memset(catalog, 0, sizeof(*catalog));
            return false;
        }
        catalog->mutex_inited = 1U;
    }

    RS_FLOW_LOG("catalog reset realloc: total=%u capacity=%u", total_items, catalog->capacity_items);
    return true;
}

uint32_t lv_top100_song_catalog_count(const lv_top100_song_catalog_t *catalog)
{
    return (catalog != NULL) ? catalog->total_items : 0U;
}

bool lv_top100_song_catalog_get_item(const lv_top100_song_catalog_t *catalog, uint32_t index, lv_top100_song_item_t *out)
{
    lv_top100_song_catalog_t *c = (lv_top100_song_catalog_t *)catalog;
    if (c == NULL || out == NULL || index >= c->total_items || c->items == NULL || !c->mutex_inited) return false;
    pthread_mutex_lock(&c->mutex);
    memcpy(out, &c->items[index], sizeof(*out));
    pthread_mutex_unlock(&c->mutex);
    return true;
}

bool lv_top100_song_catalog_set_item_by_slot(lv_top100_song_catalog_t *catalog, uint32_t slot, const lv_top100_song_item_t *item)
{
    if (catalog == NULL || item == NULL || slot >= catalog->total_items || catalog->items == NULL || !catalog->mutex_inited) return false;
    pthread_mutex_lock(&catalog->mutex);
    catalog->items[slot] = *item;
    catalog->items[slot].slot_id = slot;
    catalog->items[slot].ready = 1U;
    catalog->items[slot].loading = 0U;
    pthread_mutex_unlock(&catalog->mutex);
    return true;
}

bool lv_top100_song_catalog_truncate(lv_top100_song_catalog_t *catalog, uint32_t new_total_items)
{
    if (catalog == NULL || !catalog->mutex_inited) return false;
    if (new_total_items > catalog->total_items) return false;

    pthread_mutex_lock(&catalog->mutex);
    catalog->total_items = new_total_items;
    pthread_mutex_unlock(&catalog->mutex);
    return true;
}

bool lv_top100_song_catalog_get_vlist_item(const lv_top100_song_catalog_t *catalog, uint32_t index, lv_vlist_item_t *out)
{
    lv_top100_song_item_t item;
    if (out == NULL) return false;
    memset(out, 0, sizeof(*out));
    if (!lv_top100_song_catalog_get_item(catalog, index, &item)) return false;

    out->item_id = index;
    out->title = item.ready ? item.name : "";
    out->subtitle = "";

    if (item.ready) {
        out->img_state = LV_VLIST_IMG_READY;
        out->image_requested = false;
    } else if (item.loading) {
        out->img_state = LV_VLIST_IMG_LOADING;
        out->image_requested = true;
    } else {
        out->img_state = LV_VLIST_IMG_NONE;
        out->image_requested = false;
    }

    out->img_src = NULL;
    return true;
}

void lv_top100_song_catalog_set_selected(lv_top100_song_catalog_t *catalog, uint32_t index, bool selected)
{
    if (catalog == NULL || index >= catalog->total_items || catalog->items == NULL || !catalog->mutex_inited) return;
    pthread_mutex_lock(&catalog->mutex);
    catalog->items[index].selected = selected ? 1U : 0U;
    pthread_mutex_unlock(&catalog->mutex);
}
