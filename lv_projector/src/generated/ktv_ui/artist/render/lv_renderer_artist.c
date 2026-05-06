#include "lv_renderer_artist.h"
#include "lv_artist_adapter.h"
#include "lv_artist_catalog.h"
#include "lv_artist_view_style.h"

#include <stdlib.h>
#include <string.h>


#define ARTIST_IMG_ZOOM_DEFAULT 256

static void artist_set_bubble_flags(lv_obj_t *obj)
{
    if (obj == NULL) return;
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

typedef struct {
    lv_obj_t *root;
    lv_obj_t *avatar_bg;
    lv_obj_t *avatar_img;
    lv_obj_t *name_bar;
    lv_obj_t *lbl_name;
    char img_src[LV_ARTIST_AVATAR_PATH_MAX + 8];
} lv_renderer_artist_cell_t;

static const lv_artist_row_style_t *artist_style_of(lv_vlist_t *vlist)
{
    const lv_vlist_config_t *cfg = lv_vlist_get_config(vlist);
    return (const lv_artist_row_style_t *)cfg->renderer_style;
}

static lv_obj_t *make_label(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w)
{
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_set_pos(lbl, x, y);
    if (w > 0) lv_obj_set_width(lbl, w);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    return lbl;
}

static void set_name_label(lv_obj_t *obj, const char *text, const lv_artist_row_style_t *s)
{
    if (obj == NULL || s == NULL) return;
    lv_label_set_text(obj, (text != NULL) ? text : "");
    lv_obj_set_style_text_color(obj, s->name_color, 0);
    lv_obj_set_style_text_opa(obj, s->name_opa, 0);
    lv_obj_set_style_text_font(obj, s->name_font, 0);
}

static void build_lvgl_fs_path(const char *local_path, char *out, size_t out_size)
{
    if (out == NULL || out_size == 0U) return;
    out[0] = '\0';

    if (local_path == NULL || local_path[0] == '\0') return;

    if (strncmp(local_path, "S:", 2) == 0) {
        snprintf(out, out_size, "%s", local_path);
        return;
    }

    if (local_path[0] == '/') {
        snprintf(out, out_size, "S:%s", local_path);
        return;
    }

    snprintf(out, out_size, "S:/%s", local_path);
}



static uint16_t artist_calc_cover_zoom(const char *src, lv_coord_t dst_w, lv_coord_t dst_h)
{
    lv_img_header_t header;
    uint32_t zoom_x;
    uint32_t zoom_y;
    uint32_t zoom;

    if (src == NULL || src[0] == '\0' || dst_w <= 0 || dst_h <= 0) {
        return ARTIST_IMG_ZOOM_DEFAULT;
    }

    if (lv_img_decoder_get_info(src, &header) != LV_RES_OK || header.w == 0 || header.h == 0) {
        return ARTIST_IMG_ZOOM_DEFAULT;
    }

    zoom_x = ((uint32_t)dst_w * 256U + (uint32_t)header.w - 1U) / (uint32_t)header.w;
    zoom_y = ((uint32_t)dst_h * 256U + (uint32_t)header.h - 1U) / (uint32_t)header.h;
    zoom = (zoom_x > zoom_y) ? zoom_x : zoom_y;

    if (zoom < 1U) zoom = 1U;
    if (zoom > 4095U) zoom = 4095U;
    return (uint16_t)zoom;
}

static void artist_apply_cover_image(lv_renderer_artist_cell_t *cell, const char *src)
{
    lv_coord_t box_w;
    lv_coord_t box_h;
    uint16_t zoom;

    if (cell == NULL || cell->avatar_img == NULL || cell->avatar_bg == NULL || src == NULL || src[0] == '\0') {
        return;
    }

    box_w = lv_obj_get_width(cell->avatar_bg);
    box_h = lv_obj_get_height(cell->avatar_bg);
    zoom = artist_calc_cover_zoom(src, box_w, box_h);

    lv_img_set_src(cell->avatar_img, src);
    lv_img_set_zoom(cell->avatar_img, zoom);
    lv_obj_center(cell->avatar_img);
    lv_obj_clear_flag(cell->avatar_img, LV_OBJ_FLAG_HIDDEN);
}

static void artist_clear_image(lv_renderer_artist_cell_t *cell)
{
    if (cell == NULL || cell->avatar_img == NULL) return;
    lv_img_set_src(cell->avatar_img, NULL);
    lv_img_set_zoom(cell->avatar_img, ARTIST_IMG_ZOOM_DEFAULT);
    lv_obj_center(cell->avatar_img);
    lv_obj_add_flag(cell->avatar_img, LV_OBJ_FLAG_HIDDEN);
    cell->img_src[0] = '\0';
}

static void *artist_create_cell(lv_vlist_t *vlist, lv_obj_t *parent)
{
    lv_renderer_artist_cell_t *cell;
    const lv_artist_row_style_t *s = artist_style_of(vlist);

    if (s == NULL) return NULL;

    cell = (lv_renderer_artist_cell_t *)calloc(1, sizeof(*cell));
    if (cell == NULL) return NULL;

    cell->root = lv_obj_create(parent);
    lv_obj_set_size(cell->root, s->cell_width, s->cell_height);
    lv_obj_clear_flag(cell->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cell->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(cell->root, 0, 0);
    lv_obj_set_style_radius(cell->root, s->radius, 0);
    lv_obj_set_style_bg_color(cell->root, s->bg_color, 0);
    lv_obj_set_style_bg_opa(cell->root, s->bg_opa, 0);
    lv_obj_set_style_border_color(cell->root, s->border_color, 0);
    lv_obj_set_style_border_width(cell->root, s->border_width, 0);
    artist_set_bubble_flags(cell->root);

    cell->avatar_bg = lv_obj_create(cell->root);
    lv_obj_set_pos(cell->avatar_bg, s->avatar_x, s->avatar_y);
    lv_obj_set_size(cell->avatar_bg, s->avatar_w, s->avatar_h);
    lv_obj_clear_flag(cell->avatar_bg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cell->avatar_bg, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(cell->avatar_bg, 0, 0);
    lv_obj_set_style_radius(cell->avatar_bg, s->avatar_radius, 0);
    lv_obj_set_style_clip_corner(cell->avatar_bg, true, 0);
    lv_obj_set_style_bg_color(cell->avatar_bg, s->avatar_bg_color, 0);
    lv_obj_set_style_bg_opa(cell->avatar_bg, s->avatar_bg_opa, 0);
    lv_obj_set_style_border_width(cell->avatar_bg, 0, 0);
    artist_set_bubble_flags(cell->avatar_bg);

    cell->avatar_img = lv_img_create(cell->avatar_bg);
    lv_img_set_zoom(cell->avatar_img, ARTIST_IMG_ZOOM_DEFAULT);
    lv_obj_center(cell->avatar_img);
    artist_set_bubble_flags(cell->avatar_img);

    if (s->name_h > 0) {
        cell->name_bar = lv_obj_create(cell->root);
        lv_obj_set_pos(cell->name_bar, s->name_x, s->name_y);
        lv_obj_set_size(cell->name_bar, s->name_w, s->name_h);
        lv_obj_clear_flag(cell->name_bar, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scrollbar_mode(cell->name_bar, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_pad_all(cell->name_bar, 0, 0);
        lv_obj_set_style_radius(cell->name_bar, 0, 0);
        lv_obj_set_style_border_width(cell->name_bar, 0, 0);
        /* 仅背景半透明，整体 opa 显式拉满，避免影响里面的文字 */
        lv_obj_set_style_opa(cell->name_bar, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(cell->name_bar, s->name_bg_color, 0);
        lv_obj_set_style_bg_opa(cell->name_bar, s->name_bg_opa, 0);
        /* 默认隐藏，bind_cell 时按 ready/avatar_ready 决定是否露出 */
        lv_obj_add_flag(cell->name_bar, LV_OBJ_FLAG_HIDDEN);
        artist_set_bubble_flags(cell->name_bar);

        cell->lbl_name = lv_label_create(cell->name_bar);
        lv_obj_set_width(cell->lbl_name, s->name_w);
        lv_label_set_long_mode(cell->lbl_name, LV_LABEL_LONG_DOT);
        lv_obj_set_style_text_align(cell->lbl_name, LV_TEXT_ALIGN_CENTER, 0);
        /* 文字固定不透明，本体不要被父背景关联 */
        lv_obj_set_style_opa(cell->lbl_name, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(cell->lbl_name, LV_OPA_TRANSP, 0);
        lv_obj_center(cell->lbl_name);
        artist_set_bubble_flags(cell->lbl_name);
    } else {
        cell->lbl_name = make_label(cell->root, s->name_x, s->name_y, s->name_w);
        artist_set_bubble_flags(cell->lbl_name);
    }
    artist_clear_image(cell);
    return cell;
}

static void artist_destroy_cell(lv_vlist_t *vlist, void *renderer_ctx)
{
    lv_renderer_artist_cell_t *cell = (lv_renderer_artist_cell_t *)renderer_ctx;
    (void)vlist;
    if (cell != NULL) {
        artist_clear_image(cell);
        free(cell);
    }
}

static lv_obj_t *artist_get_root(lv_vlist_t *vlist, void *renderer_ctx)
{
    lv_renderer_artist_cell_t *cell = (lv_renderer_artist_cell_t *)renderer_ctx;
    (void)vlist;
    return cell ? cell->root : NULL;
}

static void artist_bind_cell(lv_vlist_t *vlist, void *renderer_ctx, const lv_vlist_item_t *item)
{
    lv_renderer_artist_cell_t *cell = (lv_renderer_artist_cell_t *)renderer_ctx;
    const lv_vlist_config_t *cfg = lv_vlist_get_config(vlist);
    const lv_artist_row_style_t *s = artist_style_of(vlist);
    lv_artist_adapter_t *adapter;
    lv_artist_item_t biz;

    if (cell == NULL || item == NULL || cfg == NULL || s == NULL) return;

    adapter = (lv_artist_adapter_t *)cfg->user_ctx;
    if (adapter == NULL) return;

    if (!lv_artist_adapter_get_business_item(adapter, item->item_id, &biz)) return;

    if (biz.selected) {
        lv_obj_set_style_bg_color(cell->root, s->checked_bg_color, 0);
        lv_obj_set_style_bg_opa(cell->root, s->checked_bg_opa, 0);
    } else {
        lv_obj_set_style_bg_color(cell->root, s->bg_color, 0);
        lv_obj_set_style_bg_opa(cell->root, s->bg_opa, 0);
    }

    /* 没数据：完全空白，名字条也藏起来 */
    if (!biz.ready) {
        if (cell->name_bar != NULL) {
            lv_obj_add_flag(cell->name_bar, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_label_set_text(cell->lbl_name, "");
        }
        artist_clear_image(cell);
        return;
    }

    /* 有名字就立即显示名字条；图没到位时 avatar 区域透明（不做灰框/text 占位） */
    if (cell->name_bar != NULL) {
        lv_obj_clear_flag(cell->name_bar, LV_OBJ_FLAG_HIDDEN);
    }
    set_name_label(cell->lbl_name, biz.name, s);

    if (biz.avatar_ready && biz.avatar_local_path[0] != '\0') {
        char next_src[sizeof(cell->img_src)];
        build_lvgl_fs_path(biz.avatar_local_path, next_src, sizeof(next_src));
        if (strcmp(cell->img_src, next_src) != 0) {
            artist_clear_image(cell);
            snprintf(cell->img_src, sizeof(cell->img_src), "%s", next_src);
            artist_apply_cover_image(cell, cell->img_src);
        } else {
            artist_apply_cover_image(cell, cell->img_src);
        }
    } else {
        artist_clear_image(cell);
    }
}

static void artist_set_pos(lv_vlist_t *vlist, void *renderer_ctx, lv_coord_t x, lv_coord_t y)
{
    lv_renderer_artist_cell_t *cell = (lv_renderer_artist_cell_t *)renderer_ctx;
    (void)vlist;
    if (cell != NULL && cell->root != NULL) {
        lv_obj_set_pos(cell->root, x, y);
    }
}

const lv_vlist_renderer_ops_t g_lv_renderer_artist_ops = {
    .create_cell = artist_create_cell,
    .destroy_cell = artist_destroy_cell,
    .get_root = artist_get_root,
    .bind_cell = artist_bind_cell,
    .set_pos = artist_set_pos
};
