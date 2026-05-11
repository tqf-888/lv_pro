#include "lv_cover_catalog.h"
#include <stdlib.h>
#include <string.h>

static void cover_init_item(lv_cover_item_t *item, uint32_t slot_id)
{
    if (item == NULL) return;
    memset(item, 0, sizeof(*item));
    item->slot_id = slot_id;
    item->cover_id = 0U;
}

bool lv_cover_catalog_init(lv_cover_catalog_t *catalog, uint32_t total_items)
{
    uint32_t i;
    if (catalog == NULL || total_items == 0U) return false;

    memset(catalog, 0, sizeof(*catalog));
    catalog->items = (lv_cover_item_t *)calloc(total_items, sizeof(lv_cover_item_t));
    if (catalog->items == NULL) return false;

    for (i = 0U; i < total_items; ++i) cover_init_item(&catalog->items[i], i);
    catalog->total_items = total_items;
    pthread_mutex_init(&catalog->mutex, NULL);
    return true;
}

void lv_cover_catalog_deinit(lv_cover_catalog_t *catalog)
{
    if (catalog == NULL) return;
    pthread_mutex_destroy(&catalog->mutex);
    free(catalog->items);
    memset(catalog, 0, sizeof(*catalog));
}

bool lv_cover_catalog_reset(lv_cover_catalog_t *catalog, uint32_t total_items)
{
    if (catalog == NULL) return false;
    lv_cover_catalog_deinit(catalog);
    return lv_cover_catalog_init(catalog, total_items);
}

uint32_t lv_cover_catalog_count(const lv_cover_catalog_t *catalog)
{
    return (catalog != NULL) ? catalog->total_items : 0U;
}

bool lv_cover_catalog_get_item(const lv_cover_catalog_t *catalog, uint32_t index, lv_cover_item_t *out)
{
    lv_cover_catalog_t *c = (lv_cover_catalog_t *)catalog;
    if (c == NULL || out == NULL || index >= c->total_items || c->items == NULL) return false;
    pthread_mutex_lock(&c->mutex);
    memcpy(out, &c->items[index], sizeof(*out));
    pthread_mutex_unlock(&c->mutex);
    return true;
}

bool lv_cover_catalog_set_item_by_slot(lv_cover_catalog_t *catalog, uint32_t slot, const lv_cover_item_t *item)
{
    if (catalog == NULL || item == NULL || slot >= catalog->total_items || catalog->items == NULL) return false;
    pthread_mutex_lock(&catalog->mutex);
    catalog->items[slot] = *item;
    catalog->items[slot].slot_id = slot;
    pthread_mutex_unlock(&catalog->mutex);
    return true;
}

bool lv_cover_catalog_get_vlist_item(const lv_cover_catalog_t *catalog, uint32_t index, lv_vlist_item_t *out)
{
    lv_cover_item_t item;
    if (out == NULL) return false;
    memset(out, 0, sizeof(*out));
    if (!lv_cover_catalog_get_item(catalog, index, &item)) return false;

    out->item_id = index;
    out->title = item.ready ? item.name : "";
    out->subtitle = "";

    if (!item.ready) {
        out->img_state = item.loading ? LV_VLIST_IMG_LOADING : LV_VLIST_IMG_NONE;
        out->image_requested = item.loading ? true : false;
    } else if (item.cover_ready) {
        out->img_state = LV_VLIST_IMG_READY;
        out->image_requested = true;
    } else if (item.cover_loading) {
        out->img_state = LV_VLIST_IMG_LOADING;
        out->image_requested = true;
    } else {
        out->img_state = LV_VLIST_IMG_NONE;
        out->image_requested = false;
    }

    out->img_src = NULL;
    return true;
}

void lv_cover_catalog_set_selected(lv_cover_catalog_t *catalog, uint32_t index, bool selected)
{
    if (catalog == NULL || index >= catalog->total_items || catalog->items == NULL) return;
    pthread_mutex_lock(&catalog->mutex);
    catalog->items[index].selected = selected ? 1U : 0U;
    pthread_mutex_unlock(&catalog->mutex);
}
