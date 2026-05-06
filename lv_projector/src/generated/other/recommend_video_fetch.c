#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "ktv_ctrl.h"
#include "ktv.h"
#include "cJSON.h"
#include "recommend_video_fetch.h"

/* LVGL 异步调用所需头文件（根据实际项目路径调整） */
#include "lvgl/lvgl.h"

/* 声明外部 UI 刷新函数（定义在 lv_wifi_ui.c 或其他 UI 文件中） */
extern void lv_page_refresh_video_list(void);

/* ========== 全局存储 ========== */
#define MAX_VIDEOS 128
static VideoInfo_t g_video_list[MAX_VIDEOS];
static int g_video_count = 0;
static uint32_t g_current_epoch = 0;

/* 异步刷新回调（在 LVGL 主线程中执行） */
static void async_refresh_ui(void *data) {
    (void)data;
    lv_page_refresh_video_list();
}

int get_recommend_video_list(VideoInfo_t **videos) {
    if (videos) *videos = g_video_list;
    return g_video_count;
}

static void update_video_list_from_json(const char *json_str, uint32_t epoch) {
    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        printf("JSON 解析失败\n");
        return;
    }
    if (!cJSON_IsArray(root)) {
        printf("JSON 根节点不是数组\n");
        cJSON_Delete(root);
        return;
    }
    if (epoch != g_current_epoch) {
        printf("忽略旧数据: req_epoch=%u, cur_epoch=%u\n", epoch, g_current_epoch);
        cJSON_Delete(root);
        return;
    }

    int count = cJSON_GetArraySize(root);
    if (count > MAX_VIDEOS) count = MAX_VIDEOS;

    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(root, i);
        if (!item) continue;

        cJSON *id_obj = cJSON_GetObjectItem(item, "id");
        cJSON *title_obj = cJSON_GetObjectItem(item, "title");
        cJSON *url_obj = cJSON_GetObjectItem(item, "play_url");

        if (id_obj && cJSON_IsNumber(id_obj) &&
            title_obj && cJSON_IsString(title_obj) &&
            url_obj && cJSON_IsString(url_obj)) {

            g_video_list[i].id = id_obj->valueint;
            strncpy(g_video_list[i].title, title_obj->valuestring, sizeof(g_video_list[i].title) - 1);
            strncpy(g_video_list[i].play_url, url_obj->valuestring, sizeof(g_video_list[i].play_url) - 1);
            g_video_list[i].title[sizeof(g_video_list[i].title) - 1] = '\0';
            g_video_list[i].play_url[sizeof(g_video_list[i].play_url) - 1] = '\0';
        }
    }
    g_video_count = count;
    printf("成功解析 %d 个视频，epoch=%u\n", g_video_count, epoch);
    cJSON_Delete(root);

    /* 数据更新完成，异步刷新 UI（保证在主线程中执行） */
    lv_async_call(async_refresh_ui, NULL);
}

/* 构建临时文件路径 */
static int build_temp_path(uint32_t epoch, char *buf, size_t size) {
    int n = snprintf(buf, size, "/tmp/videos_%u.json", epoch);
    return (n >= 0 && (size_t)n < size) ? 0 : -1;
}

/* 固定 URL */
static int build_url(char *buf, size_t size) {
    const char *url = "http://media.djyos.com/videos.json";
    int n = snprintf(buf, size, "%s", url);
    return (n >= 0 && (size_t)n < size) ? 0 : -1;
}

/* 下载完成回调 */
static void on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len) {
    (void)data_len;
    if (!req) return;

    uint32_t epoch = (uint32_t)(uintptr_t)req->user_data;
    const char *path = (const char *)data;

    if (result == 0 && path) {
        FILE *fp = fopen(path, "rb");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            char *json = malloc(size + 1);
            if (json) {
                fread(json, 1, size, fp);
                json[size] = '\0';
                fclose(fp);
                update_video_list_from_json(json, epoch);
                free(json);
            } else {
                fclose(fp);
                printf("内存不足\n");
            }
            unlink(path);
        } else {
            printf("无法打开临时文件 %s\n", path);
        }
    } else {
        printf("下载失败，epoch=%u\n", epoch);
    }
}

static const KtvReqOps_t g_ops = {
    .type = KTV_REQ_DOWNLOAD_FILE,
    .on_complete = on_complete
};


//老王说重做mv，所以废弃
void fetch_video_list(void) {
    // KtvRequest_t req;
    // uint32_t epoch;

    // memset(&req, 0, sizeof(req));
    // epoch = (uint32_t)time(NULL);
    // g_current_epoch = epoch;

    // if (build_url(req.url, sizeof(req.url)) != 0) {
    //     printf("URL 生成失败\n");
    //     return;
    // }
    // if (build_temp_path(epoch, req.local_path, sizeof(req.local_path)) != 0) {
    //     printf("临时路径生成失败\n");
    //     return;
    // }

    // req.ops = &g_ops;
    // req.id = 0;
    // req.page_index = 0;
    // req.user_data = (void *)(uintptr_t)epoch;

    // if (Ktv_Ctrl_PostTask(&req) != 0) {
    //     printf("任务投递失败\n");
    // }
}

/* ========== 最近点击的 URL（仅保存一个） ========== */
static char g_last_clicked_url[256] = {0};
static char g_last_artist[256] = {0};
static char g_last_song_namel[256] = {0};
static int order_num = 0;

/**
 * @brief 记录最近点击的 URL（每次覆盖之前的）
 * @param url 要记录的 URL，不能为 NULL
 */
void record_last_clicked_url(const char *url) {
    if (url && *url) {
        strncpy(g_last_clicked_url, url, sizeof(g_last_clicked_url) - 1);
        g_last_clicked_url[sizeof(g_last_clicked_url) - 1] = '\0';
        printf("记录最近点击 URL: %s\n", g_last_clicked_url);
    }
}

/**
 * @brief 获取最近点击的 URL
 * @return URL 字符串指针，如果从未点击则返回 NULL
 */
const char* get_last_clicked_url(void) {
    if (g_last_clicked_url[0] == '\0') {
        return "http://media.djyos.com/中国人.mp4";
    }
    return g_last_clicked_url;
}

void record_last_artist(const char *artist) {
    if (artist && *artist) {
        strncpy(g_last_artist, artist, sizeof(g_last_artist) - 1);
        g_last_artist[sizeof(g_last_artist) - 1] = '\0';
    }
}

const char* get_last_artist(void) {
    return (g_last_artist[0] == '\0') ? NULL : g_last_artist;
}

void record_last_song_name(const char *song_name) {
    if (song_name && *song_name) {
        strncpy(g_last_song_namel, song_name, sizeof(g_last_song_namel) - 1);
        g_last_song_namel[sizeof(g_last_song_namel) - 1] = '\0';
    }
}

const char* get_last_song_name(void) {
    return (g_last_song_namel[0] == '\0') ? NULL : g_last_song_namel;
}

void set_order_num(int num) 
{ 
    if(!num)
        order_num = num; 
    else
        order_num++;
}
int get_order_num(void) { return order_num; }