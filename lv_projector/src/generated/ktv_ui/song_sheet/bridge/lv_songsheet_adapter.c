#include "lv_songsheet_adapter.h"

#include <stdlib.h>
#include <string.h>

#include "ktv_songsheet_manager.h"
#include "lv_renderer_generic.h"
#include "lv_layout_styles.h"

static lv_songsheet_adapter_t *g_lv_songsheet_active = NULL;

static uint32_t lv_sa_safe_u32(uint32_t v, uint32_t defv)
{
    return (v == 0U) ? defv : v;
}

static uint32_t lv_sa_get_count(void *user_ctx)
{
    lv_songsheet_adapter_t *adapter = (lv_songsheet_adapter_t *)user_ctx;
    return (adapter != NULL) ? lv_dc_total_items(&adapter->catalog) : 0U;
}

static bool lv_sa_get_item(uint32_t index, lv_vlist_item_t *out, void *user_ctx)
{
    lv_songsheet_adapter_t *adapter = (lv_songsheet_adapter_t *)user_ctx;
    return (adapter != NULL && out != NULL) ? lv_dc_get_item_vlist(&adapter->catalog, index, out) : false;
}

static int lv_sa_sync_json_batch(lv_songsheet_adapter_t *adapter, uint32_t batch_index)
{
    uint32_t start_id;
    uint32_t end_id;
    uint32_t i;
    char title[LV_DC_TITLE_MAX];
    char subtitle[LV_DC_SUBTITLE_MAX];
    char pic_url[LV_DC_URL_MAX];

    if (adapter == NULL || batch_index >= adapter->json_batch_count) {
        return -1;
    }

    start_id = batch_index * adapter->json_batch_size;
    end_id = start_id + adapter->json_batch_size;
    if (end_id > adapter->total_count) {
        end_id = adapter->total_count;
    }

    for (i = start_id; i < end_id; ++i) {
        memset(title, 0, sizeof(title));
        memset(subtitle, 0, sizeof(subtitle));
        memset(pic_url, 0, sizeof(pic_url));

        if (ktv_get_songsheet_info((int)i,
                                   title, sizeof(title),
                                   subtitle, sizeof(subtitle)) == 0) {
            (void)ktv_get_songsheet_pic_url((int)i, pic_url, sizeof(pic_url));
            (void)lv_dc_set_item(&adapter->catalog, i, title, subtitle, pic_url);
        }
    }

    return 0;
}

static void lv_sa_request_json_batch_if_needed(lv_songsheet_adapter_t *adapter, uint32_t item_id)
{
    uint32_t batch_index;

    if (adapter == NULL || adapter->json_batch_size == 0U || item_id >= adapter->total_count) {
        return;
    }

    batch_index = item_id / adapter->json_batch_size;
    if (batch_index >= adapter->json_batch_count) {
        return;
    }

    if (adapter->json_batch_state[batch_index] != 0U) {
        return;
    }

    adapter->json_batch_state[batch_index] = 1U;
    fetch_song_sheet((int)batch_index);
}

static void lv_sa_request_image(uint32_t item_id, uint32_t index, void *user_ctx)
{
    lv_songsheet_adapter_t *adapter = (lv_songsheet_adapter_t *)user_ctx;
    (void)index;

    if (adapter == NULL || !adapter->started || item_id >= adapter->total_count) {
        return;
    }

    if (!lv_dc_is_item_ready(&adapter->catalog, item_id)) {
        lv_sa_request_json_batch_if_needed(adapter, item_id);
        return;
    }

    if (!lv_dc_should_request_image(&adapter->catalog, item_id)) {
        return;
    }

    if (!lv_dc_mark_image_loading(&adapter->catalog, item_id)) {
        return;
    }

    fetch_song_sheet_img((int)item_id);
}

