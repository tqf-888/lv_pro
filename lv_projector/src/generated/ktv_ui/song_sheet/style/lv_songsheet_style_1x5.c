#include "lv_songsheet_style_1x5.h"

#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_songsheet_view_style_t g_lv_songsheet_style_1x5;

void lv_songsheet_style_1x5_init(void)
{
    memset(&g_lv_songsheet_style_1x5, 0, sizeof(g_lv_songsheet_style_1x5));

    g_lv_songsheet_style_1x5.visible_rows = 5;
    g_lv_songsheet_style_1x5.visible_cols = 1;
    g_lv_songsheet_style_1x5.overscan_rows_front = 1;
    g_lv_songsheet_style_1x5.overscan_rows_back = 1;
    g_lv_songsheet_style_1x5.preload_before = 2;
    g_lv_songsheet_style_1x5.preload_after = 8;
    g_lv_songsheet_style_1x5.viewport_width = 1180;
    g_lv_songsheet_style_1x5.viewport_height = 800;
    g_lv_songsheet_style_1x5.gap_x = 0;
    g_lv_songsheet_style_1x5.gap_y = 10;

    /* 关键修复：补齐顶层布局参数，供 vlist 使用 */
    g_lv_songsheet_style_1x5.cell_width = 1180;
    g_lv_songsheet_style_1x5.cell_height = 152;

    g_lv_songsheet_style_1x5.renderer_style.cell_width = 1180;
    g_lv_songsheet_style_1x5.renderer_style.cell_height = 152;
    g_lv_songsheet_style_1x5.renderer_style.cell_radius = 18;
    g_lv_songsheet_style_1x5.renderer_style.bg_color = lv_color_hex(0x101010);
    g_lv_songsheet_style_1x5.renderer_style.bg_opa = LV_OPA_COVER;
    g_lv_songsheet_style_1x5.renderer_style.border_color = lv_color_hex(0x404040);
    g_lv_songsheet_style_1x5.renderer_style.border_width = 1;
    g_lv_songsheet_style_1x5.renderer_style.show_image = true;
    g_lv_songsheet_style_1x5.renderer_style.image_x = 18;
    g_lv_songsheet_style_1x5.renderer_style.image_y = 18;
    g_lv_songsheet_style_1x5.renderer_style.image_w = 180;
    g_lv_songsheet_style_1x5.renderer_style.image_h = 116;
    g_lv_songsheet_style_1x5.renderer_style.show_title = true;
    g_lv_songsheet_style_1x5.renderer_style.title_x = 230;
    g_lv_songsheet_style_1x5.renderer_style.title_y = 20;
    g_lv_songsheet_style_1x5.renderer_style.title_w = 900;
    g_lv_songsheet_style_1x5.renderer_style.title_font = &lv_font_Regular_20;
    g_lv_songsheet_style_1x5.renderer_style.title_color = lv_color_hex(0xFFD86B);
    g_lv_songsheet_style_1x5.renderer_style.show_subtitle = true;
    g_lv_songsheet_style_1x5.renderer_style.subtitle_x = 230;
    g_lv_songsheet_style_1x5.renderer_style.subtitle_y = 72;
    g_lv_songsheet_style_1x5.renderer_style.subtitle_w = 900;
    g_lv_songsheet_style_1x5.renderer_style.subtitle_font = &lv_font_Regular_20;
    g_lv_songsheet_style_1x5.renderer_style.subtitle_color = lv_color_hex(0xE0E0E0);
}
