#ifndef LV_ARTIST_VIEW_STYLE_H
#define LV_ARTIST_VIEW_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

typedef struct {
    lv_coord_t cell_width;
    lv_coord_t cell_height;

    lv_coord_t avatar_x;
    lv_coord_t avatar_y;
    lv_coord_t avatar_w;
    lv_coord_t avatar_h;

    lv_coord_t name_x;
    lv_coord_t name_y;
    lv_coord_t name_w;

    lv_color_t bg_color;
    lv_opa_t bg_opa;
    lv_color_t border_color;
    uint8_t border_width;
    lv_coord_t radius;

    lv_color_t avatar_bg_color;
    lv_opa_t avatar_bg_opa;
    lv_coord_t avatar_radius;

    lv_color_t checked_bg_color;
    lv_opa_t checked_bg_opa;

    const lv_font_t *name_font;
    lv_color_t name_color;
    lv_opa_t name_opa;

    /* 当 name_h > 0 时，会在 cell 内部创建一个独立的名字条容器，
     * 支持半透明遮罩 + label 垂直居中（用于"图文交合"场景）。
     * 否则维持旧行为：label 直接挂 root，无背景。 */
    lv_coord_t name_h;
    lv_color_t name_bg_color;
    lv_opa_t name_bg_opa;

    const lv_font_t *placeholder_font;
    lv_color_t placeholder_color;
    lv_opa_t placeholder_opa;
} lv_artist_row_style_t;

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

    lv_artist_row_style_t row_style;
} lv_artist_view_style_t;

extern lv_artist_view_style_t g_lv_artist_default_style;
void lv_artist_default_style_init(void);

#ifdef __cplusplus
}
#endif

#endif
