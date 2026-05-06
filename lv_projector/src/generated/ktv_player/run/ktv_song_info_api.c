#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "ktv.h"
#include "ktv_song_info_api.h"
#include "ktv_http_fetch_api.h"

#define KTV_SONG_INFO_URL_LEN (2048)

static int ktv_song_info_extract_string(cJSON *obj,
                                        const char *key,
                                        char *out,
                                        size_t out_size)
{
    cJSON *item;

    if (obj == NULL || key == NULL || out == NULL || out_size == 0U)
    {
        return 0;
    }

    item = cJSON_GetObjectItem(obj, key);
    if (item == NULL || !cJSON_IsString(item) || item->valuestring == NULL)
    {
        out[0] = '\0';
        return 0;
    }

    snprintf(out, out_size, "%s", item->valuestring);
    return 1;
}

int ktv_fetch_song_info_json(int song_id,
                             const char *user_token,
                             char *out_json,
                             size_t out_size)
{
    char url[KTV_SONG_INFO_URL_LEN];
    cJSON *root = NULL;
    cJSON *code_obj = NULL;
    cJSON *result_obj = NULL;
    cJSON *songinfo_obj = NULL;
    int ret = 0;

    if (song_id <= 0 || user_token == NULL || user_token[0] == '\0' || out_json == NULL || out_size < 2U)
    {
        printf("[KTV] [SONG_INFO] invalid args, song_id=%d\n", song_id);
        return 0;
    }

    out_json[0] = '\0';

    if (ktv_build_base_url(KTV_4_30_GET_SONG_INFO,
                           url,
                           sizeof(url),
                           song_id,
                           user_token,
                           326) != 0)
    {
        printf("[KTV] [SONG_INFO] build url failed, song_id=%d\n", song_id);
        return 0;
    }

    if (!ktv_http_get_to_memory(url, out_json, out_size, 15000))
    {
        printf("[KTV] [SONG_INFO] GET failed, song_id=%d\n", song_id);
        return 0;
    }

    root = cJSON_Parse(out_json);
    if (root == NULL)
    {
        printf("[KTV] [SONG_INFO] json parse failed, song_id=%d\n", song_id);
        goto EXIT;
    }

    code_obj = cJSON_GetObjectItem(root, "code");
    if (code_obj == NULL || !cJSON_IsNumber(code_obj) || code_obj->valueint != 0)
    {
        printf("[KTV] [SONG_INFO] invalid code, song_id=%d\n", song_id);
        goto EXIT;
    }

    result_obj = cJSON_GetObjectItem(root, "result");
    if (result_obj == NULL || !cJSON_IsObject(result_obj))
    {
        printf("[KTV] [SONG_INFO] missing result, song_id=%d\n", song_id);
        goto EXIT;
    }

    songinfo_obj = cJSON_GetObjectItem(result_obj, "songinfo");
    if (songinfo_obj == NULL || !cJSON_IsObject(songinfo_obj))
    {
        printf("[KTV] [SONG_INFO] missing songinfo, song_id=%d\n", song_id);
        goto EXIT;
    }

    printf("[KTV] [SONG_INFO] fetch json success, song_id=%d\n", song_id);
    ret = 1;

EXIT:
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return ret;
}

int ktv_song_info_parse_media(const char *json_str,
                              ktv_song_info_media_t *out_media)
{
    cJSON *root = NULL;
    cJSON *result_obj = NULL;
    cJSON *songinfo_obj = NULL;

    if (json_str == NULL || out_media == NULL)
    {
        return 0;
    }

    memset(out_media, 0, sizeof(*out_media));

    root = cJSON_Parse(json_str);
    if (root == NULL)
    {
        return 0;
    }

    result_obj = cJSON_GetObjectItem(root, "result");
    if (result_obj == NULL || !cJSON_IsObject(result_obj))
    {
        goto FAIL;
    }

    songinfo_obj = cJSON_GetObjectItem(result_obj, "songinfo");
    if (songinfo_obj == NULL || !cJSON_IsObject(songinfo_obj))
    {
        goto FAIL;
    }

    (void)ktv_song_info_extract_string(songinfo_obj, "mv_url", out_media->mv_url, sizeof(out_media->mv_url));
    if (out_media->mv_url[0] == '\0')
    {
        (void)ktv_song_info_extract_string(songinfo_obj, "bg_mov_new", out_media->mv_url, sizeof(out_media->mv_url));
    }
    if (out_media->mv_url[0] == '\0')
    {
        (void)ktv_song_info_extract_string(songinfo_obj, "bc_mov", out_media->mv_url, sizeof(out_media->mv_url));
    }

    (void)ktv_song_info_extract_string(songinfo_obj, "mp3", out_media->mp3_url, sizeof(out_media->mp3_url));
    if (out_media->mp3_url[0] == '\0')
    {
        (void)ktv_song_info_extract_string(songinfo_obj, "music", out_media->mp3_url, sizeof(out_media->mp3_url));
    }

    (void)ktv_song_info_extract_string(songinfo_obj, "zrc", out_media->subtitle_url, sizeof(out_media->subtitle_url));

    cJSON_Delete(root);
    return 1;

FAIL:
    cJSON_Delete(root);
    return 0;
}
