#include "lv_artist_view_style.h"
#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_artist_view_style_t g_lv_artist_default_style;

void lv_artist_default_style_init(void)
{
    memset(&g_lv_artist_default_style, 0, sizeof(g_lv_artist_default_style));

    g_lv_artist_default_style.visible_rows = 6;
    g_lv_artist_default_style.visible_cols = 1;
    g_lv_artist_default_style.overscan_rows_front = 2;
    g_lv_artist_default_style.overscan_rows_back = 2;
    g_lv_artist_default_style.preload_before = 8;
    g_lv_artist_default_style.preload_after = 24;

    g_lv_artist_default_style.viewport_width = 920;
    g_lv_artist_default_style.viewport_height = 640;
    g_lv_artist_default_style.cell_width = 920;
    g_lv_artist_default_style.cell_height = 100;
    g_lv_artist_default_style.gap_x = 0;
    g_lv_artist_default_style.gap_y = 0;

    g_lv_artist_default_style.row_style.cell_width = 920;
    g_lv_artist_default_style.row_style.cell_height = 100;

    g_lv_artist_default_style.row_style.avatar_x = 18;
    g_lv_artist_default_style.row_style.avatar_y = 14;
    g_lv_artist_default_style.row_style.avatar_w = 72;
    g_lv_artist_default_style.row_style.avatar_h = 72;

    g_lv_artist_default_style.row_style.name_x = 110;
    g_lv_artist_default_style.row_style.name_y = 34;
    g_lv_artist_default_style.row_style.name_w = 700;

    g_lv_artist_default_style.row_style.bg_color = lv_color_hex(0x0E0E0E);
    g_lv_artist_default_style.row_style.bg_opa = LV_OPA_COVER;
    g_lv_artist_default_style.row_style.border_color = lv_color_hex(0x2C2C2C);
    g_lv_artist_default_style.row_style.border_width = 1;
    g_lv_artist_default_style.row_style.radius = 18;

    g_lv_artist_default_style.row_style.avatar_bg_color = lv_color_hex(0x2F5AA8);
    g_lv_artist_default_style.row_style.avatar_bg_opa = LV_OPA_COVER;
    g_lv_artist_default_style.row_style.avatar_radius = 36;

    g_lv_artist_default_style.row_style.checked_bg_color = lv_color_hex(0x1F6FEB);
    g_lv_artist_default_style.row_style.checked_bg_opa = LV_OPA_COVER;

    g_lv_artist_default_style.row_style.name_font = &lv_font_Regular_20;
    g_lv_artist_default_style.row_style.name_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_default_style.row_style.name_opa = LV_OPA_COVER;

    g_lv_artist_default_style.row_style.placeholder_font = &lv_font_Regular_20;
    g_lv_artist_default_style.row_style.placeholder_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_default_style.row_style.placeholder_opa = LV_OPA_COVER;
}
