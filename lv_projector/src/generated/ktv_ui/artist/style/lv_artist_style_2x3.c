#include "lv_artist_style_2x3.h"
#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_artist_view_style_t g_lv_artist_style_2x3;

void lv_artist_style_2x3_init(void)
{
    memset(&g_lv_artist_style_2x3, 0, sizeof(g_lv_artist_style_2x3));

    g_lv_artist_style_2x3.visible_cols = 3;
    g_lv_artist_style_2x3.visible_rows = 2;

    g_lv_artist_style_2x3.overscan_rows_front = 1;
    g_lv_artist_style_2x3.overscan_rows_back = 1;
    g_lv_artist_style_2x3.preload_before = 0;
    g_lv_artist_style_2x3.preload_after = 1;

    g_lv_artist_style_2x3.viewport_width = 640;
    /* 只显示两行：140 * 2 + 8 = 288 */
    g_lv_artist_style_2x3.viewport_height = 288;

    g_lv_artist_style_2x3.gap_x = 8;
    g_lv_artist_style_2x3.gap_y = 8;

    /* cont 只包住头像和歌手名，边距尽量小 */
    g_lv_artist_style_2x3.cell_width = 208;
    g_lv_artist_style_2x3.cell_height = 140;

    g_lv_artist_style_2x3.row_style.cell_width = 208;
    g_lv_artist_style_2x3.row_style.cell_height = 140;

    g_lv_artist_style_2x3.row_style.avatar_x = 54;
    g_lv_artist_style_2x3.row_style.avatar_y = 6;
    g_lv_artist_style_2x3.row_style.avatar_w = 100;
    g_lv_artist_style_2x3.row_style.avatar_h = 100;

    g_lv_artist_style_2x3.row_style.name_x = 6;
    g_lv_artist_style_2x3.row_style.name_y = 112;
    g_lv_artist_style_2x3.row_style.name_w = 196;

    g_lv_artist_style_2x3.row_style.bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x3.row_style.bg_opa = LV_OPA_TRANSP;
    g_lv_artist_style_2x3.row_style.border_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x3.row_style.border_width = 0;
    g_lv_artist_style_2x3.row_style.radius = 0;

    /* 头像后面不要蓝底 */
    g_lv_artist_style_2x3.row_style.avatar_bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x3.row_style.avatar_bg_opa = LV_OPA_TRANSP;
    g_lv_artist_style_2x3.row_style.avatar_radius = 0;

    /* 选中态也不要再出现蓝底 */
    g_lv_artist_style_2x3.row_style.checked_bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x3.row_style.checked_bg_opa = LV_OPA_TRANSP;

    g_lv_artist_style_2x3.row_style.name_font = &lv_font_Regular_20;
    g_lv_artist_style_2x3.row_style.name_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_2x3.row_style.name_opa = LV_OPA_COVER;

    g_lv_artist_style_2x3.row_style.placeholder_font = &lv_font_Regular_20;
    g_lv_artist_style_2x3.row_style.placeholder_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_2x3.row_style.placeholder_opa = LV_OPA_COVER;
}
