#include "app_data_store.h"
#include "gen_queue.h"          /* 依赖底层的通用队列模块 */
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ktv.h"                /* 可能包含调试宏等 */

/* 内部上下文：使用数组维护多个队列句柄，每个类型对应一个独立的队列 */
static gen_queue_handle_t g_queues[DATA_TYPE_MAX] = {NULL};
static uint32_t g_packet_seq = 1;

/* ==========================================
 * 生命周期管理
 * ========================================== */

int app_data_store_init(void)
{
    int i;
    for (i = 0; i < DATA_TYPE_MAX; i++) {
        if (g_queues[i] == NULL) {
            g_queues[i] = gen_queue_create("app_data");
            if (g_queues[i] == NULL) {
                /* 创建失败回滚 */
                for (int j = 0; j < i; j++) {
                    if (g_queues[j]) {
                        gen_queue_destroy(g_queues[j]);
                        g_queues[j] = NULL;
                    }
                }
                return -1;
            }
        }
    }
    g_packet_seq = 1;
    return 0;
}

int app_data_store_deinit(void)
{
    int i;
    for (i = 0; i < DATA_TYPE_MAX; i++) {
        if (g_queues[i]) {
            gen_queue_destroy(g_queues[i]);
            g_queues[i] = NULL;
        }
    }
    return 0;
}

/* ==========================================
 * 核心操作：按类型分发
 * ========================================== */

int app_data_push_by_type(app_data_type_e type, const void *data, uint32_t len)
{
    if (type >= DATA_TYPE_MAX || data == NULL) {
        return -1;
    }
    if (g_queues[type] == NULL) {
        return -2; /* 未初始化 */
    }
    return gen_queue_push(g_queues[type], data, len);
}

int app_data_pop_by_type(app_data_type_e type, void *out_buffer, uint32_t buffer_len)
{
    if (type >= DATA_TYPE_MAX || out_buffer == NULL) {
        return -1;
    }
    if (g_queues[type] == NULL) {
        return -2;
    }
    return gen_queue_pop(g_queues[type], out_buffer, buffer_len, NULL);
}

int app_data_is_empty_by_type(app_data_type_e type)
{
    if (type >= DATA_TYPE_MAX || g_queues[type] == NULL) {
        return 1;
    }
    return gen_queue_is_empty(g_queues[type]);
}

int app_data_get_count_by_type(app_data_type_e type)
{
    if (type >= DATA_TYPE_MAX || g_queues[type] == NULL) {
        return 0;
    }
    return gen_queue_get_count(g_queues[type]);
}

/* ==========================================
 * 内部工具函数
 * ========================================== */

static void safe_copy(char *dst, size_t size, const char *src)
{
    if (dst && size > 0) {
        if (src) {
            snprintf(dst, size, "%s", src);
        } else {
            dst[0] = '\0';
        }
    }
}

/* ==========================================
 * 业务解析实现：歌曲列表
 * ========================================== */

static void fill_song_item(song_item_t *dst, cJSON *json_item)
{
    memset(dst, 0, sizeof(*dst));

    cJSON *songid = cJSON_GetObjectItem(json_item, "songid");
    cJSON *songname = cJSON_GetObjectItem(json_item, "songname");
    cJSON *mv_id = cJSON_GetObjectItem(json_item, "mv_id");
    cJSON *is_vip = cJSON_GetObjectItem(json_item, "is_vip");

    if (songid && cJSON_IsNumber(songid)) {
        dst->id = songid->valueint;
    }

    if (songname && cJSON_IsString(songname)) {
        safe_copy(dst->name, sizeof(dst->name), songname->valuestring);
    } else {
        safe_copy(dst->name, sizeof(dst->name), "未知");
    }

    /* 字段映射逻辑，保持不变 */
    if (is_vip && cJSON_IsNumber(is_vip) && is_vip->valueint) {
        safe_copy(dst->text_a, sizeof(dst->text_a), "0");
    } else if (mv_id && cJSON_IsNumber(mv_id) && mv_id->valueint) {
        safe_copy(dst->text_a, sizeof(dst->text_a), "1");
    }

    if (is_vip && cJSON_IsNumber(is_vip) && is_vip->valueint && 
        mv_id && cJSON_IsNumber(mv_id) && mv_id->valueint) {
        safe_copy(dst->text_b, sizeof(dst->text_b), "1");
    }

    safe_copy(dst->text_c, sizeof(dst->text_c), "2");
    safe_copy(dst->text_d, sizeof(dst->text_d), "3");
    safe_copy(dst->text_e, sizeof(dst->text_e), "4");
    safe_copy(dst->text_f, sizeof(dst->text_f), "5");
}

int app_data_parse_and_push_song_json(const char *json_str)
{
    cJSON *root = NULL;
    cJSON *result = NULL;
    cJSON *data = NULL;
    song_packet_t packet;
    int total_count = 0;
    int array_size = 0;
    int i = 0;

    if (json_str == NULL) return -1;
    if (g_queues[DATA_TYPE_SONG_LIST] == NULL) return -1;

    root = cJSON_Parse(json_str);
    if (root == NULL) return -1;

    result = cJSON_GetObjectItem(root, "result");
    if (result == NULL) {
        cJSON_Delete(root);
        return -1;
    }

    data = cJSON_GetObjectItem(result, "data");
    if (!cJSON_IsArray(data)) {
        cJSON_Delete(root);
        return -1;
    }

    memset(&packet, 0, sizeof(packet));
    packet.header.type = DATA_TYPE_SONG_LIST;
    packet.header.packet_id = g_packet_seq++;

    array_size = cJSON_GetArraySize(data);
    for (i = 0; i < array_size; i++) {
        cJSON *item = cJSON_GetArrayItem(data, i);
        if (item == NULL) continue;

        /* 包满了，先入库，再重置 */
        if (packet.header.item_count >= SONG_PACKET_MAX_ITEMS) {
            app_data_push_by_type(DATA_TYPE_SONG_LIST, &packet, sizeof(packet));
            memset(&packet, 0, sizeof(packet));
            packet.header.type = DATA_TYPE_SONG_LIST;
            packet.header.packet_id = g_packet_seq++;
        }

        fill_song_item(&packet.items[packet.header.item_count], item);
        packet.header.item_count++;
        total_count++;
    }

    /* 处理剩余数据 */
    if (packet.header.item_count > 0) {
        app_data_push_by_type(DATA_TYPE_SONG_LIST, &packet, sizeof(packet));
    }

    cJSON_Delete(root);
    return total_count;
}