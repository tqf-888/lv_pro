#include "lv_cover_adapter.h"
#include "lv_renderer_cover.h"
#include "cover_page_demo.h"
#include "cover_media_loader.h"
#include "cover_media_loader_demo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "db_list_pro_worker.h"
#include "ktv_player_ui.h"
#include "ktv_cloud_order_api.h"
#include "db_list_pro_thread.h"
#include "gui_guider.h"
#include "recommend_video_fetch.h"

#ifndef COVER_ADAPTER_LOG_ENABLE
#define COVER_ADAPTER_LOG_ENABLE 1
#endif

#ifndef COVER_ADAPTER_LOG_DEBUG
#define COVER_ADAPTER_LOG_DEBUG 0
#endif

#ifndef COVER_QUERY_MAX_COUNT
#define COVER_QUERY_MAX_COUNT 10U
#endif

/*
 * probe 同时盯几"页"：1 = 只盯当前页（旧行为），2 = 当前页 + 下一页（默认）。
 *
 * 设成 2 的目的：用户还在当前页时，把下一页 8 个 cell 的 cover_local_path
 * 提前回填到 catalog，从而触发它们的 lv_img_set_src（这些 cell 在 vlist 物理
 * 层已经因为 overscan_rows_back=1 创建并 bind 了，只是位置在视口下方）。
 *
 * 注意：这个值只影响 UI 层的"预绑窗口"，按真实可见 cell 数计算，不能再拿
 * json_page_size(常见 50) 来乘，否则 probe 会变成 100 个槽。
 */
#ifndef COVER_PROBE_PAGES
#define COVER_PROBE_PAGES 2U
#endif

#ifndef COVER_IMAGE_DOWNLOAD_PAGES
/* 图片下载只追真实可见页 + 1 页 probe。JSON 可以预取几页，但图片不能跟着
 * JSON 整页 50 张一起下，否则首屏/快滑会把 HTTP 队列打爆。 */
#define COVER_IMAGE_DOWNLOAD_PAGES 2U
#endif

#ifndef COVER_READY_RETAIN_LIMIT
/* 本地 READY 图片保留在 50-80 张量级；这里用 80，和 image_manager 的
 * IMG_READY_FILE_LIMIT 默认值保持一致。 */
#define COVER_READY_RETAIN_LIMIT 80U
#endif

#if COVER_ADAPTER_LOG_ENABLE
#define COVER_LOGI(fmt, ...) printf("[cover] " fmt "\n", ##__VA_ARGS__)
#else
#define COVER_LOGI(fmt, ...) ((void)0)
#endif

#if COVER_ADAPTER_LOG_ENABLE && COVER_ADAPTER_LOG_DEBUG
#define COVER_LOGD(fmt, ...) printf("[cover][D] " fmt "\n", ##__VA_ARGS__)
#else
#define COVER_LOGD(fmt, ...) ((void)0)
#endif

/* image_manager 暴露的"可见范围"提示，用来让 need_image 通知给当前页打 HIGH 优先。
 * 这里 extern 声明而不直接 include image_manager.h 是为了不污染 adapter 头依赖。 */
extern void img_mgr_set_visible_range(uint32_t start_slot_id, uint32_t end_slot_id);
extern void img_mgr_set_image_active_range(uint32_t download_start_slot_id,
                                           uint32_t download_end_slot_id,
                                           uint32_t retain_start_slot_id,
                                           uint32_t retain_end_slot_id);

static lv_cover_adapter_t *g_cover_default = NULL;
static uint32_t g_clicked_cover_id = 0U;
static char g_clicked_cover_name[LV_COVER_NAME_MAX] = {0};
static uint8_t g_cover_jump_pending = 0U;
static uint32_t g_cover_focused_item_id = 0U;
static uint8_t g_cover_has_focused_item = 0U;

static void cover_ensure_item_ready(lv_cover_adapter_t *adapter, uint32_t item_id);
static void cover_prefetch_page_window(lv_cover_adapter_t *adapter,
                                        uint32_t visible_start,
                                        uint32_t visible_end,
                                        uint32_t page_start,
                                        uint32_t page_end);

static uint32_t cover_safe_u32(uint32_t v, uint32_t defv)
{
    return (v == 0U) ? defv : v;
}

static uint32_t cover_visible_count(const lv_cover_adapter_t *adapter)
{
    uint32_t rows;
    uint32_t cols;

    if (adapter == NULL || adapter->view_style == NULL) return 6U;
    rows = cover_safe_u32(adapter->view_style->visible_rows, 1U);
    cols = cover_safe_u32(adapter->view_style->visible_cols, 1U);
    return rows * cols;
}

