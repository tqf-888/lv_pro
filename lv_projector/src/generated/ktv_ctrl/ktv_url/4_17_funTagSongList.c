#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "ktv_ctrl.h"
#include "ktv.h"

/**
 * 构建推荐歌单本地保存路径
 * @param page       页码（从0或1开始，取决于API定义）
 * @param buf        输出缓冲区
 * @param size       缓冲区大小
 * @return           0成功，-1失败
 */
static int recommend_sheet_build_path(int page, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0 || page < 0) {
        return -1;
    }

    n = snprintf(buf, size, "/tmp/recommend_song_sheet_%d.json", page);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

/**
 * 构建推荐歌单请求URL
 * @param page       页码
 * @param buf        输出缓冲区
 * @param size       缓冲区大小
 * @return           0成功，-1失败
 *
 * 注意：ktv_build_base_url 的可变参数格式取决于具体实现。
 * 此处假设推荐歌单API支持 page 和 size 两个参数，均为整数类型。
 * 如果实际API需要其他参数（如用户ID、设备信息等），请根据文档修改。
 */
static int recommend_sheet_build_url(int page, char *buf, size_t size)
{
    if (buf == NULL || size == 0 || page < 0) {
        return -1;
    }

    /* 假设API接受分页参数：page（页码，从0或1开始），size（每页数量） */
    return ktv_build_base_url(KTV_4_17_FUN_TAG_SONG_LIST, buf, size, page);  /* 以NULL结尾，表示参数结束 */
}

/**
 * 下载完成回调函数
 * @param req        请求对象
 * @param result     0成功，非0失败
 * @param data       成功时为本地文件路径，失败时为错误信息
 * @param data_len   数据长度（未使用）
 */
static void recommend_sheet_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    (void)data_len;

    if (req == NULL) {
        return;
    }

    if (result == 0) {
        const char *path = (const char *)data;
        printf("推荐歌单下载成功: page=%d path=%s\n",
               req->page_index,
               (path != NULL) ? path : "");
    } else {
        printf("推荐歌单下载失败: page=%d\n", req->page_index);
    }
}

/* 下载请求的操作回调结构体 */
static const KtvReqOps_t g_ops_download_recommend_sheet = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = recommend_sheet_on_complete
};

/**
 * 获取推荐歌单列表（异步下载）
 * @param page  页码（从0或1开始，由API定义决定）
 */
void fetch_4_17(int page)
{
    KtvRequest_t req;

    if (page < 0) {
        printf("fetch_recommend_song_sheet: invalid page %d\n", page);
        return;
    }

    memset(&req, 0, sizeof(req));

    if (recommend_sheet_build_url(page, req.url, sizeof(req.url)) != 0) {
        printf("推荐歌单URL生成失败: page=%d\n", page);
        return;
    }

    if (recommend_sheet_build_path(page, req.local_path, sizeof(req.local_path)) != 0) {
        printf("推荐歌单本地路径生成失败: page=%d\n", page);
        return;
    }

    req.ops = &g_ops_download_recommend_sheet;
    req.id = page;               /* 可用页码作为请求ID */
    req.page_index = page;
    req.user_data = NULL;

    if (Ktv_Ctrl_PostTask(&req) != 0) {
        printf("推荐歌单任务投递失败: page=%d\n", page);
    }
}