#include <stdio.h>
#include <string.h>
#include "ktv_player_ui.h"
#include "ktv_cloud_order_api.h"

int ktv_business_play_current_song(const char *device_sn,
                                   const char *user_token,
                                   int has_audio)
{
    ktv_cloud_order_resolved_t resolved;
    char current_json[8192];
    char songinfo_json[16384];

    memset(&resolved, 0, sizeof(resolved));
    memset(current_json, 0, sizeof(current_json));
    memset(songinfo_json, 0, sizeof(songinfo_json));

    if (!ktv_cloud_order_resolve_first_play_param(device_sn,
                                                  user_token,
                                                  has_audio,
                                                  0,
                                                  NULL,
                                                  &resolved,
                                                  current_json,
                                                  sizeof(current_json),
                                                  songinfo_json,
                                                  sizeof(songinfo_json)))
    {
        printf("[KTV] [BUSINESS_DEMO] resolve current play param failed\n");
        return 0;
    }

    if (resolved.subtitle_path[0] != '\0')
    {
        resolved.play_param.subtitle_path = resolved.subtitle_path;
    }
    else
    {
        resolved.play_param.subtitle_path = NULL;
    }

    printf("[KTV] [BUSINESS_DEMO] play current, song_id=%d, song=%s, artist=%s\n",
           resolved.order.song_id,
           resolved.order.song_name,
           resolved.order.artist);

    return (ktv_player_ui_play(&resolved.play_param) == 0) ? 1 : 0;
}