static uint32_t cover_sub_pages(uint32_t base, uint32_t page_size, uint32_t pages)
{
    uint32_t delta;

    if (page_size == 0U || pages == 0U) return base;
    delta = page_size * pages;
    if (delta / page_size != pages || delta >= base) return 0U;
    return base - delta;
}

static uint32_t cover_add_pages_clamped(const lv_cover_adapter_t *adapter,
                                         uint32_t base,
                                         uint32_t page_size,
                                         uint32_t pages)
{
    uint32_t delta;
    uint32_t last;

    if (adapter == NULL || adapter->total_count == 0U) return 0U;
    if (page_size == 0U || pages == 0U) return base;

    last = adapter->total_count - 1U;
    delta = page_size * pages;
    if (delta / page_size != pages) return last;
    if (base > last || base + delta < base || base + delta > last) return last;
    return base + delta;
}

static void cover_calc_prefetch_window(const lv_cover_adapter_t *adapter,
                                        uint32_t page_start,
                                        uint32_t page_end,
                                        uint32_t page_size,
                                        uint32_t *out_start,
                                        uint32_t *out_end)
{
    uint32_t before_pages = 1U;
    uint32_t after_pages = 1U;

    if (out_start == NULL || out_end == NULL) return;
    *out_start = page_start;
    *out_end = page_end;

    if (adapter == NULL || adapter->total_count == 0U) return;

    if (adapter->scroll_direction > 0) {
        before_pages = 1U;
        after_pages = 3U;
    } else if (adapter->scroll_direction < 0) {
        before_pages = 3U;
        after_pages = 1U;
    } else {
        before_pages = 1U;
        after_pages = 2U;
    }

    *out_start = cover_sub_pages(page_start, page_size, before_pages);
    *out_end = cover_add_pages_clamped(adapter, page_end, page_size, after_pages);
}

static void cover_build_default_name(uint32_t cover_id, char *buf, size_t size)
{
    (void)cover_id;
    if (buf == NULL || size == 0U) return;
    buf[0] = '\0';
}

static void cover_probe_reset_for_page(lv_cover_adapter_t *adapter, uint32_t page_start, uint32_t page_end)
{
    uint32_t i;

    if (adapter == NULL || adapter->probe_slots == NULL || adapter->probe_slot_count == 0U) return;

    adapter->probe_page_start = page_start;
    adapter->probe_page_end = page_end;

    /*
     * probe_slot_count 已经按 COVER_PROBE_PAGES * ui_batch_size 分配，
     * 这里直接以 [page_start, page_start + probe_slot_count) 作为 UI 预绑窗口，
     * 不再用 page_end 截断（page_end 是当前可见页尾，会让下一页 cell 永远拿不到 path）。
     * 上限只受 total_count 约束；末页时第二段自然就是空 probe（cover_id=0），timer 会跳过。
     */
    for (i = 0U; i < adapter->probe_slot_count; ++i) {
        lv_cover_slot_probe_t *probe = &adapter->probe_slots[i];
        uint32_t slot_id = page_start + i;

        memset(probe, 0, sizeof(*probe));
        if (slot_id < adapter->total_count) {
            probe->slot_id = slot_id;
            probe->cover_id = slot_id + 1U;
        }
    }
    (void)page_end;
}

