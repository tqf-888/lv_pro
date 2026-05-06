#include "db_list_pro_worker.h"
#include "ktv_player_ui.h"
#include "recommend_video_fetch.h"
#include <string.h>

extern void fetch_song_batch_1(char *singer_name, char *song_name, char *ai_mv_url);
extern void v_add_love_song(char *singer_name, char *song_name, char *ai_mv_url);



/*
 * 注册默认 worker 消息处理函数。
 */
int db_list_pro_worker_register(void)
{
    return db_list_pro_thread_set_worker_cb(db_list_pro_worker_handle, NULL);
}

#include "ktv_player_ui.h"
#include "ktv_cloud_order_api.h"

void db_list_pro_worker_handle(int i1,
                               int i2,
                               int i3,
                               void *p1,
                               void *p2,
                               void *p3,
                               void *user_data)
{
    (void)i3;
    (void)p1;
    (void)p2;
    (void)p3;
    (void)user_data;
    ktv_cloud_order_resolved_t resolved;
    char current_json[8192];
    char songinfo_json[16384];

    switch (i1) {
    case DBP_WORKER_MSG_PLAY:
        ktv_player_ui_resume();
        break;

    case DBP_WORKER_MSG_PAUSE:
        ktv_player_ui_pause();
        break;

    case DBP_WORKER_MSG_NEXT:
        if (i2 == 0)
        {
            memset(&resolved, 0, sizeof(resolved));
            memset(current_json, 0, sizeof(current_json));
            memset(songinfo_json, 0, sizeof(songinfo_json));

            if (ktv_cloud_order_resolve_first_play_param_auto("16666666666",
                                    "738ce11c448f23cd97b5e0a0ac901c4b",
                                    0,
                                    NULL,
                                    &resolved,
                                    current_json,
                                    sizeof(current_json),
                                    songinfo_json,
                                    sizeof(songinfo_json)) == 0)
            {
                DBP_LOGW("worker handle: NEXT resolve failed, i2=%d\n", i2);
                break;
            }

            ktv_player_ui_play(&resolved.play_param);
        }
        else if (i2 == 1)
        {
            memset(&resolved, 0, sizeof(resolved));
            memset(current_json, 0, sizeof(current_json));
            memset(songinfo_json, 0, sizeof(songinfo_json));

            if (ktv_cloud_order_resolve_first_play_param_auto("16666666666",
                                    "738ce11c448f23cd97b5e0a0ac901c4b",
                                    0,
                                    NULL,
                                    &resolved,
                                    current_json,
                                    sizeof(current_json),
                                    songinfo_json,
                                    sizeof(songinfo_json)) == 0)
            {
                DBP_LOGW("worker handle: NEXT resolve failed, i2=%d\n", i2);
                break;
            }

            ktv_player_ui_play(&resolved.play_param);
        }
        break;

    case DBP_WORKER_MSG_REPLAY:
        ktv_player_ui_replay_current();
        break;

    /* 先尝试恢复当前暂停中的 MV；只有当前没有可恢复内容时，才走自动切歌，失败后再回退到默认歌。 */
    case DBP_WORKER_MSG_AUTO_PLAY:
        if (ktv_player_ui_resume() == KTV_PLAYER_RET_FAIL)
        {
            memset(&resolved, 0, sizeof(resolved));
            memset(current_json, 0, sizeof(current_json));
            memset(songinfo_json, 0, sizeof(songinfo_json));

            if (ktv_cloud_order_resolve_first_play_param_auto("16666666666",
                                    "738ce11c448f23cd97b5e0a0ac901c4b",
                                    0,
                                    NULL,
                                    &resolved,
                                    current_json,
                                    sizeof(current_json),
                                    songinfo_json,
                                    sizeof(songinfo_json)) == 0)
            {
                memset(&resolved, 0, sizeof(resolved));
                memset(current_json, 0, sizeof(current_json));
                memset(songinfo_json, 0, sizeof(songinfo_json));

                if (ktv_cloud_order_resolve_first_play_param("16666666666",
                                        "738ce11c448f23cd97b5e0a0ac901c4b",
                                        1,   // 或 1
                                        5897449,
                                        NULL,
                                        &resolved,
                                        current_json,
                                        sizeof(current_json),
                                        songinfo_json,
                                        sizeof(songinfo_json)) == 0)
                {
                    DBP_LOGW("worker handle: AUTO_PLAY resolve failed\n");
                    break;
                }
            }

            ktv_player_ui_play(&resolved.play_param);
        }
        break;

    case DBP_WORKER_MSG_VIDEO_VOLUME:
        DBP_LOGI("worker handle: VIDEO_VOLUME=%d\n", i2);
        /* 用你自己的音量接口替换这里，i2 就是音量值 */
        /* 例如：ktv_player_ui_set_volume(i2); */
        break;

    case DBP_WORKER_MSG_EXIT_VIDEO:
        DBP_LOGI("worker handle: EXIT_VIDEO\n");
        ktv_player_ui_stop();
        break;

    case DBP_WORKER_MSG_set_video_pos:
        if (i2 == 1)
        {
            ktv_player_ui_set_small_rect();
        }
        else if (i2 == 0)
        {
            ktv_player_ui_set_right_rect();
        }
        else if (i2 == 2)
        {
            ktv_player_ui_set_full_rect();
        }
        
        break;

    case DBP_WORKER_MSG_add_song:

        fetch_song_batch_1(p1, p2, p3);
    break;

    case DBP_WORKER_MSG_love_song:
    v_add_love_song(p1, p2, p3);
    break;
    default:
        DBP_LOGW("worker handle: unknown msg=%d\n", i1);
        break;
    }
}