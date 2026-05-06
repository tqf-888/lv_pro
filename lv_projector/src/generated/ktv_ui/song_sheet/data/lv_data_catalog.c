#include "lv_data_catalog.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LV_DC_EMPTY_TEXT_PLACEHOLDER   "暂时无"

static void lv_dc_invalidate_cache_src(const char *src)
{
    if (src == NULL || src[0] == '\0') return;
    lv_img_cache_invalidate_src(src);
}

static void lv_dc_copy_text(char *dst, size_t dst_size, const char *src)
{
    if (dst == NULL || dst_size == 0U) return;
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }
    strncpy(dst, src, dst_size - 1U);
    dst[dst_size - 1U] = '\0';
}

bool lv_dc_init(lv_data_catalog_t *catalog, uint32_t total_items)
{
    uint32_t i;
    if (catalog == NULL || total_items == 0U) return false;
    memset(catalog, 0, sizeof(*catalog));
    catalog->items = (lv_dc_item_t *)calloc(total_items, sizeof(lv_dc_item_t));
    if (catalog->items == NULL) return false;
    catalog->total_items = total_items;
    catalog->touch_seq_counter = 1U;
    for (i = 0U; i < total_items; ++i) {
        catalog->items[i].id = (uint16_t)i;
        catalog->items[i].img_state = LV_VLIST_IMG_NONE;
        catalog->items[i].touch_seq = 0U;
    }
    return true;
}

void lv_dc_deinit(lv_data_catalog_t *catalog)
{
    if (catalog == NULL) return;
    free(catalog->items);
    memset(catalog, 0, sizeof(*catalog));
}

uint32_t lv_dc_total_items(const lv_data_catalog_t *catalog)
{
    return (catalog != NULL) ? catalog->total_items : 0U;
}

bool lv_dc_is_item_ready(const lv_data_catalog_t *catalog, uint32_t id)
{
    return (catalog != NULL && id < catalog->total_items) ? (catalog->items[id].data_ready != 0U) : false;
}

bool lv_dc_set_item(lv_data_catalog_t *catalog, uint32_t id, const char *title, const char *subtitle, const char *pic_url)
{
    lv_dc_item_t *item;
    if (catalog == NULL || id >= catalog->total_items) return false;
    item = &catalog->items[id];
    item->data_ready = 1U;
    lv_dc_copy_text(item->title, sizeof(item->title), (title != NULL && title[0] != '\0') ? title : LV_DC_EMPTY_TEXT_PLACEHOLDER);
    lv_dc_copy_text(item->subtitle, sizeof(item->subtitle), (subtitle != NULL && subtitle[0] != '\0') ? subtitle : LV_DC_EMPTY_TEXT_PLACEHOLDER);
    lv_dc_copy_text(item->pic_url, sizeof(item->pic_url), pic_url);
    return true;
}

bool lv_dc_get_item_vlist(const lv_data_catalog_t *catalog, uint32_t index, lv_vlist_item_t *out)
{
    const lv_dc_item_t *item;
    if (catalog == NULL || out == NULL || index >= catalog->total_items) return false;
    item = &catalog->items[index];
    memset(out, 0, sizeof(*out));
    out->item_id = item->id;
    out->title = item->data_ready ? item->title : "";
    out->subtitle = item->data_ready ? item->subtitle : "";
    out->img_state = item->img_state;
    out->image_requested = (item->image_requested != 0U);
    out->img_src = (item->img_state == LV_VLIST_IMG_READY && item->img_display_path[0] != '\0') ? (const void *)item->img_display_path : NULL;
    if (item->img_state == LV_VLIST_IMG_READY && item->img_display_path[0] != '\0') {
        ((lv_dc_item_t *)item)->image_cache_marked = 1U;
    }
    return true;
}

bool lv_dc_should_request_image(const lv_data_catalog_t *catalog, uint32_t id)
{
    const lv_dc_item_t *item;
    if (catalog == NULL || id >= catalog->total_items) return false;
    item = &catalog->items[id];
    return (item->data_ready != 0U) && (item->pic_url[0] != '\0') && (item->image_requested == 0U) && (item->img_state == LV_VLIST_IMG_NONE);
}

bool lv_dc_mark_image_loading(lv_data_catalog_t *catalog, uint32_t id)
{
    if (catalog == NULL || id >= catalog->total_items) return false;
    catalog->items[id].image_requested = 1U;
    catalog->items[id].img_state = LV_VLIST_IMG_LOADING;
    return true;
}

bool lv_dc_mark_image_ready(lv_data_catalog_t *catalog, uint32_t id, const char *display_path, const char *local_path)
{
    if (catalog == NULL || id >= catalog->total_items || display_path == NULL || local_path == NULL ||
        display_path[0] == '\0' || local_path[0] == '\0') return false;
    catalog->items[id].image_requested = 0U;
    catalog->items[id].img_state = LV_VLIST_IMG_READY;
    catalog->items[id].image_cache_marked = 0U;
    lv_dc_copy_text(catalog->items[id].img_display_path, sizeof(catalog->items[id].img_display_path), display_path);
    lv_dc_copy_text(catalog->items[id].img_local_path, sizeof(catalog->items[id].img_local_path), local_path);
    return true;
}