static void cover_prefetch_page_window(lv_cover_adapter_t *adapter,
                                        uint32_t visible_start,
                                        uint32_t visible_end,
                                        uint32_t page_start,
                                        uint32_t page_end)
{
    uint32_t visible;
    uint32_t page_size;
    uint32_t page_index;
    uint32_t prefetch_start;
    uint32_t prefetch_end;
    uint32_t download_start;
    uint32_t download_end;
    uint32_t retain_start;
    uint32_t retain_end;
    uint32_t retain_before;
    uint32_t retain_after;
    uint32_t download_slots;

    if (adapter == NULL || adapter->total_count == 0U) return;

    visible = cover_visible_count(adapter);
    page_size = cover_safe_u32(adapter->json_page_size, visible);
    if (visible_start >= adapter->total_count) visible_start = adapter->total_count - 1U;
    if (visible_end >= adapter->total_count) visible_end = adapter->total_count - 1U;
    if (visible_end < visible_start) visible_end = visible_start;
    if (page_start >= adapter->total_count) return;
    if (page_end >= adapter->total_count) page_end = adapter->total_count - 1U;

    page_index = page_start / page_size;
    cover_calc_prefetch_window(adapter, page_start, page_end, page_size, &prefetch_start, &prefetch_end);

    download_slots = visible * COVER_IMAGE_DOWNLOAD_PAGES;
    if (download_slots < visible) download_slots = visible;
    download_start = visible_start;
    if (adapter->scroll_direction < 0) {
        download_start = cover_sub_pages(visible_start, visible, COVER_IMAGE_DOWNLOAD_PAGES - 1U);
        download_end = visible_end;
    } else {
        download_end = cover_add_pages_clamped(adapter, visible_end, visible, COVER_IMAGE_DOWNLOAD_PAGES - 1U);
    }
    if (download_end < download_start || (download_end - download_start + 1U) > download_slots) {
        download_end = cover_add_pages_clamped(adapter, download_start, 1U, download_slots - 1U);
    }

    retain_before = visible * 2U;
    if (COVER_READY_RETAIN_LIMIT > visible + retain_before) {
        retain_after = COVER_READY_RETAIN_LIMIT - visible - retain_before;
    } else {
        retain_after = visible;
    }
    retain_start = (visible_start > retain_before) ? (visible_start - retain_before) : 0U;
    retain_end = cover_add_pages_clamped(adapter, visible_end, 1U, retain_after);

    adapter->debug_last_page_index = page_index;
    adapter->debug_last_visible_start = visible_start;
    COVER_LOGD("prefetch page=%u dir=%d visible_page=%u-%u request=%u-%u",
                page_index,
                adapter->scroll_direction,
                page_start,
                page_end,
                prefetch_start,
                prefetch_end);

    cover_probe_reset_for_page(adapter, visible_start, visible_end);

    /*
     * 这里故意拆成三套范围，避免再把 json_page_size、ui_batch_size、page_end
     * 混在一起：
     *
     * 1. visible_start/end：真实屏幕可见 cell，一般 2x4 = 8 个。只有这 8 个
     *    是 HIGH 优先级，绝不能把 JSON 页尾 page_end(常见 50 个)当可见范围。
     * 2. download_start/end：允许下载图片的小窗口，默认当前可见 + 下一屏 probe。
     *    JSON 解析出 50 个 URL 时，只有落在这个窗口里的 slot 会发 need_image。
     * 3. retain_start/end：本地 READY 文件保留窗口，离 UI 太远的图片由
     *    image_manager 主动 remove；entry/URL 仍保留，需要时可重新下载。
     *
     * cover_ui_scroll_range 下面先按 JSON 页预取范围 access，用来提前请求 page
     * JSON；图片是否下载由 download 窗口二次拦住。预取之后再 access 一次真实
     * 可见范围，把 image_manager 的 focus_slot_id 拉回当前屏，避免删图策略
     * 误以为焦点在 JSON 预取窗口的最远端。
     */
    img_mgr_set_visible_range(visible_start + 1U, visible_end + 1U);
    img_mgr_set_image_active_range(download_start + 1U,
                                   download_end + 1U,
                                   retain_start + 1U,
                                   retain_end + 1U);

    cover_ui_scroll_range(prefetch_start + 1U, prefetch_end + 1U);
    cover_ui_scroll_range(visible_start + 1U, visible_end + 1U);
}

static void cover_apply_image_path(lv_cover_adapter_t *adapter, uint32_t slot_id, const char *path)
{
    lv_cover_item_t item;

    if (adapter == NULL || path == NULL || path[0] == '\0' || slot_id >= adapter->total_count) return;
    cover_ensure_item_ready(adapter, slot_id);
    if (!lv_cover_catalog_get_item(&adapter->catalog, slot_id, &item)) return;

    item.cover_ready = 1U;
    item.cover_loading = 0U;
    snprintf(item.cover_local_path, sizeof(item.cover_local_path), "%s", path);
    (void)lv_cover_catalog_set_item_by_slot(&adapter->catalog, slot_id, &item);
    if (adapter->vlist != NULL) lv_vlist_notify_item_changed(adapter->vlist, slot_id);
}

static void cover_mark_loading(lv_cover_adapter_t *adapter, uint32_t slot_id, uint8_t loading)
{
    lv_cover_item_t item;
    uint8_t changed = 0U;

    if (adapter == NULL || slot_id >= adapter->total_count) return;
    cover_ensure_item_ready(adapter, slot_id);
    if (!lv_cover_catalog_get_item(&adapter->catalog, slot_id, &item)) return;

    if (item.cover_ready) {
        /* image_manager 可能已经按保留窗口删掉远处本地文件。再次 probe 到这个
         * slot 但 pull 不到 path 时，必须清掉 catalog 里的旧路径，避免 LVGL
         * 继续渲染一个已经 remove 的 /tmp 文件。 */
        item.cover_ready = 0U;
        item.cover_local_path[0] = '\0';
        changed = 1U;
    }
    if (item.cover_loading != (loading ? 1U : 0U)) {
        item.cover_loading = loading ? 1U : 0U;
        changed = 1U;
    }
    if (changed != 0U) {
        (void)lv_cover_catalog_set_item_by_slot(&adapter->catalog, slot_id, &item);
        if (adapter->vlist != NULL) lv_vlist_notify_item_changed(adapter->vlist, slot_id);
    }
}

