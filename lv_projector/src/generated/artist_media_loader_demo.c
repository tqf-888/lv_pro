#include <stdio.h>
#include <string.h>

#include "image_manager.h"
#include "artist_media_loader.h"
#include "lv_artist_adapter.h"

/*
 * 这个 demo 专门演示正式工程里的分层接法：
 * 1. image_manager 只做状态管理
 * 2. artist_media_loader 单独负责下载/解析/回填
 * 3. JSON 解析使用 cJSON
 * 4. 外部 UI 只需要 access / pull / is_last / reset
 *
 * 注意：
 * - 这里的 KtvReqOps_t 仅示意如何接入你现有下载框架
 * - 你要把 req.ops 换成你工程里真实使用的那套静态 ops
 */

static int demo_request_page(void *user_ctx,
                             uint32_t page_index,
                             uint32_t page_size,
                             uint32_t generation)
{
    (void)user_ctx;
    return artist_media_fetch_page(page_index, page_size, generation);
}

static void demo_need_image(void *user_ctx,
                            uint32_t slot_id,
                            uint32_t content_id,
                            const char *url,
                            uint32_t generation,
                            int high_priority)
{
    (void)user_ctx;
    /* high_priority 直接转给 artist_media_fetch_image，它会写到 KtvRequest_t.priority，
     * 进 ktv_ctrl → http_download_priority → http_pool 的 overflow 队首。 */
    (void)artist_media_fetch_image(slot_id, content_id, url, generation, high_priority);
}

void demo_image_manager_init(void)
{
    img_mgr_cfg_t cfg;

    memset(&cfg, 0, sizeof(cfg));
    artist_media_reset_slot_meta();
    cfg.page_size = 50;
    cfg.max_cached_items = 1000;
    cfg.max_cached_pages = 30;
    cfg.request_page = demo_request_page;
    cfg.need_image = demo_need_image;
    cfg.remove_file = NULL;
    cfg.cleanup_session = NULL;
    cfg.user_ctx = NULL;

    (void)img_mgr_init(&cfg);
}

void demo_ui_scroll_range(uint32_t start_slot_id, uint32_t end_slot_id)
{
    img_mgr_access_range(start_slot_id, end_slot_id);
}

const char *demo_ui_get_image_path(uint32_t slot_id,
                                   uint32_t *out_content_id,
                                   const char **out_name_str)
{
    const char *path = img_mgr_pull_tls(slot_id);

    if (artist_media_get_slot_meta(slot_id, out_content_id, out_name_str) != 0)
    {
        if (out_content_id != NULL)
        {
            *out_content_id = 0U;
        }
        if (out_name_str != NULL)
        {
            *out_name_str = NULL;
        }
    }
    return path;
}

int demo_ui_has_more(void)
{
    return img_mgr_is_last();
}

void demo_ui_reset_all(void)
{
    img_mgr_reset();
    artist_media_reset_slot_meta();
}

void demo_ui_deinit_all(void)
{
    img_mgr_deinit();
    artist_media_reset_slot_meta();
}
