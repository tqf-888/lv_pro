#ifndef LV_RENDERER_GENERIC_H
#define LV_RENDERER_GENERIC_H

#include "lv_virtual_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_coord_t cell_width;
    lv_coord_t cell_height;
    lv_coord_t cell_radius;
    lv_color_t bg_color;
    lv_opa_t bg_opa;
    lv_color_t border_color;
    uint8_t border_width;
    bool show_image;
    lv_coord_t image_x;
    lv_coord_t image_y;
    lv_coord_t image_w;
    lv_coord_t image_h;
    bool show_title;
    lv_coord_t title_x;
    lv_coord_t title_y;
    lv_coord_t title_w;
    const lv_font_t *title_font;
    lv_color_t title_color;
    bool show_subtitle;
    lv_coord_t subtitle_x;
    lv_coord_t subtitle_y;
    lv_coord_t subtitle_w;
    const lv_font_t *subtitle_font;
    lv_color_t subtitle_color;
} lv_renderer_layout_style_t;

extern const lv_vlist_renderer_ops_t g_lv_renderer_generic_ops;

#ifdef __cplusplus
}
#endif

#endif
