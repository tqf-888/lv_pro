#include "lv_songsheet_style_3x2.h"

#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_songsheet_view_style_t g_lv_songsheet_style_3x2;

void lv_songsheet_style_3x2_init(void)
{
    memset(&g_lv_songsheet_style_3x2, 0, sizeof(g_lv_songsheet_style_3x2));

    g_lv_songsheet_style_3x2.visible_rows = 3;
    g_lv_songsheet_style_3x2.visible_cols = 2;
    g_lv_songsheet_style_3x2.overscan_rows_front = 1;
    g_lv_songsheet_style_3x2.overscan_rows_back = 1;
    g_lv_songsheet_style_3x2.preload_before = 3;
    g_lv_songsheet_style_3x2.preload_after = 8;

    g_lv_songsheet_style_3x2.viewport_width = 1180;
    g_lv_songsheet_style_3x2.viewport_height = 800;

    g_lv_songsheet_style_3x2.gap_x = 20;
    g_lv_songsheet_style_3x2.gap_y = 19;

    /* 布局层尺寸 */
    g_lv_songsheet_style_3x2.cell_width = 580;   /* 580 * 2 + 20 = 1180 */
    g_lv_songsheet_style_3x2.cell_height = 254;  /* 254 * 3 + 19 * 2 = 800 */

    /* 渲染层尺寸 */
    g_lv_songsheet_style_3x2.renderer_style.cell_width = 580;
    g_lv_songsheet_style_3x2.renderer_style.cell_height = 254;
    g_lv_songsheet_style_3x2.renderer_style.cell_radius = 18;
    g_lv_songsheet_style_3x2.renderer_style.bg_color = lv_color_hex(0x101010);
    g_lv_songsheet_style_3x2.renderer_style.bg_opa = LV_OPA_COVER;
    g_lv_songsheet_style_3x2.renderer_style.border_color = lv_color_hex(0x404040);
    g_lv_songsheet_style_3x2.renderer_style.border_width = 1;

    g_lv_songsheet_style_3x2.renderer_style.show_image = true;
    g_lv_songsheet_style_3x2.renderer_style.image_x = 14;
    g_lv_songsheet_style_3x2.renderer_style.image_y = 14;
    g_lv_songsheet_style_3x2.renderer_style.image_w = 552;
    g_lv_songsheet_style_3x2.renderer_style.image_h = 162;

    g_lv_songsheet_style_3x2.renderer_style.show_title = true;
    g_lv_songsheet_style_3x2.renderer_style.title_x = 16;
    g_lv_songsheet_style_3x2.renderer_style.title_y = 186;
    g_lv_songsheet_style_3x2.renderer_style.title_w = 548;
    g_lv_songsheet_style_3x2.renderer_style.title_font = &lv_font_Regular_20;
    g_lv_songsheet_style_3x2.renderer_style.title_color = lv_color_hex(0xFFD86B);

    g_lv_songsheet_style_3x2.renderer_style.show_subtitle = true;
    g_lv_songsheet_style_3x2.renderer_style.subtitle_x = 16;
    g_lv_songsheet_style_3x2.renderer_style.subtitle_y = 216;
    g_lv_songsheet_style_3x2.renderer_style.subtitle_w = 548;
    g_lv_songsheet_style_3x2.renderer_style.subtitle_font = &lv_font_Regular_20;
    g_lv_songsheet_style_3x2.renderer_style.subtitle_color = lv_color_hex(0xE0E0E0);
}
