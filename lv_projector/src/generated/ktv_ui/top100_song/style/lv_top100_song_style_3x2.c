#include "lv_top100_song_style_3x2.h"
#include <string.h>
LV_FONT_DECLARE(lv_font_Regular_20);
LV_FONT_DECLARE(lv_font_ktv_30);
lv_top100_song_view_style_t g_lv_top100_song_style_3x2;
void lv_top100_song_style_3x2_init(void)
{
    memset(&g_lv_top100_song_style_3x2, 0, sizeof(g_lv_top100_song_style_3x2));
    g_lv_top100_song_style_3x2.visible_cols = 3;
    g_lv_top100_song_style_3x2.visible_rows = 2;
    /*
     * 前向 overscan 仍然保留，用于向上回滚时减少临界闪烁。
     * 但它只能对“真实存在的前置行”生效。
     * 顶部首屏的 clamp 必须由 lv_virtual_list.c 内部处理，不能靠这里把功能关掉来规避。
     */
    g_lv_top100_song_style_3x2.overscan_rows_front = 1;
    g_lv_top100_song_style_3x2.overscan_rows_back = 1;
    g_lv_top100_song_style_3x2.preload_before = 6;
    g_lv_top100_song_style_3x2.preload_after = 12;
    g_lv_top100_song_style_3x2.viewport_width = 1280;
    g_lv_top100_song_style_3x2.viewport_height = 800;
    g_lv_top100_song_style_3x2.gap_x = 20;
    g_lv_top100_song_style_3x2.gap_y = 20;
    g_lv_top100_song_style_3x2.cell_width = 413;
    g_lv_top100_song_style_3x2.cell_height = 390;
    g_lv_top100_song_style_3x2.row_style.cell_width = 413;
    g_lv_top100_song_style_3x2.row_style.cell_height = 390;
    g_lv_top100_song_style_3x2.row_style.idx_x = 8; g_lv_top100_song_style_3x2.row_style.idx_w = 28;
    g_lv_top100_song_style_3x2.row_style.name_x = 40; g_lv_top100_song_style_3x2.row_style.name_w = 110;
    g_lv_top100_song_style_3x2.row_style.artist_x = 152; g_lv_top100_song_style_3x2.row_style.artist_w = 48;
    g_lv_top100_song_style_3x2.row_style.a_x = 206; g_lv_top100_song_style_3x2.row_style.a_w = 32;
    g_lv_top100_song_style_3x2.row_style.b_x = 242; g_lv_top100_song_style_3x2.row_style.b_w = 32;
    g_lv_top100_song_style_3x2.row_style.c_x = 286; g_lv_top100_song_style_3x2.row_style.c_w = 24;
    g_lv_top100_song_style_3x2.row_style.d_x = 314; g_lv_top100_song_style_3x2.row_style.d_w = 24;
    g_lv_top100_song_style_3x2.row_style.e_x = 342; g_lv_top100_song_style_3x2.row_style.e_w = 24;
    g_lv_top100_song_style_3x2.row_style.f_x = 370; g_lv_top100_song_style_3x2.row_style.f_w = 24;
    g_lv_top100_song_style_3x2.row_style.text_y = 182;
    g_lv_top100_song_style_3x2.row_style.bg_color = lv_color_hex(0x101010);
    g_lv_top100_song_style_3x2.row_style.bg_opa = LV_OPA_COVER;
    g_lv_top100_song_style_3x2.row_style.border_color = lv_color_hex(0x404040);
    g_lv_top100_song_style_3x2.row_style.border_width = 1;
    g_lv_top100_song_style_3x2.row_style.radius = 18;
    g_lv_top100_song_style_3x2.row_style.checked_bg_color = lv_color_hex(0x0055AA);
    g_lv_top100_song_style_3x2.row_style.checked_bg_opa = LV_OPA_COVER;
    g_lv_top100_song_style_3x2.row_style.fallback_font = &lv_font_Regular_20;
    g_lv_top100_song_style_3x2.row_style.fallback_text_color = lv_color_hex(0xFFFFFF);
    g_lv_top100_song_style_3x2.row_style.fallback_text_opa = LV_OPA_COVER;
    (void)lv_font_ktv_30;
}
