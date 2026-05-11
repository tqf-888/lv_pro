#include "lv_cover_style_2x4.h"
#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_cover_view_style_t g_lv_cover_style_2x4;

/* 封面业务：2 行 * 4 列。
 * 图片在上方，description 在图片下面。
 */
void lv_cover_style_2x4_init(void)
{
    memset(&g_lv_cover_style_2x4, 0, sizeof(g_lv_cover_style_2x4));

    g_lv_cover_style_2x4.visible_cols = 4;
    g_lv_cover_style_2x4.visible_rows = 2;
    g_lv_cover_style_2x4.overscan_rows_front = 1;
    g_lv_cover_style_2x4.overscan_rows_back = 1;
    g_lv_cover_style_2x4.preload_before = 4;
    g_lv_cover_style_2x4.preload_after = 8;

    g_lv_cover_style_2x4.viewport_width = 848;
    g_lv_cover_style_2x4.viewport_height = 556;
    g_lv_cover_style_2x4.gap_x = 16;
    g_lv_cover_style_2x4.gap_y = 16;

    g_lv_cover_style_2x4.cell_width = 200;
    g_lv_cover_style_2x4.cell_height = 270;

    g_lv_cover_style_2x4.row_style.cell_width = 200;
    g_lv_cover_style_2x4.row_style.cell_height = 270;

    g_lv_cover_style_2x4.row_style.avatar_x = 0;
    g_lv_cover_style_2x4.row_style.avatar_y = 0;
    g_lv_cover_style_2x4.row_style.avatar_w = 200;
    g_lv_cover_style_2x4.row_style.avatar_h = 200;

    g_lv_cover_style_2x4.row_style.name_x = 0;
    g_lv_cover_style_2x4.row_style.name_y = 206;
    g_lv_cover_style_2x4.row_style.name_w = 200;
    g_lv_cover_style_2x4.row_style.name_h = 60;

    g_lv_cover_style_2x4.row_style.bg_color = lv_color_hex(0x000000);
    g_lv_cover_style_2x4.row_style.bg_opa = LV_OPA_TRANSP;
    g_lv_cover_style_2x4.row_style.border_color = lv_color_hex(0x000000);
    g_lv_cover_style_2x4.row_style.border_width = 0;
    g_lv_cover_style_2x4.row_style.radius = 0;

    g_lv_cover_style_2x4.row_style.avatar_bg_color = lv_color_hex(0x000000);
    g_lv_cover_style_2x4.row_style.avatar_bg_opa = LV_OPA_TRANSP;
    g_lv_cover_style_2x4.row_style.avatar_radius = 8;

    g_lv_cover_style_2x4.row_style.checked_bg_color = lv_color_hex(0x000000);
    g_lv_cover_style_2x4.row_style.checked_bg_opa = LV_OPA_TRANSP;

    g_lv_cover_style_2x4.row_style.name_font = &lv_font_Regular_20;
    g_lv_cover_style_2x4.row_style.name_color = lv_color_hex(0x000000);
    g_lv_cover_style_2x4.row_style.name_opa = LV_OPA_COVER;
    g_lv_cover_style_2x4.row_style.name_bg_color = lv_color_hex(0x000000);
    g_lv_cover_style_2x4.row_style.name_bg_opa = LV_OPA_TRANSP;

    g_lv_cover_style_2x4.row_style.placeholder_font = &lv_font_Regular_20;
    g_lv_cover_style_2x4.row_style.placeholder_color = lv_color_hex(0x000000);
    g_lv_cover_style_2x4.row_style.placeholder_opa = LV_OPA_COVER;
}
