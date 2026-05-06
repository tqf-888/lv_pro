#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "page_manager.h"
#include "ktv_ctrl.h"
#include "ktv.h"
#include "lv_rich_rank_adapter.h"

/**
 * 构建排行榜本地保存路径。
 * 这里直接沿用你的 fetch 文件，只补上 epoch，避免 reset 后旧批次文件覆盖新批次。
 */
static int recommend_sheet_build_path(int page, uint32_t request_epoch, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0 || page < 0) {
        return -1;
    }

    if (request_epoch != 0U) {
        n = snprintf(buf, size, "/tmp/4_15_song_%u_%d.json", request_epoch, page);
        if (n >= 0 && (size_t)n < size) {
            return 0;
        }
    }

    n = snprintf(buf, size, "/tmp/4_15_song_%d.json", page);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

/**
 * 构建排行榜请求 URL。
 * 仍然完全使用你原来的 KTV_4_15_RANKING_SONG_LIST。
 */
static int recommend_sheet_build_url(int page, char *buf, size_t size)
{
    if (buf == NULL || size == 0 || page < 0) {
        return -1;
    }

    return ktv_build_base_url(KTV_4_15_RANKING_SONG_LIST, buf, size, subpage_get(), page, 50);
}

/**
 * 下载完成回调。
 * 成功/失败后，直接通知排行榜 adapter 对应 batch 已就绪，UI 线程再去读 JSON。
 */
static void recommend_sheet_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    uint32_t request_epoch = 0U;

    (void)data_len;

    if (req == NULL) {
        return;
    }

    request_epoch = (uint32_t)(uintptr_t)req->user_data;

    if (result == 0) {
        const char *path = (const char *)data;
        printf("排行榜下载成功: page=%d epoch=%u path=%s\n",
               req->page_index,
               request_epoch,
               (path != NULL) ? path : "");

        lv_rich_rank_adapter_notify_default_batch_ready_with_epoch((uint32_t)req->page_index,
                                                                   true,
                                                                   request_epoch);
    } else {
        printf("排行榜下载失败: page=%d epoch=%u\n",
               req->page_index,
               request_epoch);

        lv_rich_rank_adapter_notify_default_batch_ready_with_epoch((uint32_t)req->page_index,
                                                                   false,
                                                                   request_epoch);
    }
}

static const KtvReqOps_t g_ops_download_recommend_sheet = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = recommend_sheet_on_complete
};

/**
 * 仍然保留你的函数名，不新建 fetch_rank_batch_4_15。
 * 排行榜 adapter 会直接调这个函数。
 */
void fetch_song_by_rank_list_4_15(int page)
{
    KtvRequest_t req;
    uint32_t request_epoch;

    if (page < 0) {
        printf("fetch_song_by_rank_list_4_15: invalid page %d\n", page);
        return;
    }

    memset(&req, 0, sizeof(req));

    request_epoch = lv_rich_rank_adapter_get_default_request_epoch();

    if (recommend_sheet_build_url(page, req.url, sizeof(req.url)) != 0) {
        printf("排行榜 URL 生成失败: page=%d\n", page);
        return;
    }

    if (recommend_sheet_build_path(page, request_epoch, req.local_path, sizeof(req.local_path)) != 0) {
        printf("排行榜本地路径生成失败: page=%d epoch=%u\n", page, request_epoch);
        return;
    }

    req.ops = &g_ops_download_recommend_sheet;
    req.id = page;
    req.page_index = page;
    req.user_data = (void *)(uintptr_t)request_epoch;

    if (Ktv_Ctrl_PostTask(&req) != 0) {
        printf("排行榜任务投递失败: page=%d epoch=%u\n", page, request_epoch);
        lv_rich_rank_adapter_notify_default_batch_ready_with_epoch((uint32_t)page,
                                                                   false,
                                                                   request_epoch);
    }
}
