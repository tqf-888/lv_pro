#ifndef LV_SONGSHEET_VIEW_STYLE_H
#define LV_SONGSHEET_VIEW_STYLE_H

#include "lvgl/lvgl.h"
#include "lv_renderer_generic.h"

typedef struct
{
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

    lv_renderer_layout_style_t renderer_style;
} lv_songsheet_view_style_t;

void lv_songsheet_styles_init(void);

#endif