static int lv_sa_pop_one_pending_batch(lv_songsheet_adapter_t *adapter, uint32_t *batch_index, uint8_t *success)
{
    uint32_t i;
    if (adapter == NULL) return 0;
    pthread_mutex_lock(&adapter->pending_lock);
    for (i = 0U; i < adapter->json_batch_count; ++i) {
        if (adapter->pending_batch_valid[i]) {
            adapter->pending_batch_valid[i] = 0U;
            *batch_index = i;
            *success = adapter->pending_batch_success[i];
            pthread_mutex_unlock(&adapter->pending_lock);
            return 1;
        }
    }
    pthread_mutex_unlock(&adapter->pending_lock);
    return 0;
}

static int lv_sa_pop_one_pending_image(lv_songsheet_adapter_t *adapter, uint32_t *item_id, uint8_t *success)
{
    uint32_t i;
    if (adapter == NULL) return 0;
    pthread_mutex_lock(&adapter->pending_lock);
    for (i = 0U; i < adapter->total_count; ++i) {
        if (adapter->pending_image_valid[i]) {
            adapter->pending_image_valid[i] = 0U;
            *item_id = i;
            *success = adapter->pending_image_success[i];
            pthread_mutex_unlock(&adapter->pending_lock);
            return 1;
        }
    }
    adapter->has_pending = 0U;
    pthread_mutex_unlock(&adapter->pending_lock);
    return 0;
}

static void lv_sa_trim_cache(lv_songsheet_adapter_t *adapter)
{
    uint32_t top;
    uint32_t visible_count;
    if (adapter == NULL || adapter->vlist == NULL) return;
    top = lv_vlist_get_top_index(adapter->vlist);
    visible_count = adapter->view_style->visible_rows * adapter->view_style->visible_cols;
    lv_dc_release_image_cache_outside_visible(&adapter->catalog, top, visible_count);
    lv_dc_touch_range(&adapter->catalog,
                      (top > 4U) ? (top - 4U) : 0U,
                      visible_count + 14U);
    (void)lv_dc_evict_outside_window(&adapter->catalog,
                                     top,
                                     visible_count,
                                     4U,
                                     10U,
                                     20U);
}

static void lv_sa_ui_timer_cb(lv_timer_t *timer)
{
    lv_songsheet_adapter_t *adapter = (lv_songsheet_adapter_t *)timer->user_data;
    uint32_t batch_index;
    uint32_t item_id;
    uint8_t success;
    int batch_updated = 0;
    int any_updated = 0;

    if (adapter == NULL || adapter->vlist == NULL) {
        return;
    }

    pthread_mutex_lock(&adapter->pending_lock);
    if (!adapter->has_pending) {
        pthread_mutex_unlock(&adapter->pending_lock);
        return;
    }
    pthread_mutex_unlock(&adapter->pending_lock);

    while (lv_sa_pop_one_pending_batch(adapter, &batch_index, &success)) {
        uint32_t start_id;
        uint32_t count;
        if (batch_index >= adapter->json_batch_count) {
            continue;
        }
        if (success) {
            if (lv_sa_sync_json_batch(adapter, batch_index) == 0) {
                adapter->json_batch_state[batch_index] = 2U;
                start_id = batch_index * adapter->json_batch_size;
                count = adapter->json_batch_size;
                if (start_id + count > adapter->total_count) {
                    count = adapter->total_count - start_id;
                }
                lv_vlist_notify_range_changed(adapter->vlist, start_id, count);
                batch_updated = 1;
            } else {
                adapter->json_batch_state[batch_index] = 0U;
            }
        } else {
            adapter->json_batch_state[batch_index] = 0U;
        }
        any_updated = 1;
    }

    while (lv_sa_pop_one_pending_image(adapter, &item_id, &success)) {
        char display_path[LV_DC_PATH_MAX];
        char local_path[LV_DC_PATH_MAX];
        memset(display_path, 0, sizeof(display_path));
        memset(local_path, 0, sizeof(local_path));

        if (success &&
            ktv_get_songsheet_pic_path((int)item_id, display_path, sizeof(display_path)) == 0 &&
            app_data_make_songsheet_pic_path((int)item_id, local_path, sizeof(local_path)) == 0) {
            lv_dc_mark_image_ready(&adapter->catalog, item_id, display_path, local_path);
        } else {
            lv_dc_mark_image_failed(&adapter->catalog, item_id);
        }

        lv_vlist_notify_item_changed(adapter->vlist, item_id);
        any_updated = 1;
    }

    if (batch_updated) {
        lv_vlist_request_visible_images(adapter->vlist);
    }
    if (any_updated) {
        lv_sa_trim_cache(adapter);
    }
}

