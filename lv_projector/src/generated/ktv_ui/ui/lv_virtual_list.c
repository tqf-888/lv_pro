#include "lv_virtual_list.h"

#include <stdlib.h>
#include <string.h>

struct lv_vlist {
    lv_vlist_config_t cfg;
    lv_vlist_ops_t ops;
    uint32_t total_items;
    uint32_t total_rows;
    uint32_t top_row;
    uint32_t physical_rows;
    uint32_t physical_count;
    lv_obj_t *root;
    lv_vlist_cell_t *cells;
    bool dragging;
    bool moved;
    lv_coord_t press_x;
    lv_coord_t press_y;
    lv_coord_t last_y;
    int32_t scroll_offset_px;
};

static uint32_t lv_vlist_calc_total_rows(const lv_vlist_t *vlist)
{
    if (vlist == NULL || vlist->cfg.visible_cols == 0U) return 0U;
    return (vlist->total_items + vlist->cfg.visible_cols - 1U) / vlist->cfg.visible_cols;
}

static uint32_t lv_vlist_clamp_top_row(const lv_vlist_t *vlist, uint32_t top_row)
{
    uint32_t max_top_row;
    if (vlist == NULL || vlist->total_rows == 0U) return 0U;
    if (vlist->cfg.visible_rows >= vlist->total_rows) return 0U;
    max_top_row = vlist->total_rows - vlist->cfg.visible_rows;
    return (top_row > max_top_row) ? max_top_row : top_row;
}

const lv_vlist_config_t *lv_vlist_get_config(const lv_vlist_t *vlist)
{
    return (vlist != NULL) ? &vlist->cfg : NULL;
}

static lv_coord_t lv_vlist_calc_step(const lv_vlist_t *vlist)
{
    if (vlist == NULL) return 1;
    return vlist->cfg.cell_height + vlist->cfg.gap_y;
}

