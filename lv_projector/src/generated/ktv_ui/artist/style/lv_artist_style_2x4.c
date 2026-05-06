#include "lv_artist_style_2x4.h"
#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);

lv_artist_view_style_t g_lv_artist_style_2x4;

/* 布局：2 行 * 4 列 = 8 张
 * 每张图片 160 * 210，cell 完全等于图片大小
 * 名字条 32px 高、贴底、半透明黑、文字白色居中（图文交合） */
void lv_artist_style_2x4_init(void)
{
    memset(&g_lv_artist_style_2x4, 0, sizeof(g_lv_artist_style_2x4));

    g_lv_artist_style_2x4.visible_cols = 4;
    g_lv_artist_style_2x4.visible_rows = 2;

    g_lv_artist_style_2x4.overscan_rows_front = 1;
    g_lv_artist_style_2x4.overscan_rows_back = 1;
    g_lv_artist_style_2x4.preload_before = 4;
    g_lv_artist_style_2x4.preload_after = 8;

    /* viewport = 4*160 + 3*16 = 688 宽,  2*210 + 1*16 = 436 高 */
    g_lv_artist_style_2x4.viewport_width = 688;
    g_lv_artist_style_2x4.viewport_height = 436;

    g_lv_artist_style_2x4.gap_x = 16;
    g_lv_artist_style_2x4.gap_y = 16;

    /* cell == 图片本身 */
    g_lv_artist_style_2x4.cell_width = 160;
    g_lv_artist_style_2x4.cell_height = 210;

    g_lv_artist_style_2x4.row_style.cell_width = 160;
    g_lv_artist_style_2x4.row_style.cell_height = 210;

    /* 头像占满整格 */
    g_lv_artist_style_2x4.row_style.avatar_x = 0;
    g_lv_artist_style_2x4.row_style.avatar_y = 0;
    g_lv_artist_style_2x4.row_style.avatar_w = 160;
    g_lv_artist_style_2x4.row_style.avatar_h = 210;

    /* 名字条贴底，宽=160，高=32 */
    g_lv_artist_style_2x4.row_style.name_x = 0;
    g_lv_artist_style_2x4.row_style.name_y = 210 - 32;
    g_lv_artist_style_2x4.row_style.name_w = 160;
    g_lv_artist_style_2x4.row_style.name_h = 32;

    /* cell root 完全透明，让位给图片本身 */
    g_lv_artist_style_2x4.row_style.bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x4.row_style.bg_opa = LV_OPA_TRANSP;
    g_lv_artist_style_2x4.row_style.border_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x4.row_style.border_width = 0;
    g_lv_artist_style_2x4.row_style.radius = 0;

    /* 头像底色透明：图没下完时不显示占位块，只浮一条名字条 */
    g_lv_artist_style_2x4.row_style.avatar_bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x4.row_style.avatar_bg_opa = LV_OPA_TRANSP;
    g_lv_artist_style_2x4.row_style.avatar_radius = 0;

    g_lv_artist_style_2x4.row_style.checked_bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x4.row_style.checked_bg_opa = LV_OPA_TRANSP;

    /* 名字条：半透明黑 + 白字居中 */
    g_lv_artist_style_2x4.row_style.name_font = &lv_font_Regular_20;
    g_lv_artist_style_2x4.row_style.name_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_2x4.row_style.name_opa = LV_OPA_COVER;
    g_lv_artist_style_2x4.row_style.name_bg_color = lv_color_hex(0x000000);
    g_lv_artist_style_2x4.row_style.name_bg_opa = LV_OPA_60;

    g_lv_artist_style_2x4.row_style.placeholder_font = &lv_font_Regular_20;
    g_lv_artist_style_2x4.row_style.placeholder_color = lv_color_hex(0xFFFFFF);
    g_lv_artist_style_2x4.row_style.placeholder_opa = LV_OPA_COVER;
}
