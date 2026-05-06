#include "lv_renderer_generic.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    lv_obj_t *root;
    lv_obj_t *img;
    lv_obj_t *title;
    lv_obj_t *subtitle;
} lv_renderer_generic_cell_t;

static const lv_renderer_layout_style_t *lv_renderer_style_of(lv_vlist_t *vlist)
{
    const lv_vlist_config_t *cfg = lv_vlist_get_config(vlist);
    return (const lv_renderer_layout_style_t *)cfg->renderer_style;
}

static void lv_renderer_set_label_if_changed(lv_obj_t *obj, const char *text)
{
    const char *cur;
    const char *next = (text != NULL) ? text : "";
    if (obj == NULL) return;
    cur = lv_label_get_text(obj);
    if (cur == NULL || strcmp(cur, next) != 0) {
        lv_label_set_text(obj, next);
    }
}

static void lv_renderer_set_img_if_changed(lv_obj_t *obj, const void *src)
{
    const void *cur;
    if (obj == NULL) return;
    cur = lv_img_get_src(obj);
    if (cur != src) lv_img_set_src(obj, src);
}

static void *lv_renderer_create_cell(lv_vlist_t *vlist, lv_obj_t *parent)
{
    lv_renderer_generic_cell_t *cell;
    const lv_renderer_layout_style_t *s = lv_renderer_style_of(vlist);
    if (s == NULL) return NULL;

    cell = (lv_renderer_generic_cell_t *)calloc(1, sizeof(*cell));
    if (cell == NULL) return NULL;

    cell->root = lv_obj_create(parent);
    lv_obj_set_size(cell->root, s->cell_width, s->cell_height);
    lv_obj_clear_flag(cell->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cell->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(cell->root, 0, 0);
    lv_obj_set_style_radius(cell->root, s->cell_radius, 0);
    lv_obj_set_style_bg_color(cell->root, s->bg_color, 0);
    lv_obj_set_style_bg_opa(cell->root, s->bg_opa, 0);
    lv_obj_set_style_border_color(cell->root, s->border_color, 0);
    lv_obj_set_style_border_width(cell->root, s->border_width, 0);
    lv_obj_add_flag(cell->root, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(cell->root, LV_OBJ_FLAG_GESTURE_BUBBLE);

    if (s->show_image) {
        cell->img = lv_img_create(cell->root);
        lv_obj_set_pos(cell->img, s->image_x, s->image_y);
        lv_obj_set_size(cell->img, s->image_w, s->image_h);
        lv_obj_add_flag(cell->img, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_flag(cell->img, LV_OBJ_FLAG_GESTURE_BUBBLE);
    }
    if (s->show_title) {
        cell->title = lv_label_create(cell->root);
        lv_obj_set_pos(cell->title, s->title_x, s->title_y);
        lv_obj_set_width(cell->title, s->title_w);
        lv_label_set_long_mode(cell->title, LV_LABEL_LONG_DOT);
        if (s->title_font) lv_obj_set_style_text_font(cell->title, s->title_font, 0);
        lv_obj_set_style_text_color(cell->title, s->title_color, 0);
        lv_obj_add_flag(cell->title, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_flag(cell->title, LV_OBJ_FLAG_GESTURE_BUBBLE);
    }
    if (s->show_subtitle) {
        cell->subtitle = lv_label_create(cell->root);
        lv_obj_set_pos(cell->subtitle, s->subtitle_x, s->subtitle_y);
        lv_obj_set_width(cell->subtitle, s->subtitle_w);
        lv_label_set_long_mode(cell->subtitle, LV_LABEL_LONG_DOT);
        if (s->subtitle_font) lv_obj_set_style_text_font(cell->subtitle, s->subtitle_font, 0);
        lv_obj_set_style_text_color(cell->subtitle, s->subtitle_color, 0);
        lv_obj_add_flag(cell->subtitle, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_flag(cell->subtitle, LV_OBJ_FLAG_GESTURE_BUBBLE);
    }

    return cell;
}

static void lv_renderer_destroy_cell(lv_vlist_t *vlist, void *renderer_ctx)
{
    (void)vlist;
    free(renderer_ctx);
}

static lv_obj_t *lv_renderer_get_root(lv_vlist_t *vlist, void *renderer_ctx)
{
    lv_renderer_generic_cell_t *cell = (lv_renderer_generic_cell_t *)renderer_ctx;
    (void)vlist;
    return cell ? cell->root : NULL;
}

static void lv_renderer_bind_cell(lv_vlist_t *vlist, void *renderer_ctx, const lv_vlist_item_t *item)
{
    lv_renderer_generic_cell_t *cell = (lv_renderer_generic_cell_t *)renderer_ctx;
    const lv_vlist_config_t *cfg = lv_vlist_get_config(vlist);
    if (cell == NULL || item == NULL || cfg == NULL) return;

    if (cell->title) lv_renderer_set_label_if_changed(cell->title, item->title);
    if (cell->subtitle) lv_renderer_set_label_if_changed(cell->subtitle, item->subtitle);

    if (cell->img) {
        switch (item->img_state) {
        case LV_VLIST_IMG_READY:
            lv_renderer_set_img_if_changed(cell->img,
                (item->img_src != NULL) ? item->img_src : cfg->placeholder_img_src);
            break;
        case LV_VLIST_IMG_FAILED:
            lv_renderer_set_img_if_changed(cell->img, cfg->failed_img_src);
            break;
        case LV_VLIST_IMG_LOADING:
        case LV_VLIST_IMG_NONE:
        default:
            lv_renderer_set_img_if_changed(cell->img, cfg->placeholder_img_src);
            break;
        }
    }
}

static void lv_renderer_set_pos(lv_vlist_t *vlist, void *renderer_ctx, lv_coord_t x, lv_coord_t y)
{
    lv_renderer_generic_cell_t *cell = (lv_renderer_generic_cell_t *)renderer_ctx;
    (void)vlist;
    if (cell != NULL && cell->root != NULL) lv_obj_set_pos(cell->root, x, y);
}

const lv_vlist_renderer_ops_t g_lv_renderer_generic_ops = {
    .create_cell = lv_renderer_create_cell,
    .destroy_cell = lv_renderer_destroy_cell,
    .get_root = lv_renderer_get_root,
    .bind_cell = lv_renderer_bind_cell,
    .set_pos = lv_renderer_set_pos
};