int lv_songsheet_adapter_start(lv_songsheet_adapter_t *adapter,
                               lv_obj_t *parent,
                               const lv_songsheet_view_style_t *view_style,
                               uint32_t total_count,
                               uint32_t json_batch_size,
                               const void *placeholder_img_src,
                               const void *failed_img_src,
                               const lv_vlist_renderer_ops_t *renderer_ops,
                               const void *renderer_style)
{
    lv_vlist_config_t cfg;
    lv_vlist_ops_t ops;
    uint32_t batch_count;

    if (adapter == NULL || parent == NULL || view_style == NULL || total_count == 0U || json_batch_size == 0U) {
        return -1;
    }

    memset(adapter, 0, sizeof(*adapter));

    adapter->parent = parent;
    adapter->view_style = view_style;
    adapter->renderer_ops = (renderer_ops != NULL) ? renderer_ops : &g_lv_renderer_generic_ops;
    adapter->renderer_style = (renderer_style != NULL) ? renderer_style : &view_style->renderer_style;
    adapter->placeholder_img_src = placeholder_img_src;
    adapter->failed_img_src = (failed_img_src != NULL) ? failed_img_src : placeholder_img_src;
    adapter->total_count = total_count;
    adapter->json_batch_size = json_batch_size;
    adapter->json_batch_count = (total_count + json_batch_size - 1U) / json_batch_size;

    if (!lv_dc_init(&adapter->catalog, total_count)) {
        return -1;
    }

    batch_count = adapter->json_batch_count;
    adapter->json_batch_state = (uint8_t *)calloc(batch_count, sizeof(uint8_t));
    adapter->pending_batch_valid = (uint8_t *)calloc(batch_count, sizeof(uint8_t));
    adapter->pending_batch_success = (uint8_t *)calloc(batch_count, sizeof(uint8_t));
    adapter->pending_image_valid = (uint8_t *)calloc(total_count, sizeof(uint8_t));
    adapter->pending_image_success = (uint8_t *)calloc(total_count, sizeof(uint8_t));
    if (adapter->json_batch_state == NULL || adapter->pending_batch_valid == NULL ||
        adapter->pending_batch_success == NULL || adapter->pending_image_valid == NULL ||
        adapter->pending_image_success == NULL) {
        lv_dc_deinit(&adapter->catalog);
        free(adapter->json_batch_state);
        free(adapter->pending_batch_valid);
        free(adapter->pending_batch_success);
        free(adapter->pending_image_valid);
        free(adapter->pending_image_success);
        memset(adapter, 0, sizeof(*adapter));
        return -1;
    }

    pthread_mutex_init(&adapter->pending_lock, NULL);

    memset(&cfg, 0, sizeof(cfg));
    cfg.parent = parent;
    cfg.visible_rows = lv_sa_safe_u32(view_style->visible_rows, 1U);
    cfg.visible_cols = lv_sa_safe_u32(view_style->visible_cols, 1U);
    cfg.overscan_rows_front = lv_sa_safe_u32(view_style->overscan_rows_front, 1U);
    cfg.overscan_rows_back = lv_sa_safe_u32(view_style->overscan_rows_back, 1U);
    cfg.preload_before = lv_sa_safe_u32(view_style->preload_before, 2U);
    cfg.preload_after = lv_sa_safe_u32(view_style->preload_after, 8U);
    cfg.viewport_width = view_style->viewport_width;
    cfg.viewport_height = view_style->viewport_height;
    cfg.cell_width = (view_style->cell_width != 0) ? view_style->cell_width : view_style->renderer_style.cell_width;
    cfg.cell_height = (view_style->cell_height != 0) ? view_style->cell_height : view_style->renderer_style.cell_height;
    cfg.gap_x = view_style->gap_x;
    cfg.gap_y = view_style->gap_y;
    cfg.placeholder_img_src = adapter->placeholder_img_src;
    cfg.failed_img_src = adapter->failed_img_src;
    cfg.user_ctx = adapter;
    cfg.renderer_ops = adapter->renderer_ops;
    cfg.renderer_style = adapter->renderer_style;
    cfg.on_item_click = NULL;

    memset(&ops, 0, sizeof(ops));
    ops.get_count = lv_sa_get_count;
    ops.get_item = lv_sa_get_item;
    ops.request_image = lv_sa_request_image;

    adapter->vlist = lv_vlist_create(&cfg, &ops);
    if (adapter->vlist == NULL) {
        pthread_mutex_destroy(&adapter->pending_lock);
        lv_dc_deinit(&adapter->catalog);
        free(adapter->json_batch_state);
        free(adapter->pending_batch_valid);
        free(adapter->pending_batch_success);
        free(adapter->pending_image_valid);
        free(adapter->pending_image_success);
        memset(adapter, 0, sizeof(*adapter));
        return -1;
    }

    lv_img_cache_set_size(12);
    adapter->ui_timer = lv_timer_create(lv_sa_ui_timer_cb, 50U, adapter);
    adapter->started = 1U;
    g_lv_songsheet_active = adapter;
    lv_vlist_request_visible_images(adapter->vlist);
    return 0;
}

