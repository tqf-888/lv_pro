#include "rich_rank_page_demo.h"
#include <string.h>

#include "lv_rich_song_view_style.h"
#include "lv_rich_song_style_3x2.h"
#include "lv_rich_song_style_2x3.h"
#include "lv_rich_song_style_1x8.h"

static rich_rank_page_demo_t g_rank_page;

int rich_rank_page_demo_open(rich_rank_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t batch_size,
                             const lv_rich_song_view_style_t *view_style)
{
    if (page == NULL || parent == NULL || total_count == 0U || view_style == NULL) {
        return -1;
    }

    memset(page, 0, sizeof(*page));

    page->root = lv_obj_create(parent);
    if (page->root == NULL) {
        return -2;
    }

    lv_obj_set_size(page->root, view_style->viewport_width, view_style->viewport_height);
    lv_obj_set_pos(page->root, 0, 0);
    lv_obj_clear_flag(page->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(page->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(page->root, 0, 0);
    lv_obj_set_style_border_width(page->root, 0, 0);
    lv_obj_set_style_radius(page->root, 0, 0);
    lv_obj_set_style_bg_opa(page->root, LV_OPA_TRANSP, 0);

    if (lv_rich_rank_adapter_start(&page->adapter, view_style, total_count, batch_size) != 0) {
        lv_obj_del(page->root);
        page->root = NULL;
        return -3;
    }

    page->vlist = lv_rich_rank_adapter_create_vlist(&page->adapter, page->root);
    if (page->vlist == NULL) {
        lv_rich_rank_adapter_stop(&page->adapter);
        lv_obj_del(page->root);
        page->root = NULL;
        return -4;
    }

    lv_vlist_request_visible_images(page->vlist);
    return 0;
}

void rich_rank_page_demo_close(void)
{
    lv_rich_rank_adapter_stop(&g_rank_page.adapter);

    if (g_rank_page.root != NULL) {
        lv_obj_del(g_rank_page.root);
        g_rank_page.root = NULL;
    }

    memset(&g_rank_page, 0, sizeof(g_rank_page));
}

void rich_rank_page_demo_reset(uint32_t total_count)
{
    if (g_rank_page.root == NULL) {
        return;
    }

    lv_rich_rank_adapter_reset(&g_rank_page.adapter, total_count);
}

static void app_ui_init(void)
{
    static int initial_flag = 0;
    if (initial_flag == 0) {
        lv_rich_song_default_style_init();
        lv_rich_song_style_3x2_init();
        lv_rich_song_style_2x3_init();
        lv_rich_song_style_1x8_init();
        initial_flag++;
    }
}

void demo_app_rank_list(lv_obj_t *parent)
{
    app_ui_init();

    if (g_rank_page.root != NULL) {
        rich_rank_page_demo_close();
    }

    /*
     * 这里 total_count 只是首开兜底值；
     * 真正精确总数会在首批 json 返回后，按 result.total_count 自动收缩。
     */
    rich_rank_page_demo_open(&g_rank_page,
                             parent,
                             500,
                             50,
                             &g_lv_rich_song_default_style);
}
