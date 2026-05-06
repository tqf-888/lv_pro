#include "lv_renderer_rich_song.h"
#include "lv_rich_song_adapter.h"
#include "lv_rich_song_catalog.h"
#include "lv_rich_song_view_style.h"
#include "../lv_rich_song_debug.h"
#include <stdlib.h>

typedef struct {
    lv_obj_t *root;
    lv_obj_t *lbl_idx;
    lv_obj_t *lbl_name;
    lv_obj_t *lbl_artist;
    lv_obj_t *lbl_a;
    lv_obj_t *lbl_b;
    lv_obj_t *lbl_c;
    lv_obj_t *lbl_d;
    lv_obj_t *lbl_e;
    lv_obj_t *lbl_f;
} lv_renderer_rich_song_cell_t;

static const lv_rich_song_row_style_t *rs_style_of(lv_vlist_t *vlist)
{
    const lv_vlist_config_t *cfg = lv_vlist_get_config(vlist);
    return (const lv_rich_song_row_style_t *)cfg->renderer_style;
}

static lv_obj_t *rs_make_label(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w)
{
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_set_pos(lbl, x, y);
    if (w > 0) lv_obj_set_width(lbl, w);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_DOT);
    lv_obj_add_flag(lbl, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(lbl, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return lbl;
}

static lv_obj_t *rs_make_icon_label(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w)
{
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_set_pos(lbl, x, y);
    if (w > 0) lv_obj_set_width(lbl, w);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_flag(lbl, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(lbl, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return lbl;
}

static void rs_set_label(lv_obj_t *obj,const char *text,uint32_t color_hex,lv_opa_t opa,const lv_font_t *font,const lv_rich_song_row_style_t *s)
{
    if (obj == NULL || s == NULL) return;
    lv_label_set_text(obj, (text != NULL) ? text : "");
    lv_obj_set_style_text_color(obj, color_hex ? lv_color_hex(color_hex) : s->fallback_text_color, 0);
    lv_obj_set_style_text_opa(obj, opa ? opa : s->fallback_text_opa, 0);
    lv_obj_set_style_text_font(obj, (font != NULL) ? font : s->fallback_font, 0);
}

static void *rs_create_cell(lv_vlist_t *vlist, lv_obj_t *parent)
{
    lv_renderer_rich_song_cell_t *cell;
    const lv_rich_song_row_style_t *s = rs_style_of(vlist);
    if (s == NULL) return NULL;
    cell = (lv_renderer_rich_song_cell_t *)RS_CALLOC(1, sizeof(*cell), "renderer.cell");
    if (cell == NULL) return NULL;

    cell->root = lv_obj_create(parent);
    lv_obj_set_size(cell->root, s->cell_width, s->cell_height);
    lv_obj_clear_flag(cell->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cell->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(cell->root, 0, 0);
    lv_obj_set_style_radius(cell->root, s->radius, 0);
    lv_obj_set_style_bg_color(cell->root, s->bg_color, 0);
    lv_obj_set_style_bg_opa(cell->root, s->bg_opa, 0);
    lv_obj_set_style_bg_color(cell->root, s->bg_color, LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cell->root, s->bg_opa, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(cell->root, s->bg_color, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(cell->root, s->bg_opa, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(cell->root, s->bg_color, LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(cell->root, s->bg_opa, LV_STATE_CHECKED);
    lv_obj_set_style_border_color(cell->root, s->border_color, 0);
    lv_obj_set_style_border_width(cell->root, s->border_width, 0);
    lv_obj_add_flag(cell->root, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(cell->root, LV_OBJ_FLAG_GESTURE_BUBBLE);

    cell->lbl_idx    = rs_make_label(cell->root, s->idx_x, s->text_y, s->idx_w);
    cell->lbl_name   = rs_make_label(cell->root, s->name_x, s->text_y, s->name_w);
    cell->lbl_artist = rs_make_label(cell->root, s->artist_x, s->text_y, s->artist_w);
    cell->lbl_a      = rs_make_icon_label(cell->root, s->a_x, s->text_y, s->a_w);
    cell->lbl_b      = rs_make_icon_label(cell->root, s->b_x, s->text_y, s->b_w);
    cell->lbl_c      = rs_make_icon_label(cell->root, s->c_x, s->text_y, s->c_w);
    cell->lbl_d      = rs_make_icon_label(cell->root, s->d_x, s->text_y, s->d_w);
    cell->lbl_e      = rs_make_icon_label(cell->root, s->e_x, s->text_y, s->e_w);
    cell->lbl_f      = rs_make_icon_label(cell->root, s->f_x, s->text_y, s->f_w);
    return cell;
}

static void rs_destroy_cell(lv_vlist_t *vlist, void *renderer_ctx) { (void)vlist; RS_FREE(renderer_ctx, "renderer.cell"); }
static lv_obj_t *rs_get_root(lv_vlist_t *vlist, void *renderer_ctx) { (void)vlist; return renderer_ctx ? ((lv_renderer_rich_song_cell_t *)renderer_ctx)->root : NULL; }

static void rs_bind_cell(lv_vlist_t *vlist, void *renderer_ctx, const lv_vlist_item_t *item)
{
    lv_renderer_rich_song_cell_t *cell = (lv_renderer_rich_song_cell_t *)renderer_ctx;
    const lv_vlist_config_t *cfg = lv_vlist_get_config(vlist);
    const lv_rich_song_row_style_t *s = rs_style_of(vlist);
    lv_rich_song_adapter_t *adapter;
    lv_rich_song_item_t biz;

    if (cell == NULL || item == NULL || cfg == NULL || s == NULL) return;
    adapter = (lv_rich_song_adapter_t *)cfg->user_ctx;
    if (adapter == NULL) return;
    if (!lv_rich_song_adapter_get_business_item(adapter, item->item_id, &biz)) return;

    lv_obj_clear_state(cell->root, LV_STATE_PRESSED | LV_STATE_FOCUSED | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(cell->root, s->bg_color, 0);
    lv_obj_set_style_bg_opa(cell->root, s->bg_opa, 0);

    if (!biz.ready) {
        rs_set_label(cell->lbl_idx, "", 0, 255, NULL, s);
        rs_set_label(cell->lbl_name, "Loading...", 0, 255, NULL, s);
        rs_set_label(cell->lbl_artist, "", 0, 255, NULL, s);
        rs_set_label(cell->lbl_a, "", 0, 255, NULL, s);
        rs_set_label(cell->lbl_b, "", 0, 255, NULL, s);
        rs_set_label(cell->lbl_c, "", 0, 255, NULL, s);
        rs_set_label(cell->lbl_d, "", 0, 255, NULL, s);
        rs_set_label(cell->lbl_e, "", 0, 255, NULL, s);
        rs_set_label(cell->lbl_f, "", 0, 255, NULL, s);
        return;
    }

    rs_set_label(cell->lbl_idx,    biz.index_text, biz.color_idx,    biz.opa_idx,    biz.font_idx,    s);
    rs_set_label(cell->lbl_name,   biz.name,       biz.color_name,   biz.opa_name,   biz.font_name,   s);
    rs_set_label(cell->lbl_artist, biz.artist,     biz.color_artist, biz.opa_artist, biz.font_artist, s);
    rs_set_label(cell->lbl_a,      biz.text_a,     biz.color_a,      biz.opa_a,      biz.font_a,      s);
    rs_set_label(cell->lbl_b,      biz.text_b,     biz.color_b,      biz.opa_b,      biz.font_b,      s);
    rs_set_label(cell->lbl_c,      biz.text_c,     biz.color_c,      biz.opa_c,      biz.font_c,      s);
    rs_set_label(cell->lbl_d,      biz.text_d,     biz.color_d,      biz.opa_d,      biz.font_d,      s);
    rs_set_label(cell->lbl_e,      biz.text_e,     biz.color_e,      biz.opa_e,      biz.font_e,      s);
    rs_set_label(cell->lbl_f,      biz.text_f,     biz.color_f,      biz.opa_f,      biz.font_f,      s);
}

static void rs_set_pos(lv_vlist_t *vlist, void *renderer_ctx, lv_coord_t x, lv_coord_t y)
{
    lv_renderer_rich_song_cell_t *cell = (lv_renderer_rich_song_cell_t *)renderer_ctx;
    (void)vlist;
    if (cell && cell->root) lv_obj_set_pos(cell->root, x, y);
}

const lv_vlist_renderer_ops_t g_lv_renderer_rich_song_ops = {
    .create_cell = rs_create_cell,
    .destroy_cell = rs_destroy_cell,
    .get_root = rs_get_root,
    .bind_cell = rs_bind_cell,
    .set_pos = rs_set_pos
};
