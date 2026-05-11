#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "db_list_pro_thread.h"

// 把 Topic 定义拿过来用于比对（或者直接字符串比对）
#define TOPIC_SUB_1 "cloud/maku-boot/16666666666"
#define TOPIC_SUB_2 "device/control/16666666666"

typedef struct {
    char action[32];
    char song_id[32];
    char device_sn[32];
    int play_order;
} Payload;

void biz_parse(const char* topic, const char* json_str) {
    if (!json_str || !topic) return;
    cJSON *root = cJSON_Parse(json_str);
    if (!root) return;

    cJSON *cmd = cJSON_GetObjectItem(root, "cmd");
    cJSON *p_json = cJSON_GetObjectItem(root, "payload");
    if (!cmd || !cJSON_IsString(cmd) || !p_json) {
        cJSON_Delete(root);
        return;
    }

    Payload p = {0};
    cJSON *item;
    if ((item = cJSON_GetObjectItem(p_json, "action")) && cJSON_IsString(item)) strncpy(p.action, item->valuestring, sizeof(p.action) - 1);
    if ((item = cJSON_GetObjectItem(p_json, "deviceSn")) && cJSON_IsString(item)) strncpy(p.device_sn, item->valuestring, sizeof(p.device_sn) - 1);
    if ((item = cJSON_GetObjectItem(p_json, "songId"))) {
        if (cJSON_IsString(item)) strncpy(p.song_id, item->valuestring, sizeof(p.song_id) - 1);
        else if (cJSON_IsNumber(item)) snprintf(p.song_id, sizeof(p.song_id), "%d", item->valueint);
    }
    if ((item = cJSON_GetObjectItem(p_json, "playOrder")) && cJSON_IsNumber(item)) p.play_order = item->valueint;

    // ================= 业务分发（按 Topic 隔离） =================
    
    // 来源 1：云端下发的歌单同步/更新消息
    if (strcmp(topic, TOPIC_SUB_1) == 0) {
        if (strcmp(cmd->valuestring, "songlistUpdate") == 0) {
            if (strcmp(p.action, "add") == 0) {
                printf("[业务-歌单] 新增歌单 - 歌曲:%s\n", p.song_id);
                /* todo: 插入歌单逻辑 */
            } else if (strcmp(p.action, "control") == 0) {
                printf("[业务-歌单] 云端状态同步\n");
                /* todo: 处理云端下发的当前歌曲状态同步 */
            }
        }
    }
    // 来源 2：设备控制指令
    else if (strcmp(topic, TOPIC_SUB_2) == 0) {
        if (strcmp(cmd->valuestring, "deviceControl") == 0) {
            if (strcmp(p.action, "play") == 0 || strcmp(p.action, "continue") == 0) {
                printf("[业务-控制] 播放 - 歌曲:%s\n", p.song_id);
                db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PLAY, 0, 0, NULL, NULL, NULL);
            } else if (strcmp(p.action, "pause") == 0) {
                printf("[业务-控制] 暂停 - 歌曲:%s\n", p.song_id);
                db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
            } else if (strcmp(p.action, "next") == 0) {
                printf("[业务-控制] 下一首 - 歌曲:%s\n", p.song_id);
                db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_NEXT, subpage_get(), 0, NULL, NULL, NULL);
            } else if (strcmp(p.action, "replay") == 0) {
                printf("[业务-控制] 重播 - 歌曲:%s\n", p.song_id);
                db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_REPLAY, subpage_get(), 0, NULL, NULL, NULL);
            }
        }
    }

    // ==============================================================
    cJSON_Delete(root);
}
