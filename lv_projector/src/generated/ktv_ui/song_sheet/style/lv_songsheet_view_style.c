#include "lv_songsheet_view_style.h"
#include "lv_layout_styles.h"

static int g_lv_songsheet_styles_inited = 0;

void lv_songsheet_styles_init(void)
{
    if (g_lv_songsheet_styles_inited) return;
    lv_layout_styles_init();
    g_lv_songsheet_styles_inited = 1;
}