static void lv_vlist_bind_one_cell_ex(lv_vlist_t *vlist, lv_vlist_cell_t *cell, uint32_t index, bool force)
{
    lv_vlist_item_t item;
    bool ok;

    if (vlist == NULL || cell == NULL) return;
    if (index >= vlist->total_items) {
        if (cell->root != NULL) lv_obj_add_flag(cell->root, LV_OBJ_FLAG_HIDDEN);
        cell->bound_valid = false;
        return;
    }

    if (!force && cell->bound_valid && cell->bound_index == index) {
        if (cell->root != NULL) lv_obj_clear_flag(cell->root, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    memset(&item, 0, sizeof(item));
    ok = vlist->ops.get_item(index, &item, vlist->cfg.user_ctx);
    if (!ok) {
        if (cell->root != NULL) lv_obj_add_flag(cell->root, LV_OBJ_FLAG_HIDDEN);
        cell->bound_valid = false;
        return;
    }

    if (cell->root != NULL) lv_obj_clear_flag(cell->root, LV_OBJ_FLAG_HIDDEN);
    cell->bound_index = index;
    cell->bound_item_id = item.item_id;
    cell->bound_valid = true;

    if (vlist->cfg.renderer_ops != NULL && vlist->cfg.renderer_ops->bind_cell != NULL) {
        vlist->cfg.renderer_ops->bind_cell(vlist, cell->renderer_ctx, &item);
    }
}

static void lv_vlist_bind_one_cell(lv_vlist_t *vlist, lv_vlist_cell_t *cell, uint32_t index)
{
    lv_vlist_bind_one_cell_ex(vlist, cell, index, false);
}

static void lv_vlist_refresh_cells(lv_vlist_t *vlist)
{
    uint32_t first_row;
    uint32_t effective_front_rows;
    uint32_t r, c;
    lv_coord_t base_y;
    uint32_t cell_i = 0U;

    if (vlist == NULL) return;

    /*
     * 关键边界修复：
     *
     * overscan_rows_front 的意义是“在当前可视起始行之前，额外保留多少行物理 cell”，
     * 它只能作用在“确实存在的前置行”上，不能在 top_row=0 时继续把首屏整体上移。
     *
     * 旧逻辑的问题：
     * - first_row 在 top_row=0 时会被钳成 0
     * - 但 base_y 仍然固定减去 overscan_rows_front * step
     * - 结果第 0 页初始化时，前 1~N 行被摆到 y<0 的位置
     * - 现象就是“首屏第一行/前几行不显示，要滚一下或点一下才正常”
     *
     * 正确做法：前向 overscan 必须和 top_row 取最小值。
     * 也就是首屏没有前置真实行时，effective_front_rows 必须为 0。
     *
     * !!! 警示后人：
     * 以后如果再改布局算法，first_row 与 base_y 必须使用同一份 effective_front_rows。
     * 只改 first_row、不改 base_y，会再次把首屏前几行顶出可视区。
     */
    effective_front_rows = (vlist->top_row < vlist->cfg.overscan_rows_front)
                         ? vlist->top_row
                         : vlist->cfg.overscan_rows_front;

    first_row = vlist->top_row - effective_front_rows;

    base_y = -(lv_coord_t)effective_front_rows * lv_vlist_calc_step(vlist)
           - (lv_coord_t)vlist->scroll_offset_px;

    for (r = 0U; r < vlist->physical_rows; ++r) {
        lv_coord_t y = base_y + (lv_coord_t)r * lv_vlist_calc_step(vlist);
        for (c = 0U; c < vlist->cfg.visible_cols; ++c) {
            uint32_t index = (first_row + r) * vlist->cfg.visible_cols + c;
            lv_coord_t x = (lv_coord_t)c * (vlist->cfg.cell_width + vlist->cfg.gap_x);
            if (cell_i >= vlist->physical_count) return;
            if (vlist->cfg.renderer_ops != NULL && vlist->cfg.renderer_ops->set_pos != NULL) {
                vlist->cfg.renderer_ops->set_pos(vlist, vlist->cells[cell_i].renderer_ctx, x, y);
            }
            lv_vlist_bind_one_cell(vlist, &vlist->cells[cell_i], index);
            cell_i++;
        }
    }
}

static void lv_vlist_request_visible_images_now(lv_vlist_t *vlist)
{
    uint32_t begin_row, end_row, begin, end, i;
    lv_vlist_item_t item;

    if (vlist == NULL || vlist->ops.request_image == NULL || vlist->total_items == 0U) return;

    begin_row = (vlist->top_row > vlist->cfg.preload_before)
              ? (vlist->top_row - vlist->cfg.preload_before) : 0U;
    end_row = vlist->top_row + vlist->cfg.visible_rows + vlist->cfg.preload_after;
    if (end_row > vlist->total_rows) end_row = vlist->total_rows;

    begin = begin_row * vlist->cfg.visible_cols;
    end = end_row * vlist->cfg.visible_cols;
    if (end > vlist->total_items) end = vlist->total_items;

    for (i = begin; i < end; ++i) {
        memset(&item, 0, sizeof(item));
        if (!vlist->ops.get_item(i, &item, vlist->cfg.user_ctx)) continue;
        if (!item.image_requested && item.img_state == LV_VLIST_IMG_NONE) {
            vlist->ops.request_image(item.item_id, i, vlist->cfg.user_ctx);
        }
    }
}

static lv_vlist_cell_t *lv_vlist_find_cell_from_target(lv_vlist_t *vlist, lv_obj_t *target)
{
    uint32_t i;
    lv_obj_t *obj;
    if (vlist == NULL || target == NULL) return NULL;
    obj = target;
    while (obj != NULL && obj != vlist->root) {
        for (i = 0U; i < vlist->physical_count; ++i) {
            if (vlist->cells[i].root == obj) {
                return &vlist->cells[i];
            }
        }
        obj = lv_obj_get_parent(obj);
    }
    return NULL;
}

static void lv_vlist_emit_click(lv_vlist_t *vlist, lv_event_t *e)
{
    lv_vlist_cell_t *cell;
    lv_obj_t *target;
    lv_point_t p;
    lv_area_t a;
    lv_indev_t *indev;

    if (vlist == NULL || e == NULL || vlist->cfg.on_item_click == NULL) return;
    target = lv_event_get_target(e);
    cell = lv_vlist_find_cell_from_target(vlist, target);
    if (cell == NULL || !cell->bound_valid || cell->root == NULL) return;
    indev = lv_indev_get_act();
    if (indev == NULL) return;
    lv_indev_get_point(indev, &p);
    lv_obj_get_coords(cell->root, &a);
    vlist->cfg.on_item_click(vlist->cfg.user_ctx,
                             cell->bound_item_id,
                             cell->bound_index,
                             p.x - a.x1,
                             p.y - a.y1);
}

static void lv_vlist_apply_drag_delta(lv_vlist_t *vlist, int32_t dy)
{
    lv_coord_t step;
    uint32_t old_top_row;
    if (vlist == NULL || dy == 0) return;

    step = lv_vlist_calc_step(vlist);
    if (step <= 0) step = 1;
    old_top_row = vlist->top_row;
    vlist->scroll_offset_px += dy;

    while (vlist->scroll_offset_px >= step) {
        if (vlist->top_row + 1U < vlist->total_rows) {
            vlist->top_row++;
            vlist->scroll_offset_px -= step;
        } else {
            vlist->scroll_offset_px = (int32_t)step - 1;
            break;
        }
    }
    while (vlist->scroll_offset_px <= -step) {
        if (vlist->top_row > 0U) {
            vlist->top_row--;
            vlist->scroll_offset_px += step;
        } else {
            vlist->scroll_offset_px = 0;
            break;
        }
    }

    if (vlist->cfg.visible_rows >= vlist->total_rows) {
        vlist->top_row = 0U;
        vlist->scroll_offset_px = 0;
    }

    if (vlist->top_row == 0U && vlist->scroll_offset_px < 0) {
        vlist->scroll_offset_px = 0;
    }

    if (vlist->total_rows > 0U) {
        uint32_t max_top = lv_vlist_clamp_top_row(vlist, vlist->total_rows);
        if (vlist->top_row >= max_top && vlist->scroll_offset_px > 0) {
            vlist->scroll_offset_px = 0;
        }
    }

    lv_vlist_refresh_cells(vlist);
    if (old_top_row != vlist->top_row) {
        lv_vlist_request_visible_images_now(vlist);
    }
}

static void lv_vlist_finish_page_swipe(lv_vlist_t *vlist, lv_coord_t release_y)
{
#if LV_VLIST_ENABLE_PAGE_SWIPE
    int32_t total_dy;

    if (vlist == NULL) return;
    total_dy = (int32_t)release_y - (int32_t)vlist->press_y;

    if (LV_ABS(total_dy) >= LV_VLIST_PAGE_SWIPE_THRESHOLD_PX) {
        if (total_dy < 0) {
            lv_vlist_scroll_by(vlist, (int32_t)vlist->cfg.visible_rows);
        } else if (total_dy > 0) {
            lv_vlist_scroll_by(vlist, -(int32_t)vlist->cfg.visible_rows);
        }
    } else {
        lv_vlist_refresh_cells(vlist);
        lv_vlist_request_visible_images_now(vlist);
    }
#else
    LV_UNUSED(vlist);
    LV_UNUSED(release_y);
#endif
}

static void lv_vlist_input_event_cb(lv_event_t *e)
{
    lv_vlist_t *vlist;
    lv_indev_t *indev;
    lv_point_t p;
    lv_event_code_t code;

    if (e == NULL) return;
    vlist = (lv_vlist_t *)lv_event_get_user_data(e);
    if (vlist == NULL) return;

    code = lv_event_get_code(e);
    indev = lv_indev_get_act();

    switch (code) {
    case LV_EVENT_PRESSED:
        if (indev != NULL) {
            lv_indev_get_point(indev, &p);
            vlist->dragging = true;
            vlist->moved = false;
            vlist->press_x = p.x;
            vlist->press_y = p.y;
            vlist->last_y = p.y;
        }
        break;
    case LV_EVENT_PRESSING:
        if (vlist->dragging && indev != NULL) {
            lv_indev_get_point(indev, &p);
            if (LV_ABS((int32_t)p.y - (int32_t)vlist->press_y) > 3 || LV_ABS((int32_t)p.x - (int32_t)vlist->press_x) > 3) {
                vlist->moved = true;
            }
#if LV_VLIST_ENABLE_PAGE_SWIPE && LV_VLIST_PAGE_SWIPE_STRICT
            vlist->last_y = p.y;
#else
            {
                int32_t dy = (int32_t)vlist->last_y - (int32_t)p.y;
                if (dy != 0) {
                    vlist->last_y = p.y;
                    lv_vlist_apply_drag_delta(vlist, dy);
                }
            }
#endif
        }
        break;
    case LV_EVENT_RELEASED:
        if (indev != NULL) {
            lv_indev_get_point(indev, &p);
        }
        if (!vlist->moved) {
            lv_vlist_emit_click(vlist, e);
        } else {
#if LV_VLIST_ENABLE_PAGE_SWIPE && LV_VLIST_PAGE_SWIPE_STRICT
            lv_vlist_finish_page_swipe(vlist, p.y);
#else
            lv_vlist_request_visible_images_now(vlist);
#endif
        }
        vlist->dragging = false;
        vlist->moved = false;
        break;
    case LV_EVENT_PRESS_LOST:
    case LV_EVENT_GESTURE:
#if LV_VLIST_ENABLE_PAGE_SWIPE && LV_VLIST_PAGE_SWIPE_STRICT
        if (vlist->dragging && vlist->moved && indev != NULL) {
            lv_indev_get_point(indev, &p);
            lv_vlist_finish_page_swipe(vlist, p.y);
        }
#endif
        vlist->dragging = false;
        vlist->moved = false;
        break;
    default:
        break;
    }
}

lv_vlist_t *lv_vlist_create(const lv_vlist_config_t *cfg, const lv_vlist_ops_t *ops)
{
    lv_vlist_t *vlist;
    uint32_t i;

    if (cfg == NULL || ops == NULL || cfg->parent == NULL ||
        cfg->visible_rows == 0U || cfg->visible_cols == 0U ||
        cfg->renderer_ops == NULL || cfg->renderer_ops->create_cell == NULL || cfg->renderer_ops->get_root == NULL ||
        ops->get_count == NULL || ops->get_item == NULL) {
        return NULL;
    }

    vlist = (lv_vlist_t *)calloc(1, sizeof(*vlist));
    if (vlist == NULL) return NULL;

    vlist->cfg = *cfg;
    vlist->ops = *ops;
    vlist->total_items = ops->get_count(cfg->user_ctx);
    vlist->total_rows = lv_vlist_calc_total_rows(vlist);
    vlist->physical_rows = cfg->visible_rows + cfg->overscan_rows_front + cfg->overscan_rows_back;
    vlist->physical_count = vlist->physical_rows * cfg->visible_cols;
    if (vlist->physical_count == 0U) {
        free(vlist);
        return NULL;
    }

    vlist->cells = (lv_vlist_cell_t *)calloc(vlist->physical_count, sizeof(lv_vlist_cell_t));
    if (vlist->cells == NULL) {
        free(vlist);
        return NULL;
    }

    vlist->root = lv_obj_create(cfg->parent);
    lv_obj_set_size(vlist->root, cfg->viewport_width, cfg->viewport_height);
    lv_obj_set_pos(vlist->root, 0, 0);
    lv_obj_clear_flag(vlist->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(vlist->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(vlist->root, 0, 0);
    lv_obj_set_style_border_width(vlist->root, 0, 0);
    lv_obj_set_style_radius(vlist->root, 0, 0);
    lv_obj_set_style_bg_opa(vlist->root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_clip_corner(vlist->root, true, 0);
    lv_obj_add_event_cb(vlist->root, lv_vlist_input_event_cb, LV_EVENT_ALL, vlist);

    for (i = 0U; i < vlist->physical_count; ++i) {
        vlist->cells[i].renderer_ctx = cfg->renderer_ops->create_cell(vlist, vlist->root);
        if (vlist->cells[i].renderer_ctx == NULL) {
            lv_vlist_destroy(vlist);
            return NULL;
        }
        vlist->cells[i].root = cfg->renderer_ops->get_root(vlist, vlist->cells[i].renderer_ctx);
    }

    lv_vlist_reload(vlist);
    return vlist;
}

void lv_vlist_destroy(lv_vlist_t *vlist)
{
    uint32_t i;
    if (vlist == NULL) return;
    if (vlist->cfg.renderer_ops != NULL && vlist->cfg.renderer_ops->destroy_cell != NULL) {
        for (i = 0U; i < vlist->physical_count; ++i) {
            if (vlist->cells[i].renderer_ctx != NULL) {
                vlist->cfg.renderer_ops->destroy_cell(vlist, vlist->cells[i].renderer_ctx);
            }
        }
    }
    if (vlist->root != NULL) lv_obj_del(vlist->root);
    free(vlist->cells);
    free(vlist);
}

void lv_vlist_reload(lv_vlist_t *vlist)
{
    if (vlist == NULL) return;
    vlist->total_items = vlist->ops.get_count(vlist->cfg.user_ctx);
    vlist->total_rows = lv_vlist_calc_total_rows(vlist);
    vlist->top_row = lv_vlist_clamp_top_row(vlist, vlist->top_row);
    vlist->scroll_offset_px = 0;
    lv_vlist_refresh_cells(vlist);
    lv_vlist_request_visible_images_now(vlist);
}

void lv_vlist_scroll_to(lv_vlist_t *vlist, uint32_t top_index)
{
    uint32_t top_row;
    if (vlist == NULL) return;
    top_row = (vlist->cfg.visible_cols > 0U) ? (top_index / vlist->cfg.visible_cols) : 0U;
    vlist->top_row = lv_vlist_clamp_top_row(vlist, top_row);
    vlist->scroll_offset_px = 0;
    lv_vlist_refresh_cells(vlist);
    lv_vlist_request_visible_images_now(vlist);
}

void lv_vlist_scroll_by(lv_vlist_t *vlist, int32_t delta_rows)
{
    int64_t next;
    if (vlist == NULL) return;
    next = (int64_t)vlist->top_row + (int64_t)delta_rows;
    if (next < 0) next = 0;
    vlist->top_row = lv_vlist_clamp_top_row(vlist, (uint32_t)next);
    vlist->scroll_offset_px = 0;
    lv_vlist_refresh_cells(vlist);
    lv_vlist_request_visible_images_now(vlist);
}

uint32_t lv_vlist_get_top_index(const lv_vlist_t *vlist)
{
    if (vlist == NULL) return 0U;
    return vlist->top_row * vlist->cfg.visible_cols;
}

void lv_vlist_scroll_by_px(lv_vlist_t *vlist, int32_t delta_px)
{
    if (vlist == NULL) return;
    lv_vlist_apply_drag_delta(vlist, delta_px);
}

int32_t lv_vlist_get_scroll_offset_px(const lv_vlist_t *vlist)
{
    return (vlist != NULL) ? vlist->scroll_offset_px : 0;
}

void lv_vlist_notify_item_changed(lv_vlist_t *vlist, uint32_t item_id)
{
    uint32_t i;
    if (vlist == NULL) return;
    for (i = 0U; i < vlist->physical_count; ++i) {
        if (vlist->cells[i].bound_valid && vlist->cells[i].bound_item_id == item_id) {
            lv_vlist_bind_one_cell_ex(vlist, &vlist->cells[i], vlist->cells[i].bound_index, true);
        }
    }
}

void lv_vlist_notify_range_changed(lv_vlist_t *vlist, uint32_t first_index, uint32_t count)
{
    uint32_t i, last_index;
    if (vlist == NULL || count == 0U) return;
    last_index = first_index + count - 1U;
    for (i = 0U; i < vlist->physical_count; ++i) {
        if (vlist->cells[i].bound_valid &&
            vlist->cells[i].bound_index >= first_index &&
            vlist->cells[i].bound_index <= last_index) {
            lv_vlist_bind_one_cell_ex(vlist, &vlist->cells[i], vlist->cells[i].bound_index, true);
        }
    }
}

void lv_vlist_request_visible_images(lv_vlist_t *vlist)
{
    lv_vlist_request_visible_images_now(vlist);
}