bool lv_dc_mark_image_failed(lv_data_catalog_t *catalog, uint32_t id)
{
    if (catalog == NULL || id >= catalog->total_items) return false;
    if (catalog->items[id].img_display_path[0] != '\0') lv_dc_invalidate_cache_src(catalog->items[id].img_display_path);
    catalog->items[id].image_requested = 0U;
    catalog->items[id].img_state = LV_VLIST_IMG_FAILED;
    catalog->items[id].img_display_path[0] = '\0';
    catalog->items[id].img_local_path[0] = '\0';
    catalog->items[id].image_cache_marked = 0U;
    return true;
}

void lv_dc_touch_range(lv_data_catalog_t *catalog, uint32_t first_index, uint32_t count)
{
    uint32_t i, last_index;
    if (catalog == NULL || count == 0U || first_index >= catalog->total_items) return;
    last_index = first_index + count;
    if (last_index > catalog->total_items) last_index = catalog->total_items;
    for (i = first_index; i < last_index; ++i) {
        catalog->items[i].touch_seq = catalog->touch_seq_counter++;
        if (catalog->touch_seq_counter == 0U) catalog->touch_seq_counter = 1U;
    }
}

void lv_dc_release_image_cache_outside_visible(lv_data_catalog_t *catalog, uint32_t top_index, uint32_t visible_count)
{
    uint32_t i, visible_begin, visible_end;
    if (catalog == NULL || catalog->items == NULL || visible_count == 0U) return;
    visible_begin = top_index;
    visible_end = top_index + visible_count;
    if (visible_begin >= catalog->total_items) visible_begin = catalog->total_items;
    if (visible_end > catalog->total_items) visible_end = catalog->total_items;
    for (i = 0U; i < catalog->total_items; ++i) {
        lv_dc_item_t *it = &catalog->items[i];
        if (it->img_state != LV_VLIST_IMG_READY || it->img_display_path[0] == '\0') continue;
        if (i >= visible_begin && i < visible_end) {
            it->image_cache_marked = 1U;
            continue;
        }
        if (it->image_cache_marked != 0U) {
            lv_dc_invalidate_cache_src(it->img_display_path);
            it->image_cache_marked = 0U;
        }
    }
}

uint32_t lv_dc_evict_outside_window(lv_data_catalog_t *catalog,
                                    uint32_t top_index,
                                    uint32_t visible_count,
                                    uint32_t keep_before,
                                    uint32_t keep_after,
                                    uint32_t max_ready_images)
{
    uint32_t i, ready_count = 0U, evicted = 0U, keep_begin, keep_end;
    if (catalog == NULL || catalog->items == NULL) return 0U;
    keep_begin = (top_index > keep_before) ? (top_index - keep_before) : 0U;
    keep_end = top_index + visible_count + keep_after;
    if (keep_end > catalog->total_items) keep_end = catalog->total_items;
    for (i = 0U; i < catalog->total_items; ++i) {
        if (catalog->items[i].img_state == LV_VLIST_IMG_READY && catalog->items[i].img_local_path[0] != '\0') ready_count++;
    }
    while (ready_count > max_ready_images) {
        uint32_t victim = UINT32_MAX;
        uint32_t oldest_touch = UINT32_MAX;
        for (i = 0U; i < catalog->total_items; ++i) {
            lv_dc_item_t *it = &catalog->items[i];
            if (it->img_state != LV_VLIST_IMG_READY || it->img_local_path[0] == '\0') continue;
            if (i >= keep_begin && i < keep_end) continue;
            if (it->touch_seq < oldest_touch) {
                oldest_touch = it->touch_seq;
                victim = i;
            }
        }
        if (victim == UINT32_MAX) break;
        lv_dc_invalidate_cache_src(catalog->items[victim].img_display_path);
        unlink(catalog->items[victim].img_local_path);
        catalog->items[victim].img_display_path[0] = '\0';
        catalog->items[victim].img_local_path[0] = '\0';
        catalog->items[victim].img_state = LV_VLIST_IMG_NONE;
        catalog->items[victim].image_requested = 0U;
        catalog->items[victim].image_cache_marked = 0U;
        evicted++;
        ready_count--;
    }
    return evicted;
}

void lv_dc_cleanup_all_images(lv_data_catalog_t *catalog)
{
    uint32_t i;
    if (catalog == NULL || catalog->items == NULL) return;
    for (i = 0U; i < catalog->total_items; ++i) {
        if (catalog->items[i].img_local_path[0] != '\0') {
            lv_dc_invalidate_cache_src(catalog->items[i].img_display_path);
            unlink(catalog->items[i].img_local_path);
        }
        catalog->items[i].img_display_path[0] = '\0';
        catalog->items[i].img_local_path[0] = '\0';
        catalog->items[i].img_state = LV_VLIST_IMG_NONE;
        catalog->items[i].image_requested = 0U;
        catalog->items[i].image_cache_marked = 0U;
    }
}
