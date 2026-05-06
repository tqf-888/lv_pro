#include "lv_songsheet_style_2x3.h"

#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_songsheet_view_style_t g_lv_songsheet_style_2x3;

void lv_songsheet_style_2x3_init(void)
{
    memset(&g_lv_songsheet_style_2x3, 0, sizeof(g_lv_songsheet_style_2x3));

    g_lv_songsheet_style_2x3.visible_rows = 2;
    g_lv_songsheet_style_2x3.visible_cols = 3;
    g_lv_songsheet_style_2x3.overscan_rows_front = 1;
    g_lv_songsheet_style_2x3.overscan_rows_back = 1;
    g_lv_songsheet_style_2x3.preload_before = 2;
    g_lv_songsheet_style_2x3.preload_after = 6;
    g_lv_songsheet_style_2x3.viewport_width = 1180;
    g_lv_songsheet_style_2x3.viewport_height = 800;
    g_lv_songsheet_style_2x3.gap_x = 20;
    g_lv_songsheet_style_2x3.gap_y = 20;

    /* 1180 = 380 * 3 + 20 * 2
       800  = 390 * 2 + 20 * 1 */
    g_lv_songsheet_style_2x3.cell_width = 380;
    g_lv_songsheet_style_2x3.cell_height = 390;

    g_lv_songsheet_style_2x3.renderer_style.cell_width = 380;
    g_lv_songsheet_style_2x3.renderer_style.cell_height = 390;
    g_lv_songsheet_style_2x3.renderer_style.cell_radius = 18;
    g_lv_songsheet_style_2x3.renderer_style.bg_color = lv_color_hex(0x101010);
    g_lv_songsheet_style_2x3.renderer_style.bg_opa = LV_OPA_COVER;
    g_lv_songsheet_style_2x3.renderer_style.border_color = lv_color_hex(0x404040);
    g_lv_songsheet_style_2x3.renderer_style.border_width = 1;

    g_lv_songsheet_style_2x3.renderer_style.show_image = true;
    g_lv_songsheet_style_2x3.renderer_style.image_x = 14;
    g_lv_songsheet_style_2x3.renderer_style.image_y = 14;
    g_lv_songsheet_style_2x3.renderer_style.image_w = 352;
    g_lv_songsheet_style_2x3.renderer_style.image_h = 250;

    g_lv_songsheet_style_2x3.renderer_style.show_title = true;
    g_lv_songsheet_style_2x3.renderer_style.title_x = 16;
    g_lv_songsheet_style_2x3.renderer_style.title_y = 280;
    g_lv_songsheet_style_2x3.renderer_style.title_w = 348;
    g_lv_songsheet_style_2x3.renderer_style.title_font = &lv_font_Regular_20;
    g_lv_songsheet_style_2x3.renderer_style.title_color = lv_color_hex(0xFFD86B);

    g_lv_songsheet_style_2x3.renderer_style.show_subtitle = true;
    g_lv_songsheet_style_2x3.renderer_style.subtitle_x = 16;
    g_lv_songsheet_style_2x3.renderer_style.subtitle_y = 322;
    g_lv_songsheet_style_2x3.renderer_style.subtitle_w = 348;
    g_lv_songsheet_style_2x3.renderer_style.subtitle_font = &lv_font_Regular_20;
    g_lv_songsheet_style_2x3.renderer_style.subtitle_color = lv_color_hex(0xE0E0E0);
}
