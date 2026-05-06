#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "ktv_cloud_order_api.h"
#include "ktv_http_fetch_api.h"
#include "ktv_subtitle_fetch_api.h"
#include "karaoke_demo.h"

#define KTV_CLOUD_ORDER_URL_LEN (1024)

static int ktv_cloud_order_extract_string(cJSON *obj,
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

int ktv_cloud_order_fetch_current_json(const char *device_sn,
                                       char *out_json,
                                       size_t out_size)
{
    char url[KTV_CLOUD_ORDER_URL_LEN];

    if (device_sn == NULL || device_sn[0] == '\0' || out_json == NULL || out_size < 2U)
    {
        return 0;
    }

    snprintf(url,
             sizeof(url),
             GET_CURRENT_PLAY_SONG_URL,
             device_sn);
             printf("%s",url);

    return ktv_http_get_to_memory(url, out_json, out_size, 15000);
}

static int ktv_cloud_order_parse_item_object(cJSON *item_obj,
                                            ktv_cloud_order_item_t *out_item)
{
    cJSON *songid_obj = NULL;
    cJSON *id_obj = NULL;
    cJSON *ismp3_obj = NULL;

    if (item_obj == NULL || out_item == NULL || !cJSON_IsObject(item_obj))
    {
        return 0;
    }

    memset(out_item, 0, sizeof(*out_item));

    id_obj = cJSON_GetObjectItem(item_obj, "id");
    if (id_obj != NULL && cJSON_IsNumber(id_obj))
    {
        out_item->id = id_obj->valueint;
    }

    songid_obj = cJSON_GetObjectItem(item_obj, "songid");
    if (songid_obj == NULL || !cJSON_IsNumber(songid_obj))
    {
        return 0;
    }
    out_item->song_id = songid_obj->valueint;

    ismp3_obj = cJSON_GetObjectItem(item_obj, "isMp3");
    if (ismp3_obj != NULL && cJSON_IsNumber(ismp3_obj))
    {
        out_item->is_mp3 = ismp3_obj->valueint;
    }

    (void)ktv_cloud_order_extract_string(item_obj, "deviceSn", out_item->device_sn, sizeof(out_item->device_sn));
    (void)ktv_cloud_order_extract_string(item_obj, "songname", out_item->song_name, sizeof(out_item->song_name));
    (void)ktv_cloud_order_extract_string(item_obj, "artist", out_item->artist, sizeof(out_item->artist));
    (void)ktv_cloud_order_extract_string(item_obj, "aiMv", out_item->ai_mv, sizeof(out_item->ai_mv));
    (void)ktv_cloud_order_extract_string(item_obj, "songIcon", out_item->song_icon, sizeof(out_item->song_icon));
    (void)ktv_cloud_order_extract_string(item_obj, "artistIcon", out_item->artist_icon, sizeof(out_item->artist_icon));

    return 1;
}

static int ktv_cloud_order_parse_selected_item(const char *json_str,
                                               int require_ai_mv,
                                               ktv_cloud_order_item_t *out_item,
                                               int *out_selected_index)
{
    cJSON *root = NULL;
    cJSON *code_obj = NULL;
    cJSON *data_obj = NULL;
    cJSON *item_obj = NULL;
    int count;
    int i;
    ktv_cloud_order_item_t temp_item;

    if (json_str == NULL || out_item == NULL)
    {
        return 0;
    }

    memset(out_item, 0, sizeof(*out_item));
    if (out_selected_index != NULL)
    {
        *out_selected_index = -1;
    }

    root = cJSON_Parse(json_str);
    if (root == NULL)
    {
        return 0;
    }

    code_obj = cJSON_GetObjectItem(root, "code");
    if (code_obj == NULL || !cJSON_IsNumber(code_obj) || code_obj->valueint != 0)
    {
        goto FAIL;
    }

    data_obj = cJSON_GetObjectItem(root, "data");
    if (data_obj == NULL)
    {
        goto FAIL;
    }

    if (cJSON_IsObject(data_obj))
    {
        if (ktv_cloud_order_parse_item_object(data_obj, &temp_item))
        {
            if (require_ai_mv == 0 || temp_item.ai_mv[0] != '\0')
            {
                *out_item = temp_item;
                if (out_selected_index != NULL)
                {
                    *out_selected_index = 0;
                }

                cJSON_Delete(root);
                return 1;
            }
        }
    }
    else if (cJSON_IsArray(data_obj) && cJSON_GetArraySize(data_obj) > 0)
    {
        count = cJSON_GetArraySize(data_obj);
        for (i = 0; i < count; ++i)
        {
            item_obj = cJSON_GetArrayItem(data_obj, i);
            if (!ktv_cloud_order_parse_item_object(item_obj, &temp_item))
            {
                continue;
            }

            if (require_ai_mv != 0 && temp_item.ai_mv[0] == '\0')
            {
                continue;
            }

            *out_item = temp_item;
            if (out_selected_index != NULL)
            {
                *out_selected_index = i;
            }

            cJSON_Delete(root);
            return 1;
        }
    }

FAIL:
    cJSON_Delete(root);
    return 0;
}

int ktv_cloud_order_parse_first_item(const char *json_str,
                                     ktv_cloud_order_item_t *out_item)
{
    return ktv_cloud_order_parse_selected_item(json_str, 0, out_item, NULL);
}

int ktv_cloud_order_resolve_first_play_param(const char *device_sn,
                                             const char *user_token,
                                             int default_has_audio,
                                             int force_song_id,
                                             const char *force_mv_url,
                                             ktv_cloud_order_resolved_t *out_resolved,
                                             char *out_current_json,
                                             size_t current_json_size,
                                             char *out_songinfo_json,
                                             size_t songinfo_json_size)
{
    const char *video_url = NULL;
    const char *audio_url = NULL;
    int selected_index = 0;
    int require_ai_mv = 0;
    int force_mv_enabled = 0;
    int force_song_enabled = 0;
    int original_song_id = 0;
    int effective_song_id = 0;
    int enable_subtitle = (default_has_audio == 0) ? 1 : 0;
    if(default_has_audio)
    karaoke_demo_set_positions(LV_ALIGN_TOP_LEFT, 1800, 1800, LV_ALIGN_BOTTOM_RIGHT, 1800, 1800);
    if (device_sn == NULL || user_token == NULL || out_resolved == NULL ||
        out_current_json == NULL || current_json_size < 2U ||
        out_songinfo_json == NULL || songinfo_json_size < 2U)
    {
        return 0;
    }

    memset(out_resolved, 0, sizeof(*out_resolved));
    out_current_json[0] = '\0';
    out_songinfo_json[0] = '\0';

    force_mv_enabled = (force_mv_url != NULL && force_mv_url[0] != '\0') ? 1 : 0;
    force_song_enabled = (force_song_id > 0) ? 1 : 0;

    if (!ktv_cloud_order_fetch_current_json(device_sn, out_current_json, current_json_size))
    {
        printf("[KTV] [CLOUD_ORDER] fetch current json failed, device_sn=%s\n", device_sn);
        return 0;
    }

    /*
     * 规则：
     * 1) 没有强覆盖时，保持原逻辑：
     *    - has_audio=0 时，向后扫描第一个有 aiMv 的条目
     *    - has_audio=1 时，取第一首
     *
     * 2) 只要有强覆盖 song_id 或强覆盖 mv：
     *    - 直接取第一首 data[0]
     *    - 不再要求 aiMv
     *    - 后续 song_id / mv 再按覆盖规则强制替换
     */
    if (force_mv_enabled || force_song_enabled)
    {
        require_ai_mv = 0;
    }
    else
    {
        require_ai_mv = (default_has_audio == 0) ? 1 : 0;
    }

    if (!ktv_cloud_order_parse_selected_item(out_current_json,
                                             require_ai_mv,
                                             &out_resolved->order,
                                             &selected_index))
    {
        if (require_ai_mv != 0)
        {
            printf("[KTV] [CLOUD_ORDER] no playable item with aiMv found, device_sn=%s\n", device_sn);
        }
        else
        {
            printf("[KTV] [CLOUD_ORDER] parse first item failed, device_sn=%s\n", device_sn);
        }
     
    }

    original_song_id = out_resolved->order.song_id;
    effective_song_id = original_song_id;

    if (force_song_enabled)
    {
        effective_song_id = force_song_id;
        out_resolved->order.song_id = force_song_id;
    }

    if (force_song_enabled || force_mv_enabled)
    {
        printf("[KTV] [CLOUD_ORDER] override enabled, selected_index=%d, original_song_id=%d, effective_song_id=%d, force_mv=%s\n",
               selected_index,
               original_song_id,
               effective_song_id,
               force_mv_enabled ? force_mv_url : "");
    }
    else if (require_ai_mv != 0 && selected_index > 0)
    {
        printf("[KTV] [CLOUD_ORDER] default_has_audio=0, skip first %d item(s), selected index=%d, song_id=%d, aiMv=%s\n",
               selected_index,
               selected_index,
               out_resolved->order.song_id,
               out_resolved->order.ai_mv);
    }

    if (!ktv_fetch_song_info_json(effective_song_id,
                                  user_token,
                                  out_songinfo_json,
                                  songinfo_json_size))
    {
        printf("[KTV] [CLOUD_ORDER] fetch song info json failed, effective_song_id=%d, original_song_id=%d\n",
               effective_song_id,
               original_song_id);
        return 0;
    }

    if (!ktv_song_info_parse_media(out_songinfo_json, &out_resolved->song_info))
    {
        printf("[KTV] [CLOUD_ORDER] parse song info media failed, effective_song_id=%d, original_song_id=%d\n",
               effective_song_id,
               original_song_id);
        return 0;
    }

    out_resolved->play_param.has_audio = (default_has_audio != 0) ? 1 : 0;

    /*
     * 强覆盖 MV：
     * 只要 force_mv_url 非空，最终视频地址无条件使用它。
     *
     * 强覆盖 song_id：
     * 第二个 JSON 永远按 effective_song_id 去取，
     * 所以 mp3 / zrc / mv_url 都来自被覆盖后的 song_id。
     *
     * 这样就允许你故意制造音画错配：
     * - 视频：force_mv_url
     * - 音频/字幕：force_song_id 对应的资源
     */
    if (force_mv_enabled)
    {
        video_url = force_mv_url;
    }
    else if (out_resolved->play_param.has_audio == 1)
    {
        video_url = out_resolved->song_info.mv_url;
    }
    else
    {
        video_url = out_resolved->order.ai_mv;
    }

    if (out_resolved->play_param.has_audio == 1)
    {
        audio_url = NULL;
    }
    else
    {
        audio_url = out_resolved->song_info.mp3_url;
    }

    snprintf(out_resolved->subtitle_url,
             sizeof(out_resolved->subtitle_url),
             "%s",
             out_resolved->song_info.subtitle_url);

    out_resolved->subtitle_path[0] = '\0';
    if (enable_subtitle && out_resolved->subtitle_url[0] != '\0')
    {
        if (!ktv_fetch_subtitle_to_local(effective_song_id,
                                         out_resolved->subtitle_url,
                                         out_resolved->subtitle_path,
                                         sizeof(out_resolved->subtitle_path)))
        {
            printf("[KTV] [CLOUD_ORDER] subtitle fetch failed, effective_song_id=%d, zrc=%s\n",
                   effective_song_id,
                   out_resolved->subtitle_url);
            out_resolved->subtitle_path[0] = '\0';
        }
        else if (out_resolved->subtitle_path[0] != '\0')
        {
            printf("[KTV] [CLOUD_ORDER] subtitle ready, effective_song_id=%d, local=%s\n",
                   effective_song_id,
                   out_resolved->subtitle_path);
        }
    }

    out_resolved->play_param.mp4_url =
        (video_url != NULL && video_url[0] != '\0') ? video_url : NULL;
    out_resolved->play_param.mp3_url =
        (audio_url != NULL && audio_url[0] != '\0') ? audio_url : NULL;
    out_resolved->play_param.subtitle_path =
        (out_resolved->subtitle_path[0] != '\0') ? out_resolved->subtitle_path : NULL;

    if (out_resolved->play_param.has_audio == 1)
    {
        if (out_resolved->play_param.mp4_url == NULL)
        {
            printf("[KTV] [CLOUD_ORDER] resolve failed: has_audio=1 requires mp4, effective_song_id=%d\n",
                   effective_song_id);
            return 0;
        }
    }
    else
    {
        if (out_resolved->play_param.mp4_url == NULL)
        {
            printf("[KTV] [CLOUD_ORDER] resolve failed: has_audio=0 requires mp4, effective_song_id=%d\n",
                   effective_song_id);
            return 0;
        }

        if (out_resolved->play_param.mp3_url == NULL)
        {
            printf("[KTV] [CLOUD_ORDER] resolve failed: has_audio=0 requires mp3, effective_song_id=%d\n",
                   effective_song_id);
            return 0;
        }
    }
    return 1;
}

int ktv_cloud_order_resolve_first_play_param_auto(const char *device_sn,
                                                  const char *user_token,
                                                  int force_song_id,
                                                  const char *force_mv_url,
                                                  ktv_cloud_order_resolved_t *out_resolved,
                                                  char *out_current_json,
                                                  size_t current_json_size,
                                                  char *out_songinfo_json,
                                                  size_t songinfo_json_size)
{
    const char *video_url = NULL;
    const char *audio_url = NULL;
    int selected_index = 0;
    int force_mv_enabled = 0;
    int force_song_enabled = 0;
    int original_song_id = 0;
    int effective_song_id = 0;
    int use_ai_mv = 0;
    int enable_subtitle = 0;

    if (device_sn == NULL || user_token == NULL || out_resolved == NULL ||
        out_current_json == NULL || current_json_size < 2U ||
        out_songinfo_json == NULL || songinfo_json_size < 2U)
    {
        return 0;
    }

    memset(out_resolved, 0, sizeof(*out_resolved));
    out_current_json[0] = '\0';
    out_songinfo_json[0] = '\0';

    force_mv_enabled = (force_mv_url != NULL && force_mv_url[0] != '\0') ? 1 : 0;
    force_song_enabled = (force_song_id > 0) ? 1 : 0;

    if (!ktv_cloud_order_fetch_current_json(device_sn, out_current_json, current_json_size))
    {
        printf("[KTV] [CLOUD_ORDER_AUTO] fetch current json failed, device_sn=%s\n", device_sn);
        return 0;
    }

    if (!ktv_cloud_order_parse_selected_item(out_current_json,
                                             0,
                                             &out_resolved->order,
                                             &selected_index))
    {
        printf("[KTV] [CLOUD_ORDER_AUTO] parse first item failed, device_sn=%s\n", device_sn);
        return 0;
    }

    original_song_id = out_resolved->order.song_id;
    effective_song_id = original_song_id;

    if (force_song_enabled)
    {
        effective_song_id = force_song_id;
        out_resolved->order.song_id = force_song_id;
    }

    if (!ktv_fetch_song_info_json(effective_song_id,
                                  user_token,
                                  out_songinfo_json,
                                  songinfo_json_size))
    {
        printf("[KTV] [CLOUD_ORDER_AUTO] fetch song info json failed, effective_song_id=%d, original_song_id=%d\n",
               effective_song_id,
               original_song_id);
        return 0;
    }

    if (!ktv_song_info_parse_media(out_songinfo_json, &out_resolved->song_info))
    {
        printf("[KTV] [CLOUD_ORDER_AUTO] parse song info media failed, effective_song_id=%d, original_song_id=%d\n",
               effective_song_id,
               original_song_id);
        return 0;
    }

    use_ai_mv = (force_mv_enabled != 0 || out_resolved->order.ai_mv[0] != '\0') ? 1 : 0;
    out_resolved->play_param.has_audio = (use_ai_mv != 0) ? 0 : 1;
    enable_subtitle = (out_resolved->play_param.has_audio == 0) ? 1 : 0;

    if (out_resolved->play_param.has_audio == 1)
    {
        karaoke_demo_set_positions(LV_ALIGN_TOP_LEFT, 1800, 1800, LV_ALIGN_BOTTOM_RIGHT, 1800, 1800);
    }

    if (force_mv_enabled)
    {
        video_url = force_mv_url;
    }
    else if (out_resolved->order.ai_mv[0] != '\0')
    {
        video_url = out_resolved->order.ai_mv;
    }
    else
    {
        video_url = out_resolved->song_info.mv_url;
    }

    if (out_resolved->play_param.has_audio == 1)
    {
        audio_url = NULL;
    }
    else
    {
        audio_url = out_resolved->song_info.mp3_url;
    }

    snprintf(out_resolved->subtitle_url,
             sizeof(out_resolved->subtitle_url),
             "%s",
             out_resolved->song_info.subtitle_url);

    out_resolved->subtitle_path[0] = '\0';
    if (enable_subtitle && out_resolved->subtitle_url[0] != '\0')
    {
        if (!ktv_fetch_subtitle_to_local(effective_song_id,
                                         out_resolved->subtitle_url,
                                         out_resolved->subtitle_path,
                                         sizeof(out_resolved->subtitle_path)))
        {
            printf("[KTV] [CLOUD_ORDER_AUTO] subtitle fetch failed, effective_song_id=%d, zrc=%s\n",
                   effective_song_id,
                   out_resolved->subtitle_url);
            out_resolved->subtitle_path[0] = '\0';
        }
        else if (out_resolved->subtitle_path[0] != '\0')
        {
            printf("[KTV] [CLOUD_ORDER_AUTO] subtitle ready, effective_song_id=%d, local=%s\n",
                   effective_song_id,
                   out_resolved->subtitle_path);
        }
    }

    out_resolved->play_param.mp4_url =
        (video_url != NULL && video_url[0] != '\0') ? video_url : NULL;
    out_resolved->play_param.mp3_url =
        (audio_url != NULL && audio_url[0] != '\0') ? audio_url : NULL;
    out_resolved->play_param.subtitle_path =
        (out_resolved->subtitle_path[0] != '\0') ? out_resolved->subtitle_path : NULL;

    if (out_resolved->play_param.has_audio == 1)
    {
        if (out_resolved->play_param.mp4_url == NULL)
        {
            printf("[KTV] [CLOUD_ORDER_AUTO] resolve failed: has_audio=1 requires mp4, effective_song_id=%d\n",
                   effective_song_id);
            return 0;
        }
    }
    else
    {
        if (out_resolved->play_param.mp4_url == NULL)
        {
            printf("[KTV] [CLOUD_ORDER_AUTO] resolve failed: has_audio=0 requires mp4, effective_song_id=%d\n",
                   effective_song_id);
            return 0;
        }

        if (out_resolved->play_param.mp3_url == NULL)
        {
            printf("[KTV] [CLOUD_ORDER_AUTO] resolve failed: has_audio=0 requires mp3, effective_song_id=%d\n",
                   effective_song_id);
            return 0;
        }
    }

    printf("[KTV] [CLOUD_ORDER_AUTO] resolved, selected_index=%d, original_song_id=%d, effective_song_id=%d, ai_mv=%s, has_audio=%d\n",
           selected_index,
           original_song_id,
           effective_song_id,
           out_resolved->order.ai_mv,
           out_resolved->play_param.has_audio);
    return 1;
}