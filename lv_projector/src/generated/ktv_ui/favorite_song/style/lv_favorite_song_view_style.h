#ifndef LV_FAVORITE_SONG_VIEW_STYLE_H
#define LV_FAVORITE_SONG_VIEW_STYLE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl/lvgl.h"
typedef struct {
    lv_coord_t cell_width;
    lv_coord_t cell_height;
    lv_coord_t idx_x; lv_coord_t idx_w;
    lv_coord_t name_x; lv_coord_t name_w;
    lv_coord_t artist_x; lv_coord_t artist_w;
    lv_coord_t a_x; lv_coord_t a_w;
    lv_coord_t b_x; lv_coord_t b_w;
    lv_coord_t c_x; lv_coord_t c_w;
    lv_coord_t d_x; lv_coord_t d_w;
    lv_coord_t e_x; lv_coord_t e_w;
    lv_coord_t f_x; lv_coord_t f_w;
    lv_coord_t text_y;
    lv_color_t bg_color;
    lv_opa_t bg_opa;
    lv_color_t border_color;
    uint8_t border_width;
    lv_coord_t radius;
    lv_color_t checked_bg_color;
    lv_opa_t checked_bg_opa;
    const lv_font_t *fallback_font;
    lv_color_t fallback_text_color;
    lv_opa_t fallback_text_opa;
} lv_favorite_song_row_style_t;
typedef struct {
    uint32_t visible_rows;
    uint32_t visible_cols;
    uint32_t overscan_rows_front;
    uint32_t overscan_rows_back;
    uint32_t preload_before;
    uint32_t preload_after;
    lv_coord_t viewport_width;
    lv_coord_t viewport_height;
    lv_coord_t cell_width;
    lv_coord_t cell_height;
    lv_coord_t gap_x;
    lv_coord_t gap_y;
    lv_favorite_song_row_style_t row_style;
} lv_favorite_song_view_style_t;
extern lv_favorite_song_view_style_t g_lv_favorite_song_default_style;
void lv_favorite_song_default_style_init(void);
#ifdef __cplusplus
}
#endif
#endif