static void cover_apply_meta(lv_cover_adapter_t *adapter,
                              uint32_t slot_id,
                              uint32_t content_id,
                              const char *name_str)
{
    lv_cover_item_t item;
    uint8_t changed = 0U;

    if (adapter == NULL || slot_id >= adapter->total_count) return;
    if (content_id == 0U && (name_str == NULL || name_str[0] == '\0')) return;

    cover_ensure_item_ready(adapter, slot_id);
    if (!lv_cover_catalog_get_item(&adapter->catalog, slot_id, &item)) return;

    if (content_id != 0U && item.cover_id != content_id) {
        item.cover_id = content_id;
        changed = 1U;
    }

    if (name_str != NULL && name_str[0] != '\0' && strcmp(item.name, name_str) != 0) {
        snprintf(item.name, sizeof(item.name), "%s", name_str);
        changed = 1U;
    }

    if (changed != 0U) {
        (void)lv_cover_catalog_set_item_by_slot(&adapter->catalog, slot_id, &item);
        if (adapter->vlist != NULL) lv_vlist_notify_item_changed(adapter->vlist, slot_id);
    }
}

static void cover_ensure_item_ready(lv_cover_adapter_t *adapter, uint32_t item_id)
{
    lv_cover_item_t item;

    if (adapter == NULL || item_id >= adapter->total_count) return;
    if (!lv_cover_catalog_get_item(&adapter->catalog, item_id, &item)) return;
    if (item.ready) return;

    memset(&item, 0, sizeof(item));
    
    item.slot_id = item_id;
    item.cover_id = 0U;
    item.ready = 1U;
    cover_build_default_name(item.cover_id, item.name, sizeof(item.name));
    item.cover_local_path[0] = '\0';

    (void)lv_cover_catalog_set_item_by_slot(&adapter->catalog, item_id, &item);
}

static uint32_t cover_get_count(void *user_ctx)
{
    lv_cover_adapter_t *adapter = (lv_cover_adapter_t *)user_ctx;
    return (adapter != NULL) ? adapter->total_count : 0U;
}

static bool cover_get_item(uint32_t index, lv_vlist_item_t *out, void *user_ctx)
{
    lv_cover_adapter_t *adapter = (lv_cover_adapter_t *)user_ctx;
    uint32_t cols;
    uint32_t row_first;

    if (adapter == NULL || out == NULL || index >= adapter->total_count) return false;

    cols = cover_safe_u32(adapter->view_style ? adapter->view_style->visible_cols : 1U, 1U);
    row_first = index - (index % cols);
    (void)row_first;

    cover_ensure_item_ready(adapter, index);
    return lv_cover_catalog_get_vlist_item(&adapter->catalog, index, out);
}

static void cover_request_image(uint32_t item_id, uint32_t index, void *user_ctx)
{
    (void)item_id;
    (void)index;
    (void)user_ctx;
}

static const char *cover_hit_area_name(const lv_cover_row_style_t *style, lv_coord_t x, lv_coord_t y)
{
    if (style == NULL) return "cell";

    if (x >= style->avatar_x && x < style->avatar_x + style->avatar_w &&
        y >= style->avatar_y && y < style->avatar_y + style->avatar_h) {
        return "image";
    }

    if (x >= style->name_x && x < style->name_x + style->name_w &&
        y >= style->name_y) {
        return "name";
    }

    return "cell";
}

__attribute__((weak)) void app_cover_on_video_clicked(uint32_t video_id,
                                                        const char *title,
                                                        const char *play_url,
                                                        const char *description)
{
    (void)video_id;
    (void)title;
    (void)play_url;
    (void)description;
}

static void cover_jump_async(void *p)
{
    cover_slot_meta_info_t *info = (cover_slot_meta_info_t *)p;

    if (info == NULL) {
        g_cover_jump_pending = 0U;
        return;
    }

    COVER_LOGI("click video: id=%u title=%s url=%s desc=%s",
               info->content_id,
               info->title[0] ? info->title : "",
               info->play_url[0] ? info->play_url : "",
               info->description[0] ? info->description : "");

    app_cover_on_video_clicked(info->content_id,
                               info->title,
                               info->play_url,
                               info->description);

    free(info);
    g_cover_jump_pending = 0U;
}

static uint8_t cover_should_block_click(const lv_cover_adapter_t *adapter)
{
    if (adapter == NULL) return 1U;

    if (adapter->last_scroll_tick != 0U && lv_tick_elaps(adapter->last_scroll_tick) < 360U) {
        return 1U;
    }

    return 0U;
}

