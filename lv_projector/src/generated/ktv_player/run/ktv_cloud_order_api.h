#ifndef __KTV_CLOUD_ORDER_API_H__
#define __KTV_CLOUD_ORDER_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include "ktv_player_ui.h"
#include "ktv_song_info_api.h"

#define GET_CURRENT_PLAY_SONG_URL "https://tuoge.djyos.com/song/order/current?deviceSn=%s"
typedef struct
{
    int id;
    char device_sn[64];
    int song_id;
    char song_name[128];
    char artist[128];
    char ai_mv[1024];
    char song_icon[1024];
    char artist_icon[1024];
    int is_mp3;
} ktv_cloud_order_item_t;

typedef struct
{
    ktv_cloud_order_item_t order;
    ktv_song_info_media_t song_info;
    ktv_player_media_param_t play_param;
    char subtitle_url[1024];
    char subtitle_path[1024];
} ktv_cloud_order_resolved_t;

int ktv_cloud_order_fetch_current_json(const char *device_sn,
                                       char *out_json,
                                       size_t out_size);

int ktv_cloud_order_parse_first_item(const char *json_str,
                                     ktv_cloud_order_item_t *out_item);

int ktv_cloud_order_resolve_first_play_param(const char *device_sn,
                                             const char *user_token,
                                             int default_has_audio,
                                             int force_song_id,
                                             const char *force_mv_url,
                                             ktv_cloud_order_resolved_t *out_resolved,
                                             char *out_current_json,
                                             size_t current_json_size,
                                             char *out_songinfo_json,
                                             size_t songinfo_json_size);

int ktv_cloud_order_resolve_first_play_param_auto(const char *device_sn,
                                                  const char *user_token,
                                                  int force_song_id,
                                                  const char *force_mv_url,
                                                  ktv_cloud_order_resolved_t *out_resolved,
                                                  char *out_current_json,
                                                  size_t current_json_size,
                                                  char *out_songinfo_json,
                                                  size_t songinfo_json_size);

#ifdef __cplusplus
}
#endif

#endif
