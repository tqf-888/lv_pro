#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

/*
 * 从 src 对象中安全取整型，取不到返回默认值
 */
static int json_get_int_def(cJSON *src, const char *key, int def)
{
    cJSON *item;

    if (src == NULL || key == NULL) {
        return def;
    }

    item = cJSON_GetObjectItemCaseSensitive(src, key);
    if (item != NULL && cJSON_IsNumber(item)) {
        return item->valueint;
    }

    return def;
}

/*
 * 从 src 对象中安全取字符串，取不到返回默认值
 */
static const char *json_get_str_def(cJSON *src, const char *key, const char *def)
{
    cJSON *item;

    if (src == NULL || key == NULL) {
        return def;
    }

    item = cJSON_GetObjectItemCaseSensitive(src, key);
    if (item != NULL && cJSON_IsString(item) && item->valuestring != NULL) {
        return item->valuestring;
    }

    return def;
}

/*
 * 顶层转换函数
 *
 * 保持外部接口不变：
 *   cJSON *transform_song_list(const char *json_str, const char *device_sn)
 *
 * 功能：
 *   从原始搜索结果 JSON 中提取 result.data[0]，
 *   直接生成 /song/order/add 所需的单对象请求体。
 */
cJSON *transform_song_list(const char *json_str, const char *device_sn, const char *ai_mv_url)
{
    cJSON *root = NULL;
    cJSON *result = NULL;
    cJSON *data = NULL;
    cJSON *first = NULL;
    cJSON *out = NULL;

    if (json_str == NULL || device_sn == NULL) {
        return NULL;
    }

    root = cJSON_Parse(json_str);
    if (root == NULL) {
        return NULL;
    }

    result = cJSON_GetObjectItemCaseSensitive(root, "result");
    if (result == NULL || !cJSON_IsObject(result)) {
        cJSON_Delete(root);
        return NULL;
    }

    data = cJSON_GetObjectItemCaseSensitive(result, "data");
    if (data == NULL || !cJSON_IsArray(data) || cJSON_GetArraySize(data) <= 0) {
        cJSON_Delete(root);
        return NULL;
    }

    first = cJSON_GetArrayItem(data, 0);
    if (first == NULL || !cJSON_IsObject(first)) {
        cJSON_Delete(root);
        return NULL;
    }

    out = cJSON_CreateObject();
    if (out == NULL) {
        cJSON_Delete(root);
        return NULL;
    }

    cJSON_AddStringToObject(out, "deviceSn",   device_sn);
    cJSON_AddStringToObject(out, "phone",       "16666666666");
    cJSON_AddNumberToObject(out, "songid",     json_get_int_def(first, "songid", 0));
    cJSON_AddStringToObject(out, "songname",   json_get_str_def(first, "songname", ""));
    cJSON_AddStringToObject(out, "artist",     json_get_str_def(first, "artist", ""));
    cJSON_AddStringToObject(out, "songIcon",   json_get_str_def(first, "song_icon", ""));
    cJSON_AddStringToObject(out, "artistIcon", json_get_str_def(first, "artist_icon", ""));
    cJSON_AddNumberToObject(out, "singCount",  json_get_int_def(first, "sing_count", 0));
    cJSON_AddNumberToObject(out, "isMp3",      json_get_int_def(first, "is_mp3", 0));
    cJSON_AddNumberToObject(out, "isMel",      json_get_int_def(first, "is_mel", 0));
    cJSON_AddNumberToObject(out, "isMelp",     json_get_int_def(first, "is_melp", 0));
    cJSON_AddNumberToObject(out, "isVip",      json_get_int_def(first, "is_vip", 0));
    cJSON_AddNumberToObject(out, "mvId",       json_get_int_def(first, "mv_id", 0));
    cJSON_AddNumberToObject(out, "orderType",  0);
    if(ai_mv_url)
        cJSON_AddStringToObject(out, "aiMv",  ai_mv_url);

    cJSON_Delete(root);
    return out;
}

// int test_json(void)
// {
//     const char *src_json =
//         "{\"result\":{\"data\":[{\"is_mv_melp\":1,\"is_mp3_chorus\":1,\"mv_id\":1004080,\"is_vip\":1,\"mv_chorus\":1,\"is_melp\":1,\"is_mel\":0,\"is_mp3\":1,\"is_mp3_preview\":1,\"songid\":4947032,\"songname\":\"苹果香\",\"artist\":\"狼戈\",\"song_icon\":\"http://aliimg.changba.com/cache/photo/songicon/source4~1627614195~3637.jpg\",\"sing_count\":101698,\"artist_icon\":\"http://aliimg.changba.com/cache/photo/artisticon/source4~39807~2.jpg\",\"is_hd_mv\":1},{\"is_mv_melp\":1,\"is_mp3_chorus\":1,\"mv_id\":1025642,\"is_vip\":0,\"mv_chorus\":1,\"is_melp\":1,\"is_mel\":0,\"is_mp3\":1,\"is_mp3_preview\":1,\"songid\":6494111,\"songname\":\"小美满\",\"artist\":\"周深\",\"song_icon\":\"http://aliimg.changba.com/cache/photo/songicon/source4~1707205222~1720.jpg\",\"sing_count\":81533,\"artist_icon\":\"https://songsheetpic-cdn.jzurl.cn/1600336371858_周深.jpg\",\"is_hd_mv\":1}],\"total_count\":10000},\"message\":\"\",\"code\":0}";

//     cJSON *out = transform_song_list(src_json, "16666666666");
//     if (out != NULL) {
//         char *str = cJSON_PrintUnformatted(out);
//         if (str != NULL) {
//             printf("%s\n", str);
//             free(str);
//         }
//         cJSON_Delete(out);
//     }

//     return 0;
// }