static void cover_clear_focused_item(lv_cover_adapter_t *adapter)
{
    if (adapter == NULL || !g_cover_has_focused_item) return;

    if (g_cover_focused_item_id < adapter->total_count) {
        lv_cover_catalog_set_selected(&adapter->catalog, g_cover_focused_item_id, false);
        if (adapter->vlist != NULL) lv_vlist_notify_item_changed(adapter->vlist, g_cover_focused_item_id);
    }
    g_cover_has_focused_item = 0U;
    g_cover_focused_item_id = 0U;
}

static void cover_focus_item(lv_cover_adapter_t *adapter, uint32_t item_id)
{
    if (adapter == NULL || item_id >= adapter->total_count) return;

    if (g_cover_has_focused_item && g_cover_focused_item_id != item_id &&
        g_cover_focused_item_id < adapter->total_count) {
        lv_cover_catalog_set_selected(&adapter->catalog, g_cover_focused_item_id, false);
        if (adapter->vlist != NULL) lv_vlist_notify_item_changed(adapter->vlist, g_cover_focused_item_id);
    }

    lv_cover_catalog_set_selected(&adapter->catalog, item_id, true);
    g_cover_focused_item_id = item_id;
    g_cover_has_focused_item = 1U;
    if (adapter->vlist != NULL) lv_vlist_notify_item_changed(adapter->vlist, item_id);
}

static void cover_on_item_click(void *user_ctx,
                                 uint32_t item_id,
                                 uint32_t bound_index,
                                 lv_coord_t rel_x,
                                 lv_coord_t rel_y)
{
    lv_cover_adapter_t *adapter = (lv_cover_adapter_t *)user_ctx;
    lv_cover_item_t item;
    
    if (adapter == NULL || g_cover_jump_pending) return;

    if (cover_should_block_click(adapter)) {
        COVER_LOGD("click suppressed: item=%u bound=%u rel=(%d,%d) elapsed_since_scroll=%u top=%u",
                    item_id,
                    bound_index,
                    (int)rel_x,
                    (int)rel_y,
                    lv_tick_elaps(adapter->last_scroll_tick),
                    adapter->has_last_top_index ? adapter->last_top_index : 0U);
        return;
    }

    cover_ensure_item_ready(adapter, item_id);
    if (!lv_cover_catalog_get_item(&adapter->catalog, item_id, &item)) return;
    if (adapter->view_style == NULL) return;
    if (item.cover_id == 0U || item.name[0] == '\0') return;

    {
        cover_slot_meta_info_t *info = (cover_slot_meta_info_t *)calloc(1U, sizeof(*info));
        if (info == NULL) return;
        if (cover_media_get_slot_full_meta(item_id + 1U, info) != 0) {
            free(info);
            return;
        }
        if (info->content_id == 0U || info->play_url[0] == '\0') {
            free(info);
            return;
        }

        record_last_clicked_url(info->play_url);

        if (!item.selected) {
            cover_focus_item(adapter, item_id);
            COVER_LOGD("focus: id=%u title=%s area=%s",
                       info->content_id,
                       info->title[0] ? info->title : item.name,
                       cover_hit_area_name(&adapter->view_style->row_style, rel_x, rel_y));
            free(info);
            return;
        }

        g_cover_jump_pending = 1U;
        g_clicked_cover_id = info->content_id;
        snprintf(g_clicked_cover_name, sizeof(g_clicked_cover_name), "%s",
                 info->title[0] ? info->title : item.name);
        COVER_LOGD("click: id=%u title=%s area=%s",
                   info->content_id,
                   info->title[0] ? info->title : item.name,
                   cover_hit_area_name(&adapter->view_style->row_style, rel_x, rel_y));

        lv_async_call(cover_jump_async, info);//可能是臃肿代码



        page_nav_push("screen_2",&guider_ui.screen_2,&guider_ui.screen_2_del,setup_scr_screen_2);
        ktv_cloud_order_resolved_t resolved;
        char current_json[8192];
        char songinfo_json[16384];

        ktv_cloud_order_resolve_first_play_param("16666666666",
                                                get_usr_token(),
                                                1,   // 或 1
                                                0,
                                                info->play_url,
                                                &resolved,
                                                current_json,
                                                sizeof(current_json),
                                                songinfo_json,
                                                sizeof(songinfo_json));
        ktv_player_ui_play(&resolved.play_param);

    }
}

