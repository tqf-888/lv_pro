#include "lv_artist_style_3x2.h"
#include <string.h>
LV_FONT_DECLARE(lv_font_Regular_20);
lv_artist_view_style_t g_lv_artist_style_3x2;
void lv_artist_style_3x2_init(void)
{
    memset(&g_lv_artist_style_3x2, 0, sizeof(g_lv_artist_style_3x2));
    g_lv_artist_style_3x2.visible_cols = 3;
    g_lv_artist_style_3x2.visible_rows = 2;
    g_lv_artist_style_3x2.overscan_rows_front = 1;
    g_lv_artist_style_3x2.overscan_rows_back = 1;
    g_lv_artist_style_3x2.preload_before = 6;
    g_lv_artist_style_3x2.preload_after = 12;
    g_lv_artist_style_3x2.viewport_width = 1280;
    g_lv_artist_style_3x2.viewport_height = 800;
    g_lv_artist_style_3x2.gap_x = 20;
    g_lv_artist_style_3x2.gap_y = 20;
    g_lv_artist_style_3x2.cell_width = 413;
    g_lv_artist_style_3x2.cell_height = 390;
    g_lv_artist_style_3x2.row_style.cell_width = 413;
    g_lv_artist_style_3x2.row_style.cell_height = 390;
    g_lv_artist_style_3x2.row_style.avatar_x = 126;
    g_lv_artist_style_3x2.row_style.avatar_y = 58;
    g_lv_artist_style_3x2.row_style.avatar_w = 160;
    g_lv_artist_style_3x2.row_style.avatar_h = 160;
    g_lv_artist_style_3x2.row_style.name_x = 44;
    g_lv_artist_style_3x2.row_style.name_y = 250;
    g_lv_artist_style_3x2.row_style.name_w = 325;
    g_lv_artist_style_3x2.row_style.bg_color = lv_color_hex(0x101010);
    g_lv_artist_style_3x2.row_style.bg_opa = LV_OPA_COVER;
    g_lv_artist_style_3x2.row_style.border_color = lv_color_hex(0x404040);
    g_lv_artist_style_3x2.row_style.border_width = 1;
    g_lv_artist_style_3x2.row_style.radius = 20;
    g_lv_artist_style_3x2.row_style.avatar_bg_color = lv_color_hex(0x2F5AA8);
    g_lv_artist_style_3x2.row_style.avatar_bg_opa = LV_OPA_COVER;
    g_lv_artist_style_3x2.row_style.avatar_radius = 80;
    g_lv_artist_style_3x2.row_style.checked_bg_color = lv_color_hex(0x1F6FEB);
    g_lv_artist_style_3x2.row_style.checked_bg_opa = LV_OPA_COVER;
    g_lv_artist_style_3x2.row_style.name_font = &lv_font_Regular_20;
    g_lv_artist_style_3x2.row_style.name_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_3x2.row_style.name_opa = LV_OPA_COVER;
    g_lv_artist_style_3x2.row_style.placeholder_font = &lv_font_Regular_20;
    g_lv_artist_style_3x2.row_style.placeholder_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_3x2.row_style.placeholder_opa = LV_OPA_COVER;
}
