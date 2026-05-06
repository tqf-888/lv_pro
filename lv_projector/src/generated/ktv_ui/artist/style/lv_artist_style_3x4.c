#include "lv_artist_style_3x4.h"
#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_artist_view_style_t g_lv_artist_style_3x4;

void lv_artist_style_3x4_init(void)
{
    memset(&g_lv_artist_style_3x4, 0, sizeof(g_lv_artist_style_3x4));

    g_lv_artist_style_3x4.visible_cols = 3;
    g_lv_artist_style_3x4.visible_rows = 4;
    g_lv_artist_style_3x4.overscan_rows_front = 1;
    g_lv_artist_style_3x4.overscan_rows_back = 1;
    g_lv_artist_style_3x4.preload_before = 6;
    g_lv_artist_style_3x4.preload_after = 12;

    /* 3 列 4 行整体从 (0,0) 紧凑排布，viewport 只包住内容本身 */
    g_lv_artist_style_3x4.viewport_width = 516;
    g_lv_artist_style_3x4.viewport_height = 618;
    g_lv_artist_style_3x4.gap_x = 6;
    g_lv_artist_style_3x4.gap_y = 6;

    /* cont 只比头像+文字区域多一点点边距 */
    g_lv_artist_style_3x4.cell_width = 168;
    g_lv_artist_style_3x4.cell_height = 150;

    g_lv_artist_style_3x4.row_style.cell_width = 168;
    g_lv_artist_style_3x4.row_style.cell_height = 150;

    /* 内容贴左上，避免看起来整体偏中间 */
    g_lv_artist_style_3x4.row_style.avatar_x = 4;
    g_lv_artist_style_3x4.row_style.avatar_y = 4;
    g_lv_artist_style_3x4.row_style.avatar_w = 160;
    g_lv_artist_style_3x4.row_style.avatar_h = 120;

    g_lv_artist_style_3x4.row_style.name_x = 4;
    g_lv_artist_style_3x4.row_style.name_y = 128;
    g_lv_artist_style_3x4.row_style.name_w = 160;

    /* 去掉所有背景色/边框/选中底色 */
    g_lv_artist_style_3x4.row_style.bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_3x4.row_style.bg_opa = LV_OPA_TRANSP;
    g_lv_artist_style_3x4.row_style.border_color = lv_color_hex(0x000000);
    g_lv_artist_style_3x4.row_style.border_width = 0;
    g_lv_artist_style_3x4.row_style.radius = 0;

    g_lv_artist_style_3x4.row_style.avatar_bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_3x4.row_style.avatar_bg_opa = LV_OPA_TRANSP;
    g_lv_artist_style_3x4.row_style.avatar_radius = 0;

    g_lv_artist_style_3x4.row_style.checked_bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_3x4.row_style.checked_bg_opa = LV_OPA_TRANSP;

    g_lv_artist_style_3x4.row_style.name_font = &lv_font_Regular_20;
    g_lv_artist_style_3x4.row_style.name_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_3x4.row_style.name_opa = LV_OPA_COVER;

    g_lv_artist_style_3x4.row_style.placeholder_font = &lv_font_Regular_20;
    g_lv_artist_style_3x4.row_style.placeholder_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_3x4.row_style.placeholder_opa = LV_OPA_COVER;
}
