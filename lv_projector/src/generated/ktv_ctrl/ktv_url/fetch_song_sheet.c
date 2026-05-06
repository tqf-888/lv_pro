#include <stdio.h>
#include <string.h>

#include "ktv_ctrl.h"
#include "ktv_songsheet_manager.h"
#include "lv_songsheet_adapter.h"
#include "ktv.h"

static int page_build_path(int page_index, char *buf, size_t size)
{
    if (buf == NULL || size == 0U || page_index < 0) {
        return -1;
    }

    return ktv_get_songsheet_json_path(page_index, buf, size);
}

static int page_build_url(int page_index, char *buf, size_t size)
{
    if (buf == NULL || size == 0U || page_index < 0) {
        return -1;
    }

    return ktv_build_base_url(KTV_4_7_ALL_SONG_SHEET_LIST,
                              buf,
                              size,
                              50,
                              page_index);
}

static void page_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    (void)data_len;

    if (req == NULL) {
        return;
    }

    if (result == 0) {
        const char *path = (const char *)data;
        printf("json下载成功: %s\n", (path != NULL) ? path : "");
        lv_songsheet_adapter_notify_default_page_ready((uint32_t)req->page_index, true);
    } else {
        printf("json下载失败: page=%d\n", req->page_index);
        lv_songsheet_adapter_notify_default_page_ready((uint32_t)req->page_index, false);
    }
}

static const KtvReqOps_t g_ops_download_page = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = page_on_complete
};

void fetch_song_sheet(int page_index)
{
    KtvRequest_t req;

    if (page_index < 0) {
        return;
    }

    memset(&req, 0, sizeof(req));

    if (page_build_url(page_index, req.url, sizeof(req.url)) != 0) {
        printf("json url生成失败: page=%d\n", page_index);
        lv_songsheet_adapter_notify_default_page_ready((uint32_t)page_index, false);
        return;
    }

    if (page_build_path(page_index, req.local_path, sizeof(req.local_path)) != 0) {
        printf("json path生成失败: page=%d\n", page_index);
        lv_songsheet_adapter_notify_default_page_ready((uint32_t)page_index, false);
        return;
    }

    req.ops = &g_ops_download_page;
    req.id = page_index;
    req.page_index = page_index;
    req.user_data = NULL;

    if (Ktv_Ctrl_PostTask(&req) != 0) {
        printf("json任务投递失败: page=%d\n", page_index);
        lv_songsheet_adapter_notify_default_page_ready((uint32_t)page_index, false);
    }
}
