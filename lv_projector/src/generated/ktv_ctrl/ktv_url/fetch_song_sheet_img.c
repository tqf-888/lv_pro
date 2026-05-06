#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "ktv_ctrl.h"
#include "ktv_songsheet_manager.h"
#include "lv_songsheet_adapter.h"

static int image_build_path(int id, char *buf, size_t size)
{
    if (buf == NULL || size == 0U || id < 0) {
        return -1;
    }

    return app_data_make_songsheet_pic_path(id, buf, size);
}

static int image_build_url(int id, char *buf, size_t size)
{
    if (buf == NULL || size == 0U || id < 0) {
        return -1;
    }

    return ktv_get_songsheet_pic_url(id, buf, size);
}

static void image_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    (void)data_len;

    if (req == NULL) {
        return;
    }

    if (result == 0) {
        const char *path = (const char *)data;
        // printf("图片下载成功: %s\n", (path != NULL) ? path : "");
        ktv_set_songsheet_pic_available(req->id, 1);
        lv_songsheet_adapter_notify_default_image_ready((uint32_t)req->id, true);
    } else {
        // printf("图片下载失败: id=%d\n", req->id);
        ktv_set_songsheet_pic_available(req->id, 0);
        lv_songsheet_adapter_notify_default_image_ready((uint32_t)req->id, false);
    }
}

static const KtvReqOps_t g_ops_download_image = {
    .type = KTV_REQ_DOWNLOAD_IMAGE,
    .on_complete = image_on_complete
};

void fetch_song_sheet_img(int id)
{
    KtvRequest_t req;

    if (id < 0) {
        return;
    }

    memset(&req, 0, sizeof(req));

    if (image_build_url(id, req.url, sizeof(req.url)) != 0) {
        printf("图片URL获取失败: id=%d\n", id);
        ktv_set_songsheet_pic_available(id, 0);
        lv_songsheet_adapter_notify_default_image_ready((uint32_t)id, false);
        return;
    }

    if (image_build_path(id, req.local_path, sizeof(req.local_path)) != 0) {
        printf("图片路径生成失败: id=%d\n", id);
        ktv_set_songsheet_pic_available(id, 0);
        lv_songsheet_adapter_notify_default_image_ready((uint32_t)id, false);
        return;
    }

    req.ops = &g_ops_download_image;
    req.id = id;
    req.page_index = -1;
    req.user_data = NULL;

    if (Ktv_Ctrl_PostTask(&req) != 0) {
        printf("图片任务投递失败: id=%d\n", id);
        ktv_set_songsheet_pic_available(id, 0);
        lv_songsheet_adapter_notify_default_image_ready((uint32_t)id, false);
    }
}
