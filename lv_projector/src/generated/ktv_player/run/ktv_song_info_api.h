#ifndef __KTV_SONG_INFO_API_H__
#define __KTV_SONG_INFO_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

typedef struct
{
    char mv_url[1024];
    char mp3_url[1024];
    char subtitle_url[1024];
} ktv_song_info_media_t;

/* 按 song_id 获取完整歌曲 JSON 原文 */
int ktv_fetch_song_info_json(int song_id,
                             const char *user_token,
                             char *out_json,
                             size_t out_size);

/* 从歌曲 JSON 原文里解析 mv/mp3/zrc */
int ktv_song_info_parse_media(const char *json_str,
                              ktv_song_info_media_t *out_media);

#ifdef __cplusplus
}
#endif

#endif