static void cover_apply_real_total_if_ready(lv_cover_adapter_t *adapter)
{
    uint32_t real_total;
    uint32_t display_total;

    if (adapter == NULL) return;
    if (!cover_media_is_total_ready()) return;

    real_total = cover_media_get_cached_total_count();
    display_total = (real_total == 0U) ? 1U : real_total;
    if (display_total == adapter->total_count) return;

    COVER_LOGI("real total applied: old=%u new=%u raw=%u",
               adapter->total_count, display_total, real_total);

    adapter->total_count = display_total;
    (void)lv_cover_catalog_reset(&adapter->catalog, display_total);
    adapter->dirty_cover_count = 0U;
    adapter->debug_last_page_index = 0xFFFFFFFFU;
    adapter->debug_last_visible_start = 0xFFFFFFFFU;
    adapter->has_last_top_index = 0U;
    adapter->last_top_index = 0U;
    adapter->last_scroll_tick = 0U;
    adapter->scroll_direction = 0;
    if (adapter->probe_slots != NULL && adapter->probe_slot_count > 0U) {
        memset(adapter->probe_slots, 0, sizeof(lv_cover_slot_probe_t) * adapter->probe_slot_count);
    }
    if (adapter->vlist != NULL) {
        lv_vlist_reload(adapter->vlist);
        lv_vlist_scroll_to(adapter->vlist, 0U);
    }
}

static void cover_ui_timer_cb(lv_timer_t *timer)
{
    lv_cover_adapter_t *adapter = (lv_cover_adapter_t *)timer->user_data;
    uint32_t start;
    uint32_t end;
    uint32_t visible;
    uint32_t i;
    uint32_t page_size;
    uint32_t page_index;
    uint32_t page_start;
    uint32_t page_end;

    if (adapter == NULL || adapter->vlist == NULL || adapter->total_count == 0U) return;

    cover_apply_real_total_if_ready(adapter);

    visible = cover_visible_count(adapter);
    start = lv_vlist_get_top_index(adapter->vlist);

    if (!adapter->has_last_top_index) {
        adapter->has_last_top_index = 1U;
        adapter->last_top_index = start;
    } else if (start != adapter->last_top_index) {
        adapter->scroll_direction = (start > adapter->last_top_index) ? 1 : -1;
        adapter->last_scroll_tick = lv_tick_get();
        COVER_LOGD("scroll move: old_top=%u new_top=%u dir=%d suppress=360ms",
                    adapter->last_top_index,
                    start,
                    adapter->scroll_direction);
        adapter->last_top_index = start;
    } else if (adapter->scroll_direction != 0 && lv_tick_elaps(adapter->last_scroll_tick) >= 360U) {
        COVER_LOGD("scroll settled: top=%u dir=%d",
                    start,
                    adapter->scroll_direction);
        adapter->scroll_direction = 0;
    }

    end = start + visible - 1U;
    if (end >= adapter->total_count) end = adapter->total_count - 1U;

    page_size = cover_safe_u32(adapter->json_page_size, visible);
    page_index = start / page_size;
    page_start = page_index * page_size;
    page_end = page_start + page_size - 1U;
    if (page_end >= adapter->total_count) page_end = adapter->total_count - 1U;

    if (adapter->debug_last_page_index != page_index ||
        adapter->debug_last_visible_start != start) {
        cover_prefetch_page_window(adapter, start, end, page_start, page_end);
    }

    for (i = 0U; i < adapter->probe_slot_count; ++i) {
        lv_cover_slot_probe_t *probe = &adapter->probe_slots[i];
        const char *path = NULL;
        const char *name_str = NULL;
        uint32_t content_id = 0U;
        uint8_t need_meta;
        uint8_t need_path;

        if (probe->cover_id == 0U) continue;

        need_meta = (probe->meta_ready == 0U) ? 1U : 0U;
        need_path = (!probe->path_ready && probe->query_count < COVER_QUERY_MAX_COUNT) ? 1U : 0U;
        if (!need_meta && !need_path) continue;

        if (need_path) {
            probe->query_count++;
        }

        path = cover_ui_get_image_path(probe->slot_id + 1U, &content_id, &name_str);

        if (content_id != 0U) {
            probe->content_id = content_id;
            probe->cover_id = content_id;
        }
        if (name_str != NULL && name_str[0] != '\0') {
            snprintf(probe->name, sizeof(probe->name), "%s", name_str);
            probe->meta_ready = 1U;
        } else if (content_id != 0U) {
            probe->meta_ready = 1U;
        }

        if (probe->meta_ready) {
            cover_apply_meta(adapter, probe->slot_id, probe->content_id, probe->name);
        }

        if (path != NULL && path[0] != '\0') {
            probe->path_ready = 1U;
            snprintf(probe->path, sizeof(probe->path), "%s", path);
            cover_apply_image_path(adapter, probe->slot_id, probe->path);
        } else if (!probe->path_ready) {
            cover_mark_loading(adapter, probe->slot_id,
                                (probe->query_count < COVER_QUERY_MAX_COUNT) ? 1U : 0U);
        }
    }
}

