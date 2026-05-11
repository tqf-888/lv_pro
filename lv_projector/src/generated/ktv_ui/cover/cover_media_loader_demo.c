#include <stdio.h>
#include <string.h>

#include "image_manager.h"
#include "cover_media_loader.h"
#include "lv_cover_adapter.h"

static int cover_demo_request_page(void *user_ctx,
                                   uint32_t page_index,
                                   uint32_t page_size,
                                   uint32_t generation)
{
    (void)user_ctx;
    return cover_media_fetch_page(page_index, page_size, generation);
}

static void cover_demo_need_image(void *user_ctx,
                                  uint32_t slot_id,
                                  uint32_t content_id,
                                  const char *url,
                                  uint32_t generation,
                                  int high_priority)
{
    (void)user_ctx;
    (void)cover_media_fetch_image(slot_id, content_id, url, generation, high_priority);
}

static int cover_demo_remove_file(void *user_ctx, const char *path)
{
    (void)user_ctx;
    (void)path;
    /* cover 使用 /usr/share/lv_projector/pic 下的原始图片，不能被缓存淘汰逻辑 remove。 */
    return 0;
}

void cover_image_manager_init(void)
{
    img_mgr_cfg_t cfg;

    memset(&cfg, 0, sizeof(cfg));
    cover_media_reset_runtime_cache();
    cfg.page_size = 50;
    cfg.max_cached_items = 1000;
    cfg.max_cached_pages = 30;
    cfg.request_page = cover_demo_request_page;
    cfg.need_image = cover_demo_need_image;
    cfg.remove_file = cover_demo_remove_file;
    cfg.cleanup_session = NULL;
    cfg.user_ctx = NULL;

    (void)img_mgr_init(&cfg);
}

void cover_ui_scroll_range(uint32_t start_slot_id, uint32_t end_slot_id)
{
    img_mgr_access_range(start_slot_id, end_slot_id);
}

const char *cover_ui_get_image_path(uint32_t slot_id,
                                    uint32_t *out_content_id,
                                    const char **out_desc_str)
{
    const char *path = img_mgr_pull_tls(slot_id);

    if (cover_media_get_slot_meta(slot_id, out_content_id, out_desc_str) != 0) {
        if (out_content_id != NULL) *out_content_id = 0U;
        if (out_desc_str != NULL) *out_desc_str = NULL;
    }
    return path;
}

int cover_ui_has_more(void)
{
    return img_mgr_is_last();
}

void cover_ui_reset_all(void)
{
    img_mgr_reset();
    cover_media_reset_runtime_cache();
}

void cover_ui_deinit_all(void)
{
    img_mgr_deinit();
    cover_media_reset_runtime_cache();
}
