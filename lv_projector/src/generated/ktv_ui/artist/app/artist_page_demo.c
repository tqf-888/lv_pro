#include "artist_page_demo.h"

#include <string.h>
#include <stdio.h>

#include "lv_artist_view_style.h"
#include "lv_artist_style_3x2.h"
#include "lv_artist_style_2x3.h"
#include "lv_artist_style_1x8.h"
#include "lv_artist_style_3x4.h"
#include "lv_artist_style_5x2.h"
#include "lv_artist_style_2x4.h"
#include "artist_media_loader_demo.h"

static artist_page_demo_t g_artist_page;

static void artist_page_demo_delete_root_now(artist_page_demo_t *page)
{
    if (page == NULL || page->root == NULL) {
        return;
    }

    lv_obj_del(page->root);
}

static void artist_page_demo_release_runtime(artist_page_demo_t *page)
{
    if (page == NULL) {
        return;
    }

    if (page->adapter.started) {
        lv_artist_adapter_stop(&page->adapter);
    }

    if (page->loader_inited) {
        /* 关闭页面时同步释放图片管理器与 slot meta，避免旧页面残留状态泄漏到下次打开。 */
        demo_ui_deinit_all();
        page->loader_inited = 0U;
    }

    page->vlist = NULL;
}


static void artist_page_root_delete_event_cb(lv_event_t *e)
{
    artist_page_demo_t *page = (artist_page_demo_t *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (page == NULL) {
        return;
    }

    if (page->root == target) {
        page->root = NULL;
    }

    artist_page_demo_release_runtime(page);
    page->closing = 0U;
}

int artist_page_demo_open(artist_page_demo_t *page,
                          lv_obj_t *parent,
                          uint32_t total_count,
                          uint32_t json_page_size,
                          const lv_artist_view_style_t *view_style)
{
    int ret;

    if (page == NULL || parent == NULL || total_count == 0U || json_page_size == 0U || view_style == NULL) {
        return -1;
    }

    artist_page_demo_close(page);
    memset(page, 0, sizeof(*page));

    page->root = lv_obj_create(parent);
    if (page->root == NULL) {
        return -4;
    }

    lv_obj_set_size(page->root, view_style->viewport_width, view_style->viewport_height);
    lv_obj_set_pos(page->root, 0, 0);
    lv_obj_clear_flag(page->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(page->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(page->root, 0, 0);
    lv_obj_set_style_border_width(page->root, 0, 0);
    lv_obj_set_style_radius(page->root, 0, 0);
    lv_obj_set_style_bg_opa(page->root, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(page->root, artist_page_root_delete_event_cb, LV_EVENT_DELETE, page);

    /* 页面打开时先初始化图片管理器，后续首屏预取和 timer 轮询都依赖它。 */
    demo_image_manager_init();
    page->loader_inited = 1U;

    ret = lv_artist_adapter_start(&page->adapter, view_style, total_count, json_page_size);
    if (ret != 0) {
        artist_page_demo_release_runtime(page);
        lv_obj_del(page->root);
        page->root = NULL;
        return -5;
    }

    page->vlist = lv_artist_adapter_create_vlist(&page->adapter, page->root);
    if (page->vlist == NULL) {
        artist_page_demo_release_runtime(page);
        lv_obj_del(page->root);
        page->root = NULL;
        return -6;
    }

    lv_vlist_scroll_to(page->vlist, 0U);
    /* 首屏立即预拉当前页 + 后两页，避免必须等 ui_timer 第一次触发才开始缓存。 */
    lv_artist_adapter_prime_first_screen(&page->adapter);

    printf("artist_page_demo_open ok: root=%p vlist=%p json_page_size=%u\n",
           (void *)page->root, (void *)page->vlist, json_page_size);
    return 0;
}

void artist_page_demo_close(artist_page_demo_t *page)
{
    if (page == NULL) {
        return;
    }

    page->closing = 1U;
    artist_page_demo_release_runtime(page);
    artist_page_demo_delete_root_now(page);

    if (page->root == NULL) {
        page->closing = 0U;
    }
}

void artist_page_demo_reset(artist_page_demo_t *page, uint32_t total_count)
{
    if (page == NULL) {
        return;
    }

    lv_artist_adapter_reset(&page->adapter, total_count);
}

void artist_page_demo_reset_to_page0(artist_page_demo_t *page, uint32_t total_count)
{
    if (page == NULL) {
        return;
    }

    /*
     * reset_to_page0 不只是 UI 回到顶部。
     *
     * 歌手类别切换后，page0 的 URL 会因为 subpage/name 等条件变化而变化；如果只
     * reset adapter/catalog，image_manager 里的 page_requested/page_ready 和旧
     * slot URL 还在，回到 page0 会被当成“JSON 已经拉过”，不会重新请求，也会继续
     * 使用旧类别的图片缓存。
     *
     * 所以这里必须先 reset 媒体层：generation 递增、页请求状态清空、旧本地图片删除、
     * slot meta 清空。正在路上的旧下载即使回来，也会因为 generation 不匹配被丢弃。
     */
    if (page->loader_inited) {
        demo_ui_reset_all();
    }

    lv_artist_adapter_reset(&page->adapter, total_count);
    if (page->vlist != NULL) {
        lv_vlist_scroll_to(page->vlist, 0U);
    }
    if (page->adapter.started) {
        lv_artist_adapter_prime_first_screen(&page->adapter);
    }
}

void app_ui_init(void)
{
    static int initial_flag = 0;

    if (initial_flag != 0) {
        return;
    }

    lv_artist_default_style_init();
    lv_artist_style_3x2_init();
    lv_artist_style_2x3_init();
    lv_artist_style_1x8_init();
    lv_artist_style_3x4_init();
    lv_artist_style_5x2_init();
    lv_artist_style_2x4_init();

    initial_flag = 1;
}

void app_open_artist_page(lv_obj_t *parent)
{
    int ret;
    uint32_t total_count = 300;
    /*
     * 服务端 JSON 一页 50 条。UI 一屏 2x4=8 条由 adapter 自己从 view_style 计算，
     * 不要再把 8 当 json_page_size 传进去，否则 page/window 语义又会混在一起。
     */
    uint32_t json_page_size = 50U;

    app_ui_init();

    ret = artist_page_demo_open(&g_artist_page,
                                parent,
                                total_count,
                                json_page_size,
                                &g_lv_artist_style_2x4);
    if (ret != 0) {
        printf("artist_page_demo_open failed:%d\n", ret);
    }
}

void app_close_artist_page(void)
{
    artist_page_demo_close(&g_artist_page);
    printf("app_close_artist_page\n");
}

void app_prepare_close_artist_page(void)
{
    artist_page_demo_close(&g_artist_page);
}

void app_reset_artist_page_to_page0(uint32_t total_count)
{
    artist_page_demo_reset_to_page0(&g_artist_page, total_count);
}
