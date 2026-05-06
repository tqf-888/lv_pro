#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "page_manager.h"
#include "ktv_ctrl.h"
#include "ktv.h"
#include "cJSON.h"
#include "db_list_pro_worker.h"


static char g_song_name[100];

#include <stdio.h>
#include "http_post_api.h"

static void test_song_order_add_cb(http_task_t *task, void *user_data)
{
    (void)user_data;

    printf("========== http_post callback ==========\n");
    printf("err_code      = %d (%s)\n", task->err_code, http_err_str(task->err_code));
    printf("http_status   = %ld\n", task->http_status);
    printf("response_size = %u\n", (unsigned)task->response_size);

    if (task->response_data != NULL) {
        printf("response_data = %s\n", task->response_data);
    } else {
        printf("response_data = <null>\n");
    }

    printf("========================================\n");
}


static int song_build_batch_url(char *singer_name, char *song_name, char *buf, size_t size)
{
    return ktv_build_base_url(KTV_4_2_SEARCH_SONG,
                              buf,
                              size,
                              song_name,
                              singer_name,
                              1,
                              "",
                              IGNORE_NUM,
                              IGNORE_NUM,
                              0,
                              0,
                              "",
                              0,
                              1);
}

static void song_batch_on_complete(KtvRequest_t *req, int result, const void *data, size_t data_len)
{
    (void)req;

    if (result != 0) {
        printf("[song_batch] fetch memory failed\n");
        return;
    }

    if (data == NULL || data_len == 0U) {
        printf("[song_batch] response empty\n");
        return;
    }

    printf("[song_batch] ===== memory begin =====\n");
    // printf("%.*s\n", (int)data_len, (const char *)data);
    extern cJSON *transform_song_list(const char *json_str, const char *device_sn, const char *ai_mv_url);
    
    cJSON *out = transform_song_list(data, "16666666666", (req->local_path[0] == '\0') ? NULL : req->local_path);
    if (out) {
        char *str = cJSON_Print(out);
        http_err_t err = http_post("https://tuoge.djyos.com/sys/song/favorite",
                            str,
                            "application/json",
                            test_song_order_add_cb,
                            NULL);
        free(str);
        cJSON_Delete(out);
    }

    printf("[song_batch] ===== memory end =====\n");
}

static const KtvReqOps_t g_ops_download_song_batch = {
    .type = KTV_REQ_FETCH_MEMORY,
    .on_complete = song_batch_on_complete
};

void v_add_love_song(char *singer_name, char *song_name, char *ai_mv_url)
{
    KtvRequest_t req;

    if (singer_name == NULL || song_name == NULL) {
        return;
    }

    snprintf(g_song_name, sizeof(g_song_name), "%s", song_name);

    memset(&req, 0, sizeof(req));

    if (song_build_batch_url(singer_name, song_name, req.url, sizeof(req.url)) != 0) {
        printf("[song_batch] build url failed\n");
        return;
    }

    req.ops = &g_ops_download_song_batch;
    if(ai_mv_url)
    {
        strncpy(req.local_path, ai_mv_url, sizeof(req.local_path) - 1);
        req.local_path[sizeof(req.local_path) - 1] = '\0';
    }


    if (Ktv_Ctrl_PostTask(&req) != 0) {
        printf("[song_batch] Ktv_Ctrl_PostTask failed\n");
    }
}

