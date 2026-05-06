#include "lv_top100_song_view_style.h"
#include <string.h>

LV_FONT_DECLARE(lv_font_Regular_20);
LV_FONT_DECLARE(lv_font_ktv_30);

lv_top100_song_view_style_t g_lv_top100_song_default_style;

void lv_top100_song_default_style_init(void)
{
    memset(&g_lv_top100_song_default_style, 0, sizeof(g_lv_top100_song_default_style));

    g_lv_top100_song_default_style.visible_rows = 8;
    g_lv_top100_song_default_style.visible_cols = 1;

    /*
     * 前向 overscan 仍然保留，用于向上回滚时减少临界闪烁。
     * 顶部首屏是否允许前向扩展，必须由 lv_virtual_list.c 内部做 clamp，
     * 不能靠这里直接把功能关掉来规避问题。
     */
    g_lv_top100_song_default_style.overscan_rows_front = 2;
    g_lv_top100_song_default_style.overscan_rows_back = 2;
    g_lv_top100_song_default_style.preload_before = 8;
    g_lv_top100_song_default_style.preload_after = 24;

    /*
     * 目标尺寸：840 x 670
     * 8 行铺满高度：
     * 8 * 82 + 7 * 2 = 670
     */
    g_lv_top100_song_default_style.viewport_width = 840;
    g_lv_top100_song_default_style.viewport_height = 670;

    g_lv_top100_song_default_style.cell_width = 840;
    g_lv_top100_song_default_style.cell_height = 82;

    g_lv_top100_song_default_style.gap_x = 0;
    g_lv_top100_song_default_style.gap_y = 2;

    g_lv_top100_song_default_style.row_style.cell_width = 840;
    g_lv_top100_song_default_style.row_style.cell_height = 82;

    /* 左侧序号 */
    g_lv_top100_song_default_style.row_style.idx_x = 12;
    g_lv_top100_song_default_style.row_style.idx_w = 44;

    /* 歌名 */
    g_lv_top100_song_default_style.row_style.name_x = 64;
    g_lv_top100_song_default_style.row_style.name_w = 250;

    /* 歌手名 */
    g_lv_top100_song_default_style.row_style.artist_x = 320;
    g_lv_top100_song_default_style.row_style.artist_w = 150;

    /*
     * 右侧标记区整体左收，确保 840 宽内不裁切。
     * 最右一列结束位置：790 + 40 = 830
     */
    g_lv_top100_song_default_style.row_style.a_x = 470;
    g_lv_top100_song_default_style.row_style.a_w = 90;

    g_lv_top100_song_default_style.row_style.b_x = 560;
    g_lv_top100_song_default_style.row_style.b_w = 90;

    g_lv_top100_song_default_style.row_style.c_x = 660;
    g_lv_top100_song_default_style.row_style.c_w = 40;

    g_lv_top100_song_default_style.row_style.d_x = 710;
    g_lv_top100_song_default_style.row_style.d_w = 40;

    g_lv_top100_song_default_style.row_style.e_x = 760;
    g_lv_top100_song_default_style.row_style.e_w = 40;

    g_lv_top100_song_default_style.row_style.f_x = 790;
    g_lv_top100_song_default_style.row_style.f_w = 40;

    /* 文字整体下移一点，适配 82 高度 */
    g_lv_top100_song_default_style.row_style.text_y = 24;

    g_lv_top100_song_default_style.row_style.bg_color = lv_color_hex(0x000000);
    g_lv_top100_song_default_style.row_style.bg_opa = LV_OPA_TRANSP;
    g_lv_top100_song_default_style.row_style.border_color = lv_color_hex(0x000000);
    g_lv_top100_song_default_style.row_style.border_width = 0;
    g_lv_top100_song_default_style.row_style.radius = 0;

    g_lv_top100_song_default_style.row_style.checked_bg_color = lv_color_hex(0x0055AA);
    g_lv_top100_song_default_style.row_style.checked_bg_opa = LV_OPA_COVER;

    g_lv_top100_song_default_style.row_style.fallback_font = &lv_font_Regular_20;
    g_lv_top100_song_default_style.row_style.fallback_text_color = lv_color_hex(0xFFFFFF);
    g_lv_top100_song_default_style.row_style.fallback_text_opa = LV_OPA_COVER;

    (void)lv_font_ktv_30;
}