int lv_cover_adapter_start(lv_cover_adapter_t *adapter,
                            const lv_cover_view_style_t *view_style,
                            uint32_t total_count,
                            uint32_t json_page_size)
{
    if (adapter == NULL || view_style == NULL || total_count == 0U) return -1;

    memset(adapter, 0, sizeof(*adapter));
    g_cover_jump_pending = 0U;
    g_cover_has_focused_item = 0U;
    g_cover_focused_item_id = 0U;
    g_clicked_cover_name[0] = '\0';
    adapter->view_style = view_style;
    adapter->total_count = total_count;
    /* 两个 size 分清：
     * - json_page_size：服务端一页 JSON 有多少条，常见 50，只用于算 page_index。
     * - ui_batch_size：屏幕真实一批 cell 数，2x4 就是 8，只用于 probe/UI 预绑。
     * 以前把 ui_batch_size 设成 json_page_size，会让 probe_slots=100，首屏疯狂查图。 */
    {
        uint32_t visible = cover_visible_count(adapter);
        adapter->json_page_size = (json_page_size >= visible) ? json_page_size : visible;
        adapter->ui_batch_size = visible;
    }
    adapter->debug_last_page_index = 0xFFFFFFFFU;
    adapter->debug_last_visible_start = 0xFFFFFFFFU;
    adapter->last_top_index = 0U;
    adapter->last_scroll_tick = 0U;
    adapter->scroll_direction = 0;
    adapter->has_last_top_index = 0U;

    if (!lv_cover_catalog_init(&adapter->catalog, total_count)) return -1;

    /* probe 同时盯当前页 + 下一页，让下一页 8 个 cell 的 lv_img_set_src 在
     * 用户还在当前页时就提前发出去；网络预拉范围不变。 */
    adapter->probe_slot_count = adapter->ui_batch_size * COVER_PROBE_PAGES;
    adapter->probe_slots = (lv_cover_slot_probe_t *)calloc(adapter->probe_slot_count, sizeof(lv_cover_slot_probe_t));
    if (adapter->probe_slots == NULL) {
        lv_cover_catalog_deinit(&adapter->catalog);
        return -1;
    }

    pthread_mutex_init(&adapter->pending_lock, NULL);
    adapter->ui_timer = lv_timer_create(cover_ui_timer_cb, 200U, adapter);
    adapter->started = 1U;
    g_cover_default = adapter;
    COVER_LOGI("adapter_start ok: total=%u ui_batch=%u probe_slots=%u probe_pages=%u",
               total_count, adapter->ui_batch_size, adapter->probe_slot_count, (unsigned)COVER_PROBE_PAGES);
    COVER_LOGI("BUILD MARK cover-v1: video_categories + local-random-cover + desc-below-image");
    return 0;
}

void lv_cover_adapter_stop(lv_cover_adapter_t *adapter)
{
    if (adapter == NULL) return;

    cover_clear_focused_item(adapter);

    if (adapter->ui_timer != NULL) {
        lv_timer_del(adapter->ui_timer);
        adapter->ui_timer = NULL;
    }

    if (adapter->vlist != NULL) {
        lv_vlist_destroy(adapter->vlist);
        adapter->vlist = NULL;
    }

    if (adapter->started) {
        pthread_mutex_destroy(&adapter->pending_lock);
    }

    free(adapter->probe_slots);
    adapter->probe_slots = NULL;
    adapter->probe_slot_count = 0U;
    lv_cover_catalog_deinit(&adapter->catalog);

    if (g_cover_default == adapter) g_cover_default = NULL;
    g_cover_jump_pending = 0U;
    g_clicked_cover_id = 0U;
    g_cover_has_focused_item = 0U;
    g_cover_focused_item_id = 0U;
    g_clicked_cover_name[0] = '\0';
    memset(adapter, 0, sizeof(*adapter));
}

