#include "favorite_song_page_demo.h"
#include "lv_favorite_song_adapter.h"
#include <string.h>
static favorite_song_page_demo_t g_page1;

int favorite_song_page_demo_open(favorite_song_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t batch_size,
                             const lv_favorite_song_view_style_t *view_style)
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

    if (lv_favorite_song_adapter_start(&page->adapter, view_style, total_count, batch_size) != 0) {
        lv_obj_del(page->root);
        page->root = NULL;
        return -3;
    }

    page->vlist = lv_favorite_song_adapter_create_vlist(&page->adapter, page->root);
    if (page->vlist == NULL) {
        lv_favorite_song_adapter_stop(&page->adapter);
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

void favorite_song_page_demo_close(void)
{
    lv_favorite_song_adapter_stop(&g_page1.adapter);

    if (g_page1.root != NULL) {
        lv_obj_del(g_page1.root);
        g_page1.root = NULL;
    }

    memset(&g_page1, 0, sizeof(g_page1));
}

void favorite_song_page_demo_reset(uint32_t total_count)
{
    if (g_page1.root == NULL) {
        return;
    }

    lv_favorite_song_adapter_reset(&g_page1.adapter, total_count);
}


#include "lv_favorite_song_view_style.h"
#include "lv_favorite_song_style_3x2.h"
#include "lv_favorite_song_style_2x3.h"
#include "lv_favorite_song_style_1x8.h"



static void app_ui_init(void)
{
    static int initial_flag = 0;
    if(initial_flag == 0)
    {
        lv_favorite_song_default_style_init();
        lv_favorite_song_style_3x2_init();
        lv_favorite_song_style_2x3_init();
        lv_favorite_song_style_1x8_init();
        initial_flag ++;
    }
}

void demo_app_favorite_songs_list(lv_obj_t *parent)
{
    app_ui_init();

    if (g_page1.root != NULL) {
        favorite_song_page_demo_close();
    }

    favorite_song_page_demo_open(&g_page1,
                            parent,
                            1500,
                            50,
                            &g_lv_favorite_song_default_style);
}