#include "rich_song_page_demo.h"
#include "lv_rich_song_adapter.h"
#include <string.h>
static rich_song_page_demo_t g_page1;

int rich_song_page_demo_open(rich_song_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t batch_size,
                             const lv_rich_song_view_style_t *view_style)
{
    if (page == NULL || parent == NULL || total_count == 0U || view_style == NULL) {
        return -1;
    }

    memset(page, 0, sizeof(*page));
    page->parent = parent;
    page->batch_size = batch_size;
    page->view_style = view_style;

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

    if (lv_rich_song_adapter_start(&page->adapter, view_style, total_count, batch_size) != 0) {
        lv_obj_del(page->root);
        page->root = NULL;
        return -3;
    }

    page->vlist = lv_rich_song_adapter_create_vlist(&page->adapter, page->root);
    if (page->vlist == NULL) {
        lv_rich_song_adapter_stop(&page->adapter);
        lv_obj_del(page->root);
        page->root = NULL;
        return -4;
    }

    /*
     * open 完成后，立即让 vlist 按当前首屏状态走一次可见区请求。
     *
     * 虽然 create -> reload 期间通常已经会触发首批绑定，
     * 但这里再主动补一次，能避免后续有人改动 create/reload 流程后，
     * 首屏数据请求退化成“必须等交互后才触发”。
     */
    lv_vlist_request_visible_images(page->vlist);

    return 0;
}

void rich_song_page_demo_close(void)
{
    lv_rich_song_adapter_stop(&g_page1.adapter);

    if (g_page1.root != NULL) {
        lv_obj_del(g_page1.root);
        g_page1.root = NULL;
    }

    memset(&g_page1, 0, sizeof(g_page1));
}

void rich_song_page_demo_reset(uint32_t total_count)
{
    lv_obj_t *parent;
    uint32_t batch_size;
    const lv_rich_song_view_style_t *view_style;

    if (g_page1.root == NULL || total_count == 0U) {
        return;
    }

    /*
     * 不能只做 adapter_reset。
     * 原来的 vlist 滚动/绑定状态还停在旧位置，导致：
     * 1. reset 后不会回到第 0 页；
     * 2. 首批数据回来后，当前可视 cell 没有被立即重绑，必须滑动一下才刷新。
     *
     * 直接重建当前 page，可以彻底清掉 vlist 的旧滚动状态和旧 cell 绑定状态，
     * 重新从第 0 页开始显示。
     */
    parent = (g_page1.parent != NULL) ? g_page1.parent : lv_obj_get_parent(g_page1.root);
    batch_size = (g_page1.batch_size != 0U) ? g_page1.batch_size : g_page1.adapter.batch_size;
    view_style = (g_page1.view_style != NULL) ? g_page1.view_style : g_page1.adapter.view_style;

    if (parent == NULL || view_style == NULL || batch_size == 0U) {
        lv_rich_song_adapter_reset(&g_page1.adapter, total_count);
        if (g_page1.vlist != NULL) {
            lv_vlist_request_visible_images(g_page1.vlist);
        }
        return;
    }

    rich_song_page_demo_close();
    (void)rich_song_page_demo_open(&g_page1,
                                   parent,
                                   total_count,
                                   batch_size,
                                   view_style);
}


#include "lv_rich_song_view_style.h"
#include "lv_rich_song_style_3x2.h"
#include "lv_rich_song_style_2x3.h"
#include "lv_rich_song_style_1x8.h"



static void app_ui_init(void)
{
    static int initial_flag = 0;
    if(initial_flag == 0)
    {
        lv_rich_song_default_style_init();
        lv_rich_song_style_3x2_init();
        lv_rich_song_style_2x3_init();
        lv_rich_song_style_1x8_init();
        initial_flag ++;
    }
}

void demo_app_songs_list(lv_obj_t *parent)
{
    app_ui_init();

    if (g_page1.root != NULL) {
        rich_song_page_demo_close();
    }

    rich_song_page_demo_open(&g_page1,
                            parent,
                            1500,
                            50,
                            &g_lv_rich_song_default_style);
}