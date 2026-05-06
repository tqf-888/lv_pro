#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ktv_ctrl.h"
#include "ktv.h"
#include "adapter/lv_order_song_adapter.h"
#include "page_manager.h"



static int order_song_build_batch_path(int batch_index, uint32_t request_epoch, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0U || batch_index < 0) {
        return -1;
    }

    n = snprintf(buf, size, "/tmp/order_song_batch_%u_%d.json", request_epoch, batch_index);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

static int order_song_build_batch_url(int batch_index, char *buf, size_t size)
{
    int n;

    (void)batch_index;

    if (buf == NULL || size == 0U) {
        return -1;
    }
    if(page_get() == 10)
      n = snprintf(buf, size, "%s", "https://tuoge.djyos.com/song/order/list?deviceSn=16666666666&status=0");
    else
      n = snprintf(buf, size, "%s", "https://tuoge.djyos.com/song/order/list?deviceSn=16666666666&status=2");

    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

static void order_song_batch_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    uint32_t request_epoch;

    (void)data_len;

    if (req == NULL) {
        return;
    }

    request_epoch = (uint32_t)(uintptr_t)req->user_data;

    if (result == 0) {
        const char *path = (const char *)data;
        printf("order_song batch下载成功: batch=%d epoch=%u path=%s\n",
               req->page_index,
               request_epoch,
               (path != NULL) ? path : "");
        lv_order_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)req->page_index,
                                                                    true,
                                                                    request_epoch);
    } else {
        printf("order_song batch下载失败: batch=%d epoch=%u\n",
               req->page_index,
               request_epoch);
        lv_order_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)req->page_index,
                                                                    false,
                                                                    request_epoch);
    }
}

static const KtvReqOps_t g_ops_download_order_song_batch = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = order_song_batch_on_complete
};

void fetch_order_song_batch(int batch_index)
{
    KtvRequest_t req;
    uint32_t request_epoch;

    if (batch_index != 0) {
        return;
    }

    memset(&req, 0, sizeof(req));
    request_epoch = lv_order_song_adapter_get_default_request_epoch();

    if (order_song_build_batch_url(batch_index, req.url, sizeof(req.url)) != 0) {
        printf("order_song batch url生成失败: batch=%d epoch=%u\n", batch_index, request_epoch);
        lv_order_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)batch_index, false, request_epoch);
        return;
    }

    if (order_song_build_batch_path(batch_index, request_epoch, req.local_path, sizeof(req.local_path)) != 0) {
        printf("order_song batch path生成失败: batch=%d epoch=%u\n", batch_index, request_epoch);
        lv_order_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)batch_index, false, request_epoch);
        return;
    }

    req.ops = &g_ops_download_order_song_batch;
    req.id = batch_index;
    req.page_index = batch_index;
    req.user_data = (void *)(uintptr_t)request_epoch;

    if (Ktv_Ctrl_PostTask(&req) != 0) {
        printf("order_song batch任务投递失败: batch=%d epoch=%u\n", batch_index, request_epoch);
        lv_order_song_adapter_notify_default_batch_ready_with_epoch((uint32_t)batch_index, false, request_epoch);
    }
}