lv_vlist_t *lv_cover_adapter_create_vlist(lv_cover_adapter_t *adapter, lv_obj_t *parent)
{
    lv_vlist_config_t cfg;
    lv_vlist_ops_t ops;

    if (adapter == NULL || parent == NULL || adapter->view_style == NULL) return NULL;

    memset(&cfg, 0, sizeof(cfg));
    cfg.parent = parent;
    cfg.visible_rows = cover_safe_u32(adapter->view_style->visible_rows, 1U);
    cfg.visible_cols = cover_safe_u32(adapter->view_style->visible_cols, 1U);
    cfg.overscan_rows_front = adapter->view_style->overscan_rows_front;
    cfg.overscan_rows_back = adapter->view_style->overscan_rows_back;
    cfg.preload_before = adapter->view_style->preload_before;
    cfg.preload_after = adapter->view_style->preload_after;
    cfg.viewport_width = adapter->view_style->viewport_width;
    cfg.viewport_height = adapter->view_style->viewport_height;
    cfg.cell_width = adapter->view_style->cell_width;
    cfg.cell_height = adapter->view_style->cell_height;
    cfg.gap_x = adapter->view_style->gap_x;
    cfg.gap_y = adapter->view_style->gap_y;
    cfg.user_ctx = adapter;
    cfg.renderer_ops = &g_lv_renderer_cover_ops;
    cfg.renderer_style = &adapter->view_style->row_style;
    cfg.on_item_click = cover_on_item_click;

    memset(&ops, 0, sizeof(ops));
    ops.get_count = cover_get_count;
    ops.get_item = cover_get_item;
    ops.request_image = cover_request_image;

    adapter->vlist = lv_vlist_create(&cfg, &ops);
    if (adapter->vlist != NULL) {
        lv_vlist_request_visible_images(adapter->vlist);
    }
    return adapter->vlist;
}

void lv_cover_adapter_prime_first_screen(lv_cover_adapter_t *adapter)
{
    uint32_t visible;
    uint32_t page_size;
    uint32_t page_end;

    if (adapter == NULL || adapter->total_count == 0U) return;

    visible = cover_visible_count(adapter);
    page_size = cover_safe_u32(adapter->json_page_size, visible);
    page_end = page_size - 1U;
    if (page_end >= adapter->total_count) page_end = adapter->total_count - 1U;

    cover_prefetch_page_window(adapter, 0U, visible - 1U, 0U, page_end);
}

void lv_cover_adapter_reset(lv_cover_adapter_t *adapter, uint32_t total_count)
{
    if (adapter == NULL) return;

    cover_clear_focused_item(adapter);

    adapter->total_count = total_count;
    (void)lv_cover_catalog_reset(&adapter->catalog, total_count);
    adapter->dirty_cover_count = 0U;
    adapter->debug_last_page_index = 0xFFFFFFFFU;
    adapter->debug_last_visible_start = 0xFFFFFFFFU;
    adapter->has_hint_first_visible = 0U;
    adapter->hint_first_visible = 0U;
    adapter->last_top_index = 0U;
    adapter->last_scroll_tick = 0U;
    adapter->scroll_direction = 0;
    adapter->has_last_top_index = 0U;
    adapter->probe_page_start = 0U;
    adapter->probe_page_end = 0U;
    if (adapter->probe_slots != NULL && adapter->probe_slot_count > 0U) {
        memset(adapter->probe_slots, 0, sizeof(lv_cover_slot_probe_t) * adapter->probe_slot_count);
    }
    g_cover_jump_pending = 0U;
    g_clicked_cover_id = 0U;
    g_cover_has_focused_item = 0U;
    g_cover_focused_item_id = 0U;
    g_clicked_cover_name[0] = '\0';

    if (adapter->vlist != NULL) lv_vlist_reload(adapter->vlist);
}

bool lv_cover_adapter_get_business_item(lv_cover_adapter_t *adapter, uint32_t item_id, lv_cover_item_t *out)
{
    if (adapter == NULL) return false;
    cover_ensure_item_ready(adapter, item_id);
    return lv_cover_catalog_get_item(&adapter->catalog, item_id, out);
}

void lv_cover_adapter_toggle_selected(lv_cover_adapter_t *adapter, uint32_t item_id)
{
    lv_cover_item_t item;

    if (adapter == NULL) return;
    cover_ensure_item_ready(adapter, item_id);
    if (!lv_cover_catalog_get_item(&adapter->catalog, item_id, &item)) return;

    lv_cover_catalog_set_selected(&adapter->catalog, item_id, !item.selected);
    if (adapter->vlist != NULL) lv_vlist_notify_item_changed(adapter->vlist, item_id);
}

void lv_cover_adapter_notify_cover_ready_id(uint32_t cover_id)
{
    lv_cover_adapter_t *adapter = g_cover_default;
    uint32_t i;

    if (adapter == NULL || cover_id == 0U || adapter->probe_slots == NULL) return;

    for (i = 0U; i < adapter->probe_slot_count; ++i) {
        lv_cover_slot_probe_t *probe = &adapter->probe_slots[i];
        if (probe->cover_id == cover_id && probe->path_ready) {
            cover_apply_image_path(adapter, probe->slot_id, probe->path);
            break;
        }
    }
}

lv_cover_adapter_t *lv_cover_adapter_get_default(void)
{
    return g_cover_default;
}
