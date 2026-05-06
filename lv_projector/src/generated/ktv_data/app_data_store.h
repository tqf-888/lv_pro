#ifndef APP_DATA_STORE_H
#define APP_DATA_STORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* 数据类型枚举（仅保留歌曲列表） */
typedef enum {
    DATA_TYPE_SONG_LIST = 0,   // 歌曲列表柜
    DATA_TYPE_MAX               // 用于定义队列数组大小
} app_data_type_e;

/* 通用数据包头 */
typedef struct {
    app_data_type_e type;       /* 数据种类标识 */
    uint32_t packet_id;         /* 包序号 (调试用) */
    uint32_t item_count;        /* 当前包内的条目数量 */
} app_data_header_t;

/* 歌曲相关定义 */
#define SONG_NAME_MAX_LEN   64
#define SONG_TEXT_MAX_LEN   16
#define SONG_PACKET_MAX_ITEMS 50

typedef struct {
    int32_t id;
    char name[SONG_NAME_MAX_LEN];
    char text_a[SONG_TEXT_MAX_LEN];
    char text_b[SONG_TEXT_MAX_LEN];
    char text_c[SONG_TEXT_MAX_LEN];
    char text_d[SONG_TEXT_MAX_LEN];
    char text_e[SONG_TEXT_MAX_LEN];
    char text_f[SONG_TEXT_MAX_LEN];
} song_item_t;

typedef struct {
    app_data_header_t header;   /* 必须放在第一位 */
    song_item_t items[SONG_PACKET_MAX_ITEMS];
} song_packet_t;

/* 生命周期管理 */
int app_data_store_init(void);
int app_data_store_deinit(void);

/* 核心操作：按类型存取（仅歌曲列表有效） */
int app_data_push_by_type(app_data_type_e type, const void *data, uint32_t len);
int app_data_pop_by_type(app_data_type_e type, void *out_buffer, uint32_t buffer_len);

/* 状态查询：按类型（仅歌曲列表有效） */
int app_data_is_empty_by_type(app_data_type_e type);
int app_data_get_count_by_type(app_data_type_e type);

/* 便捷接口：一键解析并入库歌曲列表JSON */
int app_data_parse_and_push_song_json(const char *json_str);

#ifdef __cplusplus
}
#endif

#endif /* APP_DATA_STORE_H */