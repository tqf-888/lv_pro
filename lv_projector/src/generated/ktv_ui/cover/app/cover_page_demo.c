#include "cover_page_demo.h"

#include <string.h>
#include <stdio.h>

#include "lv_cover_style_2x4.h"
#include "cover_media_loader.h"
#include "cover_media_loader_demo.h"

static cover_page_demo_t g_cover_page;

static void cover_page_demo_delete_root_now(cover_page_demo_t *page)
{
    if (page == NULL || page->root == NULL) return;
    lv_obj_del(page->root);
}

static void cover_page_demo_release_runtime(cover_page_demo_t *page)
{
    if (page == NULL) return;

    if (page->adapter.started) lv_cover_adapter_stop(&page->adapter);

    if (page->loader_inited) {
        cover_ui_deinit_all();
        page->loader_inited = 0U;
    }

    page->vlist = NULL;
}

static void cover_page_root_delete_event_cb(lv_event_t *e)
{
    cover_page_demo_t *page = (cover_page_demo_t *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (page == NULL) return;
    if (page->root == target) page->root = NULL;

    cover_page_demo_release_runtime(page);
    page->closing = 0U;
}

int cover_page_demo_open(cover_page_demo_t *page,
                         lv_obj_t *parent,
                         uint32_t total_count,
                         uint32_t json_page_size,
                         const lv_cover_view_style_t *view_style)
{
    int ret;

    if (page == NULL || parent == NULL || total_count == 0U || json_page_size == 0U || view_style == NULL) return -1;

    cover_page_demo_close(page);
    memset(page, 0, sizeof(*page));

    page->root = lv_obj_create(parent);
    if (page->root == NULL) return -4;

    lv_obj_set_size(page->root, view_style->viewport_width, view_style->viewport_height);
    lv_obj_set_pos(page->root, 0, 0);
    lv_obj_clear_flag(page->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(page->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(page->root, 0, 0);
    lv_obj_set_style_border_width(page->root, 0, 0);
    lv_obj_set_style_radius(page->root, 0, 0);
    lv_obj_set_style_bg_opa(page->root, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(page->root, cover_page_root_delete_event_cb, LV_EVENT_DELETE, page);

    cover_image_manager_init();
    page->loader_inited = 1U;

    ret = lv_cover_adapter_start(&page->adapter, view_style, total_count, json_page_size);
    if (ret != 0) {
        cover_page_demo_release_runtime(page);
        lv_obj_del(page->root);
        page->root = NULL;
        return -5;
    }

    page->vlist = lv_cover_adapter_create_vlist(&page->adapter, page->root);
    if (page->vlist == NULL) {
        cover_page_demo_release_runtime(page);
        lv_obj_del(page->root);
        page->root = NULL;
        return -6;
    }

    lv_vlist_scroll_to(page->vlist, 0U);
    lv_cover_adapter_prime_first_screen(&page->adapter);

    printf("cover_page_demo_open ok: root=%p vlist=%p category_pos=%d json_page_size=%u\n",
           (void *)page->root,
           (void *)page->vlist,
           cover_media_get_category_position(),
           json_page_size);
    return 0;
}

void cover_page_demo_close(cover_page_demo_t *page)
{
    if (page == NULL) return;
    page->closing = 1U;
    cover_page_demo_release_runtime(page);
    cover_page_demo_delete_root_now(page);
    if (page->root == NULL) page->closing = 0U;
}

void cover_page_demo_reset(cover_page_demo_t *page, uint32_t total_count)
{
    if (page == NULL) return;
    lv_cover_adapter_reset(&page->adapter, total_count);
}

void cover_page_demo_reset_to_page0(cover_page_demo_t *page, uint32_t total_count)
{
    if (page == NULL) return;

    if (page->loader_inited) cover_ui_reset_all();

    lv_cover_adapter_reset(&page->adapter, total_count);
    if (page->vlist != NULL) lv_vlist_scroll_to(page->vlist, 0U);
    if (page->adapter.started) lv_cover_adapter_prime_first_screen(&page->adapter);
}

void app_cover_ui_init(void)
{
    static int initial_flag = 0;
    if (initial_flag != 0) return;
    lv_cover_style_2x4_init();
    initial_flag = 1;
}

void app_cover_set_category_position(int category_pos)
{
    cover_media_set_category_position(category_pos);
    if (g_cover_page.root != NULL) {
        cover_page_demo_reset_to_page0(&g_cover_page, 300U);
    }
}

void app_open_cover_page(lv_obj_t *parent)
{
    int ret;
    uint32_t total_count = 300U;    /* 打开先用占位总数；JSON 回来后 adapter 会自动收缩到真实数量。 */
    uint32_t json_page_size = 50U;

    app_cover_ui_init();

    ret = cover_page_demo_open(&g_cover_page,
                               parent,
                               total_count,
                               json_page_size,
                               &g_lv_cover_style_2x4);
    if (ret != 0) printf("cover_page_demo_open failed:%d\n", ret);
}

void app_close_cover_page(void)
{
    cover_page_demo_close(&g_cover_page);
    printf("app_close_cover_page\n");
}

void app_prepare_close_cover_page(void)
{
    cover_page_demo_close(&g_cover_page);
}

void app_reset_cover_page_to_page0(uint32_t total_count)
{
    cover_page_demo_reset_to_page0(&g_cover_page, total_count);
}