void lv_songsheet_adapter_stop(lv_songsheet_adapter_t *adapter)
{
    if (adapter == NULL || !adapter->started) {
        return;
    }

    if (adapter->ui_timer != NULL) {
        lv_timer_del(adapter->ui_timer);
        adapter->ui_timer = NULL;
    }
    if (adapter->vlist != NULL) {
        lv_vlist_destroy(adapter->vlist);
        adapter->vlist = NULL;
    }

    lv_dc_cleanup_all_images(&adapter->catalog);
    lv_dc_deinit(&adapter->catalog);

    pthread_mutex_destroy(&adapter->pending_lock);
    free(adapter->json_batch_state);
    free(adapter->pending_batch_valid);
    free(adapter->pending_batch_success);
    free(adapter->pending_image_valid);
    free(adapter->pending_image_success);

    if (g_lv_songsheet_active == adapter) {
        g_lv_songsheet_active = NULL;
    }

    memset(adapter, 0, sizeof(*adapter));
}

lv_songsheet_adapter_t *lv_songsheet_adapter_get_default(void)
{
    return g_lv_songsheet_active;
}

void lv_songsheet_adapter_notify_page_ready(lv_songsheet_adapter_t *adapter,
                                            uint32_t page_index,
                                            bool success)
{
    if (adapter == NULL || !adapter->started || page_index >= adapter->json_batch_count) {
        return;
    }

    pthread_mutex_lock(&adapter->pending_lock);
    adapter->pending_batch_valid[page_index] = 1U;
    adapter->pending_batch_success[page_index] = success ? 1U : 0U;
    adapter->has_pending = 1U;
    pthread_mutex_unlock(&adapter->pending_lock);
}

void lv_songsheet_adapter_notify_image_ready(lv_songsheet_adapter_t *adapter,
                                             uint32_t item_id,
                                             bool success)
{
    if (adapter == NULL || !adapter->started || item_id >= adapter->total_count) {
        return;
    }

    pthread_mutex_lock(&adapter->pending_lock);
    adapter->pending_image_valid[item_id] = 1U;
    adapter->pending_image_success[item_id] = success ? 1U : 0U;
    adapter->has_pending = 1U;
    pthread_mutex_unlock(&adapter->pending_lock);
}

void lv_songsheet_adapter_notify_default_page_ready(uint32_t page_index, bool success)
{
    lv_songsheet_adapter_notify_page_ready(g_lv_songsheet_active, page_index, success);
}

void lv_songsheet_adapter_notify_default_image_ready(uint32_t item_id, bool success)
{
    lv_songsheet_adapter_notify_image_ready(g_lv_songsheet_active, item_id, success);
}
