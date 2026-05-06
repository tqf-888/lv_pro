#include "lv_layout_styles.h"

#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_renderer_layout_style_t g_lv_layout_style_1;
lv_renderer_layout_style_t g_lv_layout_style_2;
lv_renderer_layout_style_t g_lv_layout_style_3;

void lv_layout_styles_init(void)
{
    memset(&g_lv_layout_style_1, 0, sizeof(g_lv_layout_style_1));
    g_lv_layout_style_1.cell_width = 1180;
    g_lv_layout_style_1.cell_height = 152;
    g_lv_layout_style_1.cell_radius = 0;
    g_lv_layout_style_1.bg_color = lv_color_hex(0x000000);
    g_lv_layout_style_1.bg_opa = LV_OPA_TRANSP;
    g_lv_layout_style_1.border_color = lv_color_hex(0x000000);
    g_lv_layout_style_1.border_width = 0;
    g_lv_layout_style_1.show_image = true;
    g_lv_layout_style_1.image_x = 8;
    g_lv_layout_style_1.image_y = 11;
    g_lv_layout_style_1.image_w = 800;
    g_lv_layout_style_1.image_h = 130;
    g_lv_layout_style_1.show_title = true;
    g_lv_layout_style_1.title_x = 825;
    g_lv_layout_style_1.title_y = 18;
    g_lv_layout_style_1.title_w = 330;
    g_lv_layout_style_1.title_font = &lv_font_Regular_20;
    g_lv_layout_style_1.title_color = lv_color_hex(0xFFFFFF);
    g_lv_layout_style_1.show_subtitle = true;
    g_lv_layout_style_1.subtitle_x = 825;
    g_lv_layout_style_1.subtitle_y = 62;
    g_lv_layout_style_1.subtitle_w = 330;
    g_lv_layout_style_1.subtitle_font = &lv_font_Regular_20;
    g_lv_layout_style_1.subtitle_color = lv_color_hex(0xB0B0B0);

    memset(&g_lv_layout_style_2, 0, sizeof(g_lv_layout_style_2));
    g_lv_layout_style_2.cell_width = 1180;
    g_lv_layout_style_2.cell_height = 152;
    g_lv_layout_style_2.cell_radius = 12;
    g_lv_layout_style_2.bg_color = lv_color_hex(0x1B1B1B);
    g_lv_layout_style_2.bg_opa = LV_OPA_COVER;
    g_lv_layout_style_2.border_color = lv_color_hex(0x2E2E2E);
    g_lv_layout_style_2.border_width = 1;
    g_lv_layout_style_2.show_image = true;
    g_lv_layout_style_2.image_x = 20;
    g_lv_layout_style_2.image_y = 11;
    g_lv_layout_style_2.image_w = 420;
    g_lv_layout_style_2.image_h = 130;
    g_lv_layout_style_2.show_title = true;
    g_lv_layout_style_2.title_x = 470;
    g_lv_layout_style_2.title_y = 24;
    g_lv_layout_style_2.title_w = 660;
    g_lv_layout_style_2.title_font = &lv_font_Regular_20;
    g_lv_layout_style_2.title_color = lv_color_hex(0xFFFFFF);
    g_lv_layout_style_2.show_subtitle = true;
    g_lv_layout_style_2.subtitle_x = 470;
    g_lv_layout_style_2.subtitle_y = 78;
    g_lv_layout_style_2.subtitle_w = 660;
    g_lv_layout_style_2.subtitle_font = &lv_font_Regular_20;
    g_lv_layout_style_2.subtitle_color = lv_color_hex(0x8AC6FF);

    memset(&g_lv_layout_style_3, 0, sizeof(g_lv_layout_style_3));
    g_lv_layout_style_3.cell_width = 1180;
    g_lv_layout_style_3.cell_height = 152;
    g_lv_layout_style_3.cell_radius = 18;
    g_lv_layout_style_3.bg_color = lv_color_hex(0x101010);
    g_lv_layout_style_3.bg_opa = LV_OPA_COVER;
    g_lv_layout_style_3.border_color = lv_color_hex(0x404040);
    g_lv_layout_style_3.border_width = 1;
    g_lv_layout_style_3.show_image = true;
    g_lv_layout_style_3.image_x = 18;
    g_lv_layout_style_3.image_y = 18;
    g_lv_layout_style_3.image_w = 180;
    g_lv_layout_style_3.image_h = 116;
    g_lv_layout_style_3.show_title = true;
    g_lv_layout_style_3.title_x = 230;
    g_lv_layout_style_3.title_y = 20;
    g_lv_layout_style_3.title_w = 900;
    g_lv_layout_style_3.title_font = &lv_font_Regular_20;
    g_lv_layout_style_3.title_color = lv_color_hex(0xFFD86B);
    g_lv_layout_style_3.show_subtitle = true;
    g_lv_layout_style_3.subtitle_x = 230;
    g_lv_layout_style_3.subtitle_y = 72;
    g_lv_layout_style_3.subtitle_w = 900;
    g_lv_layout_style_3.subtitle_font = &lv_font_Regular_20;
    g_lv_layout_style_3.subtitle_color = lv_color_hex(0xE0E0E0);
}
