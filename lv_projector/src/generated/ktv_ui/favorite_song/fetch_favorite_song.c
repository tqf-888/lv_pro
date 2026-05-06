#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ktv_ctrl.h"
#include "lv_favorite_song_adapter.h"
#include "lv_favorite_song_debug.h"

#define FAVORITE_SONG_BATCH_SIZE 50
#define FAVORITE_SONG_PHONE      "16666666666"
#define SONG_FAVORITE_PAGE     "https://tuoge.djyos.com/sys/song/favorite/page?page=%d&limit=50&phone=%s"

static int favorite_song_build_batch_path(int batch_index, uint32_t request_epoch, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0U || batch_index < 0) {
        RS_FLOW_LOG("build path invalid args: batch=%d", batch_index);
        return -1;
    }

    n = snprintf(buf, size, "/tmp/favorite_song_batch_%u_%d.json", request_epoch, batch_index);
    if (n < 0 || (size_t)n >= size) {
        RS_FLOW_LOG("build path overflow: batch=%d epoch=%u size=%lu", batch_index, request_epoch, (unsigned long)size);
        return -1;
    }

    RS_FLOW_LOG("build path ok: batch=%d epoch=%u path=%s", batch_index, request_epoch, buf);
    return 0;
}

static int favorite_song_build_batch_url(int batch_index, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0U || batch_index < 0) {
        RS_FLOW_LOG("build url invalid args: batch=%d", batch_index);
        return -1;
    }
    batch_index++;
    /*
     * 这里按业务要求直接使用完整 HTTP 接口，不再经过 ktv_build_base_url。
     * 当前保持和 batch_index 一样的页号传入；
     * 如果后台页号是从 1 开始，只需要把 batch_index 改成 (batch_index + 1)。
     */
    n = snprintf(buf, size, SONG_FAVORITE_PAGE, batch_index, FAVORITE_SONG_PHONE);
    if (n < 0 || (size_t)n >= size) {
        RS_FLOW_LOG("build url overflow: batch=%d size=%lu", batch_index, (unsigned long)size);
        return -1;
    }

    RS_FLOW_LOG("build url ok: batch=%d url=%s", batch_index, buf);
    return 0;
}

static void favorite_song_batch_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    uint32_t request_epoch;

    (void)data_len;

    if (req == NULL) {
        return;
    }

    request_epoch = (uint32_t)(uintptr_t)req->user_data;

    if (result == 0) {
        const char *path = (const char *)data;
        RS_FLOW_LOG("batch download complete ok: batch=%d epoch=%u result=%d path=%s",
                    req->page_index, request_epoch, result, (path != NULL) ? path : "");
        printf("favorite_song batch下载成功: batch=%d epoch=%u path=%s\n",
               req->page_index,
               request_epoch,
               (path != NULL) ? path : "");
        lv_favorite_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)req->page_index,
                                                                       true,
                                                                       request_epoch);
    } else {
        RS_FLOW_LOG("batch download complete failed: batch=%d epoch=%u result=%d",
                    req->page_index, request_epoch, result);
        printf("favorite_song batch下载失败: batch=%d epoch=%u\n",
               req->page_index,
               request_epoch);
        lv_favorite_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)req->page_index,
                                                                       false,
                                                                       request_epoch);
    }
}

static const KtvReqOps_t g_ops_download_favorite_song_batch = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = favorite_song_batch_on_complete
};

void fetch_favorite_song_batch(int batch_index)
{
    KtvRequest_t req;
    uint32_t request_epoch;

    if (batch_index < 0) {
        return;
    }
    
    memset(&req, 0, sizeof(req));
    request_epoch = lv_favorite_song_adapter_get_default_request_epoch();
    RS_FLOW_LOG("fetch batch start: batch=%d epoch=%u", batch_index, request_epoch);

    if (favorite_song_build_batch_url(batch_index, req.url, sizeof(req.url)) != 0) {
        printf("favorite_song batch url生成失败: batch=%d epoch=%u\n", batch_index, request_epoch);
        lv_favorite_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)batch_index,
                                                                       false,
                                                                       request_epoch);
        return;
    }

    if (favorite_song_build_batch_path(batch_index, request_epoch, req.local_path, sizeof(req.local_path)) != 0) {
        printf("favorite_song batch path生成失败: batch=%d epoch=%u\n", batch_index, request_epoch);
        lv_favorite_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)batch_index,
                                                                       false,
                                                                       request_epoch);
        return;
    }

    req.ops = &g_ops_download_favorite_song_batch;
    req.id = batch_index;
    req.page_index = batch_index;
    req.user_data = (void *)(uintptr_t)request_epoch;

    RS_FLOW_LOG("post download task: batch=%d epoch=%u url=%s path=%s",
                batch_index, request_epoch, req.url, req.local_path);

    if (Ktv_Ctrl_PostTask(&req) != 0) {
        printf("favorite_song batch任务投递失败: batch=%d epoch=%u\n", batch_index, request_epoch);
        lv_favorite_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)batch_index,
                                                                       false,
                                                                       request_epoch);
    } else {
        RS_FLOW_LOG("post download task ok: batch=%d epoch=%u", batch_index, request_epoch);
    }
}
