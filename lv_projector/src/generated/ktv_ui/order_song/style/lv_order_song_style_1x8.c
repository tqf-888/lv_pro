#include "lv_order_song_style_1x8.h"
#include <string.h>
LV_FONT_DECLARE(lv_font_Regular_20);
LV_FONT_DECLARE(lv_font_ktv_30);
lv_order_song_view_style_t g_lv_order_song_style_1x8;
void lv_order_song_style_1x8_init(void)
{
    memset(&g_lv_order_song_style_1x8, 0, sizeof(g_lv_order_song_style_1x8));
    g_lv_order_song_style_1x8.visible_cols = 1;
    g_lv_order_song_style_1x8.visible_rows = 8;
    /*
     * 前向 overscan 仍然保留，用于向上回滚时减少临界闪烁。
     * 但它只能对“真实存在的前置行”生效。
     * 顶部首屏的 clamp 必须由 lv_virtual_list.c 内部处理，不能靠这里把功能关掉来规避。
     */
    g_lv_order_song_style_1x8.overscan_rows_front = 2;
    g_lv_order_song_style_1x8.overscan_rows_back = 2;
    g_lv_order_song_style_1x8.preload_before = 8;
    g_lv_order_song_style_1x8.preload_after = 24;
    g_lv_order_song_style_1x8.viewport_width = 650;
    g_lv_order_song_style_1x8.viewport_height = 480;
    g_lv_order_song_style_1x8.gap_x = 0;
    g_lv_order_song_style_1x8.gap_y = 0;
    g_lv_order_song_style_1x8.cell_width = 650;
    g_lv_order_song_style_1x8.cell_height = 60;
    g_lv_order_song_style_1x8.row_style.cell_width = 650;
    g_lv_order_song_style_1x8.row_style.cell_height = 60;
    g_lv_order_song_style_1x8.row_style.idx_x = 10; g_lv_order_song_style_1x8.row_style.idx_w = 40;
    g_lv_order_song_style_1x8.row_style.name_x = 56; g_lv_order_song_style_1x8.row_style.name_w = 150;
    g_lv_order_song_style_1x8.row_style.artist_x = 210; g_lv_order_song_style_1x8.row_style.artist_w = 100;
    g_lv_order_song_style_1x8.row_style.a_x = 316; g_lv_order_song_style_1x8.row_style.a_w = 60;
    g_lv_order_song_style_1x8.row_style.b_x = 382; g_lv_order_song_style_1x8.row_style.b_w = 60;
    g_lv_order_song_style_1x8.row_style.c_x = 470; g_lv_order_song_style_1x8.row_style.c_w = 35;
    g_lv_order_song_style_1x8.row_style.d_x = 510; g_lv_order_song_style_1x8.row_style.d_w = 35;
    g_lv_order_song_style_1x8.row_style.e_x = 550; g_lv_order_song_style_1x8.row_style.e_w = 35;
    g_lv_order_song_style_1x8.row_style.f_x = 590; g_lv_order_song_style_1x8.row_style.f_w = 35;
    g_lv_order_song_style_1x8.row_style.text_y = 18;
    g_lv_order_song_style_1x8.row_style.bg_color = lv_color_hex(0x000000);
    g_lv_order_song_style_1x8.row_style.bg_opa = LV_OPA_TRANSP;
    g_lv_order_song_style_1x8.row_style.border_color = lv_color_hex(0x000000);
    g_lv_order_song_style_1x8.row_style.border_width = 0;
    g_lv_order_song_style_1x8.row_style.radius = 0;
    g_lv_order_song_style_1x8.row_style.checked_bg_color = lv_color_hex(0x0055AA);
    g_lv_order_song_style_1x8.row_style.checked_bg_opa = LV_OPA_COVER;
    g_lv_order_song_style_1x8.row_style.fallback_font = &lv_font_Regular_20;
    g_lv_order_song_style_1x8.row_style.fallback_text_color = lv_color_hex(0xFFFFFF);
    g_lv_order_song_style_1x8.row_style.fallback_text_opa = LV_OPA_COVER;
    (void)lv_font_ktv_30;
}
