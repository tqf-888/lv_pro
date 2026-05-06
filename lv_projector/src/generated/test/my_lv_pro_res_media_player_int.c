#include "my_lv_pro_res_media_player_int.h"
#include "time.h"

#include <dirent.h>

#include <sunxi_display2.h>
#include "../../Layer/msgbox/lv_ui_msgbox.h"
#include "../file_explorer/lv_pro_file_explorer.h"
#include "include/sys_param.h"
#include "lv_pro_res_debug.h"
#include "ktv.h"  // 使用统一的打印方法


#define CEDARX_UNUSE(param) (void)param
#define ISNULL(x) if(!x){return -1;}

#define CHECK_MEDIA_EXIT() \
    do { \
        if (media_state == MEDIA_STATE_EXIT) { \
            dbg_print("MEDIA_STATE_EXIT!\n"); \
            return -1; \
        } \
    } while(0)

    
typedef enum 
{
    MEDIA_STATE_PREPARED,        // 准备完成（可开始播放）
    MEDIA_STATE_EXIT,            // 已退出（替代stopped，避免与paused混淆）
} media_state_t;

media_state_t media_state = MEDIA_STATE_EXIT;

typedef struct PLAYER_CONTEXT_T {
    TPlayer *mTPlayer;
    int mSeekable;
    int mError;
    int mVideoFrameNum;
    int mPreparedFlag;
    int mLoopFlag;
    int mSetLoop;

    sem_t mPreparedSem;
} player_context_t;

typedef struct lv_disp_rect {
    int x; /* xoffset */
    int y; /* yoffset */
    int w; /* width */
    int h; /* height */
} lv_disp_rect;

typedef struct movie_resolution {
    lv_disp_rect screen;
    lv_disp_rect video;
    lv_disp_rect movie;
    float screen_ratio;
} movie_resolution;

 player_context_t player_context;
 movie_resolution movie_res;
 list_head_t *media_list;


extern int movie_ratio;
extern bool media_mode_single;
 pthread_mutex_t media_play_mutex = PTHREAD_MUTEX_INITIALIZER;
extern FileType media_filetype;
#ifdef MOVIE_SUBTITLE_SUPPORT
 lv_subtitle_t lv_subtitle={0};
 int last_buf_size = 0;
#endif

 int lv_pro_get_screen_resolution(int *width, int *height)
{
    unsigned long ioctlParam[2];
    int disp_fd = open("/dev/disp", O_RDWR);
    if (disp_fd < 0) {
        dbg_print("open disp handle error!\n");
        goto Fail;
    }
    ioctlParam[0] = 0;
    ioctlParam[1] = 0;
    *width = ioctl(disp_fd, DISP_GET_SCN_WIDTH, ioctlParam);
    *height = ioctl(disp_fd, DISP_GET_SCN_HEIGHT, ioctlParam);
    if (*width == 0 || *height == 0) {
        dbg_print("get screen size fail\n");
        goto Fail;
    }
    close(disp_fd);
    return 0;
Fail:
    *width = LV_HOR_RES;
    *height = LV_VER_RES;
    return -1;
}

 void lv_pro_set_movie_rect(void)
{
    float video_ratio = ((float)movie_res.video.w / (float)movie_res.video.h);
    if (video_ratio > movie_res.screen_ratio) {
        float scaleratio = ((float)movie_res.screen.w / (float)movie_res.video.w);
        int scale_h = movie_res.video.h * scaleratio;
        movie_res.movie.x = 0;
        movie_res.movie.y = movie_res.screen.h / 2 - scale_h / 2;
        movie_res.movie.w = movie_res.screen.w;
        movie_res.movie.h = scale_h;
    } else {
        float scaleratio = ((float)movie_res.screen.h / (float)movie_res.video.h);
        int scale_w = movie_res.video.w * scaleratio;
        movie_res.movie.x = movie_res.screen.w / 2 - scale_w / 2;
        movie_res.movie.y = 0;
        movie_res.movie.w = scale_w;
        movie_res.movie.h = movie_res.screen.h;
    }
}

//* a callback for tplayer.
 int CallbackForTPlayer(void *pUserData, int msg, int param0,
        void *param1) {
    player_context_t *pPlayer = (player_context_t*) pUserData;

    CEDARX_UNUSE(param1);
    switch (msg) {
    case TPLAYER_NOTIFY_PREPARED: {
        dbg_print("player_NOTIFY_PREPARED,has prepared.\n");
        media_state = MEDIA_STATE_PREPARED;
        sem_post(&pPlayer->mPreparedSem);
        break;
    }
    case TPLAYER_NOTIFY_PLAYBACK_COMPLETE: {
        printf("\n\n\n\nTPLAYER_NOTIFY_PLAYBACK_COMPLETE\n\n\n\n\n\n");

        //需要播放下一个
        // TPlayerStart(player_context.mTPlayer);
        break;
    }
    case TPLAYER_NOTIFY_SEEK_COMPLETE: {
        dbg_print("player_NOTIFY_SEEK_COMPLETE>>>>info: seek ok.\n");
        break;
    }
    case TPLAYER_NOTIFY_SEEK_START_POINT: { 
        media_state = MEDIA_STATE_PREPARED;
        dbg_print("player_NOTIFY_SEEK_START_POINT\n");
        // my_player_play();  //开始播放
        break;
    }
    case TPLAYER_NOTIFY_MEDIA_ERROR: {
        TPlayerReset(player_context.mTPlayer);
    //     switch (param0) {
    //     case player_MEDIA_ERROR_UNKNOWN: {
    //         dbg_print("erro type:player_MEDIA_ERROR_UNKNOWN\n");
    //         player_context.mError = 1;
    //         break;
    //     }
    //     case player_MEDIA_ERROR_UNSUPPORTED: {
    //         dbg_print("erro type:player_MEDIA_ERROR_UNSUPPORTED\n");
    //         player_context.mError = 1;
    //         break;
    //     }
    //     case player_MEDIA_ERROR_IO: {
    //         dbg_print("erro type:player_MEDIA_ERROR_IO\n");
    //         player_context.mError = 1;
    //         break;
    //     }
    //     case player_MEDIA_ERROR_AUDIO_UNSUPPORTED: {
    //         dbg_print("erro type:player_MEDIA_ERROR_AUDIO_UNSUPPORTED\n");
    //         break;
    //     }
    //     }
        break;
    }
    case TPLAYER_NOTIFY_NOT_SEEKABLE: {
        pPlayer->mSeekable = 0;
        dbg_print("info: media source is unseekable.\n");
        break;
    }
    case TPLAYER_NOTIFY_BUFFER_START: {
        dbg_print("have no enough data to play\n");
        break;
    }
    case TPLAYER_NOTIFY_BUFFER_END: {
        dbg_print("have enough data to play again\n");
        break;
    }
    case TPLAYER_NOTIFY_VIDEO_FRAME: {
        //dbg_print("get the decoded video frame\n");
        break;
    }
    case TPLAYER_NOTIFY_AUDIO_FRAME: {
        //dbg_print("get the decoded audio frame\n");
        break;
    }
    case TPLAYER_NOTIFY_MEDIA_VIDEO_SIZE: {
        movie_res.video.w = ((int*)param1)[0];   //real decoded video width
        movie_res.video.h = ((int*)param1)[1];   //real decoded video height
        lv_pro_set_movie_rect();
        break;
    }
#ifdef MOVIE_SUBTITLE_SUPPORT
    // case player_NOTIFY_SUBTITLE_FRAME: {
    //     dbg_print("player_NOTIFY_SUBTITLE_FRAME: get the decoded subtitle frame\n");
    //     uintptr_t *p = (uintptr_t*)param1;
    //     unsigned long buf_size = 0;
    //     lv_subtitle.type = ((SubtitleItem*)(p[1]))->bText;
    //     if (lv_subtitle.type == 0) {
    //         char *data = (char *)((SubtitleItem*)(p[1]))->pBitmapData;
    //         if(data != NULL) {
    //             lv_subtitle.w = ((SubtitleItem*)(p[1]))->nBitmapWidth;
    //             lv_subtitle.h = ((SubtitleItem*)(p[1]))->nBitmapHeight;
    //             buf_size = lv_subtitle.w * lv_subtitle.h * 4; //ARGB
    //             if (last_buf_size < buf_size) {
    //                 last_buf_size = buf_size;
    //                 if (!lv_subtitle.data) {
    //                     lv_subtitle.data = malloc(buf_size);
    //                 } else {
    //                     lv_subtitle.data = realloc(lv_subtitle.data, buf_size);
    //                 }
    //                 if (!lv_subtitle.data) {
    //                     dbg_print("Error: no memory %s:%d\n", __func__ ,__LINE__);
    //                 }
    //                 memset(lv_subtitle.data, 0, buf_size);
    //             } else{
    //                 memset(lv_subtitle.data, 0, last_buf_size);
    //             }
    //             memcpy(lv_subtitle.data, data, buf_size);
    //             subtitles_event_send(SUBTITLES_EVENT_SHOW, &lv_subtitle);
    //             dbg_print("id:%d, pItem:%lu, pic size:%lu, WxH:%dx%d\n", (int)p[0], p[1], buf_size,
    //                     lv_subtitle.w, lv_subtitle.h);
    //         } else {
    //             dbg_print("player_NOTIFY_SUBTITLE_FRAME: pic subtitle data is NULL!\n");
    //         }
    //     } else if (lv_subtitle.type == 1) {
    //         char *text = (char *)((SubtitleItem*)(p[1]))->pText;
    //         if(text != NULL) {
    //             buf_size = strlen(text);
    //             if (last_buf_size < buf_size) {
    //                 last_buf_size = buf_size;
    //                 if (!lv_subtitle.data) {
    //                     lv_subtitle.data = malloc(buf_size);
    //                 } else {
    //                     lv_subtitle.data = realloc(lv_subtitle.data, buf_size);
    //                 }
    //                 if (!lv_subtitle.data) {
    //                     dbg_print("Error: no memory %s:%d\n", __func__ ,__LINE__);
    //                     break;
    //                 }
    //                 memset(lv_subtitle.data, 0, buf_size);
    //             } else {
    //                 memset(lv_subtitle.data, 0, last_buf_size);
    //             }
    //             memcpy(lv_subtitle.data, text, buf_size);
    //             subtitles_event_send(SUBTITLES_EVENT_SHOW, &lv_subtitle);
    //             dbg_print("id:%d, pItem:%lu, text size:%lu data:%s", (int)p[0], p[1], buf_size, text);
    //         } else {
    //             dbg_print("player_NOTIFY_SUBTITLE_FRAME: text subtitle data is NULL!\n");
    //         }
    //     } else {
    //         dbg_print("player_NOTIFY_SUBTITLE_FRAME: Unrecognized subtitle format %d\n", lv_subtitle.type);
    //     }
    //     break;
    // }
    // case player_NOTIFY_HIDE_SUBTITLE_FRAME: {
    //     dbg_print("player_NOTIFY_HIDE_SUBTITLE_FRAME\n");
    //     subtitles_event_send(SUBTITLES_EVENT_HIDDEN, NULL);
    //     break;
    // }
#endif
    default: {
        dbg_print("warning: unknown callback from Tinaplayer.\n");
        break;
    }
    }
    return 0;
}

 int semTimedWait(sem_t *sem, int64_t time_ms) {
    int err;

    if (time_ms == -1) {
        err = sem_wait(sem);
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += time_ms % 1000 * 1000 * 1000;
        ts.tv_sec += time_ms / 1000 + ts.tv_nsec / (1000 * 1000 * 1000);
        ts.tv_nsec = ts.tv_nsec % (1000 * 1000 * 1000);

        err = sem_timedwait(sem, &ts);
    }

    return err;
}

 int player_seekto(int nSeekTimeMs) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
     if(TPlayerIsPlaying(player_context.mTPlayer)){
     dbg_print("seekto can not at playing state!\n");
     return -1;
     }
    return TPlayerSeekTo(player_context.mTPlayer, nSeekTimeMs);
}

 int player_setvolumn(int volumn) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerSetVolume(player_context.mTPlayer, volumn);
}

 int player_getvolumn(void) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerGetVolume(player_context.mTPlayer);
}

 int player_setlooping(bool bLoop) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerSetLooping(player_context.mTPlayer, bLoop);
}

 int player_setspeed(TplayerPlaySpeedType nSpeed) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    if (!TPlayerIsPlaying(player_context.mTPlayer)) {
        dbg_print("not playing!\n");
        return 1;
    }
    return TPlayerSetSpeed(player_context.mTPlayer, nSpeed);
}

 void player_setratio(unsigned int x, unsigned int y,
        unsigned int width, unsigned int height) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    TPlayerSetDisplayRect(player_context.mTPlayer, x, y, width, height);
}

 MediaInfo* player_getmediainfo(void) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerGetMediaInfo(player_context.mTPlayer);
}

 int player_switchaudio(int index) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerSwitchAudio(player_context.mTPlayer, index);
}

 int player_switchsubtitle(int index) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerSwitchSubtitle(player_context.mTPlayer, index);
}

 int player_getduration(int *msec) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerGetDuration(player_context.mTPlayer, msec);
}

 int player_getcurrentpos(int *msec) {
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerGetCurrentPosition(player_context.mTPlayer, msec);
}

 int player_videodisplayenable(int enable) {
    TPlayerSetVideoDisplay(player_context.mTPlayer, enable);
    return 0;
}

 int player_setsrcrect(int x, int y, unsigned int width, unsigned int height) {
    TPlayerSetSrcRect(player_context.mTPlayer, x, y, width, height);
    return 0;
}

 int player_setbrightness(unsigned int grade) {
    TPlayerSetBrightness(player_context.mTPlayer, grade);
    return 0;
}

 int player_setcontrast(unsigned int grade) {
    TPlayerSetContrast(player_context.mTPlayer, grade);
    return 0;
}

 int player_sethue(unsigned int grade) {
    TPlayerSetHue(player_context.mTPlayer, grade);
    return 0;
}

 int player_setsaturation(unsigned int grade) {
    TPlayerSetSaturation(player_context.mTPlayer, grade);
    return 0;
}

// int aaa_media_list_init(char *cur_path, char *fn) {
//     DIR *dp;
//     struct dirent *dirp;
//     char compete_path[FILE_PATH_MAXT_LEN];
//     char compete_name[FILE_NAME_MAXT_LEN];
//     int index = 0;
//     int cur_index = 1;

//     media_list = create_list();

//     dp = opendir(cur_path);
//     if (!dp) {
//         dbg_print("open directory %s error\n", cur_path);
//         return -1;
//     }

//     /*Get media file name */
//     while ((dirp = readdir(dp))) {
//         if (lv_pro_check_file_type(dirp->d_name, media_filetype) == true) {
//             dbg_print("media filename: %s/%s\n", cur_path, dirp->d_name);
//             memset(compete_path, 0, sizeof(compete_path));
//             memset(compete_name, 0, sizeof(compete_name));
//             snprintf(compete_path, FILE_PATH_MAXT_LEN, "%s/%s", cur_path,
//                     dirp->d_name);
//             snprintf(compete_name, FILE_NAME_MAXT_LEN, "%s", dirp->d_name);
//             list_append(media_list, compete_path, compete_name,
//                     MEDIA_F_TYPE_VIDEO);
//             ++index;
//             if (!strcmp(dirp->d_name, fn)) {
//                 cur_index = index;
//             }
// #ifdef MOVIE_SUBTITLE_SUPPORT
//         } else if (media_filetype == FileType_Movie) {
//             lv_pro_file_subtitile_list_create(dirp->d_name);
// #endif
//         }
//     }

//     closedir(dp);

//     /* Initialize the default file, the media play activity can be played immediately */
//     list_skip_to_index(media_list, cur_index);

//     return 0;
// }

// int aaa_media_list_deinit(void) {
//     destroy_list(media_list);
// #ifdef MOVIE_SUBTITLE_SUPPORT
//     if (media_filetype == FileType_Movie) {
//         lv_pro_file_subtitle_list_free();
//     }
// #endif
//     return 0;
// }

int aaa_media_get_volumn(void) {
    return player_getvolumn();
}

int aaa_media_set_volumn(int volume) {
    return player_setvolumn(volume);
}

int aaa_media_set_looping(bool bLoop) {
    return player_setlooping(bLoop);
}

int aaa_media_set_forward(int nSeekTimeMs) {
    int curDuration = 0;
    int totalDuration = 0;
    int seekDuration = 0;
    player_getduration(&totalDuration);
    player_getcurrentpos(&curDuration);
    seekDuration = curDuration + nSeekTimeMs;
    if (seekDuration > totalDuration)
        seekDuration = totalDuration;
    return player_seekto(seekDuration);
}

int aaa_media_set_backward(int nSeekTimeMs) {
    int curDuration = 0;
    int totalDuration = 0;
    int seekDuration = 0;
    player_getduration(&totalDuration);
    player_getcurrentpos(&curDuration);
    seekDuration = curDuration - nSeekTimeMs;
    if (seekDuration < 0)
        seekDuration = 0;
    return player_seekto(seekDuration);
}

 int SeekTime = 0;
 int64_t last_seek_op_time = 0;
void aaa_media_set_seek(bool backward) {
    uint32_t total_time = 0;
    uint32_t play_time_initial = 0;
    uint32_t play_time = 0;
    uint32_t jump_interval = 0;
    uint32_t seek_time = 0;
    int64_t cur_op_time = 0;
    struct timeval time_t;

    player_getduration(&total_time);
    player_getcurrentpos(&play_time_initial);
    play_time = play_time_initial;

    if (total_time < jump_interval)
        return;
    if(play_time_initial >= total_time)
        return;

    gettimeofday(&time_t, NULL);
    cur_op_time = time_t.tv_sec * 1000 + time_t.tv_usec / 1000;
    if ((cur_op_time - last_seek_op_time) < 800) {
        SeekTime += 10000;
    } else {
        SeekTime = 10000;
    }

    last_seek_op_time = cur_op_time;

    if (SeekTime < 300000)
        jump_interval = SeekTime;
    else
        jump_interval = 300000;

    if (backward) { //seek backward
        if (play_time > jump_interval)
            seek_time = play_time - jump_interval;
        else
            seek_time = 0;
    } else { //seek forward
        if ((play_time + jump_interval) > total_time)
            seek_time = total_time;
        else
            seek_time = play_time + jump_interval;
    }
    player_seekto(seek_time);
}

int aaa_media_set_speed(int speed) {
    return player_setspeed(speed);
}

int aaa_movie_set_ratio(MovieRatio ratio) {
    int ret = 0;
    CHECK_MEDIA_EXIT();
    int sys_width = movie_res.screen.w;
    int sys_height = movie_res.screen.h;
    int value = lv_get_sys_param(P_ASPECT_RATIO);
    switch (ratio) {
        case Movie_Ratio_16_9:
        {
            if (value == 0) { /* ASPECT_RATIO_16_9_ID */
                player_setratio(0, 0, sys_width, sys_height);
            } else { /* ASPECT_RATIO_4_3_ID */
                int ratio_h = sys_height/4*3;
                player_setratio(0, (sys_height - ratio_h)/2, sys_width, ratio_h);
            }
            break;
        }
        case Movie_Ratio_4_3:
        {
            if (value == 0) { /* ASPECT_RATIO_16_9_ID */
                int ratio_w = sys_height/3*4;
                player_setratio((sys_width - ratio_w)/2, 0, ratio_w, sys_height);
            } else { /* ASPECT_RATIO_4_3_ID */
                player_setratio(0, 0, sys_width, sys_height);
            }
            break;
        }
        case Movie_Ratio_Auto:
        {
            player_setratio(movie_res.movie.x, movie_res.movie.y,
                    movie_res.movie.w, movie_res.movie.h);
            break;
        }
        default:
        {
            ret = -1;
            break;
        }
    }
    return ret;
}

void aaa_movie_get_info(struct MovieInfo *mi) {
    int width = 0, height = 0;
    float filesize = 0;
    MediaInfo* MediaInfo = NULL;
    MediaInfo = player_getmediainfo();
    if(MediaInfo != NULL) {
        if (MediaInfo->nFileSize > 0) {
            if (MediaInfo->nFileSize < 1024) {
                filesize = MediaInfo->nFileSize;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fB", filesize);
            } else if ((MediaInfo->nFileSize >= 1024) && (MediaInfo->nFileSize < 1048576)) {
                filesize = MediaInfo->nFileSize / 1024.0;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fKB", filesize);
            } else if ((MediaInfo->nFileSize >= 1048576) && (MediaInfo->nFileSize < 1073741824)) {
                filesize = MediaInfo->nFileSize / 1048576.0;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fMB", filesize);
            } else {
                filesize = MediaInfo->nFileSize / 1073741824.0;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fGB", filesize);
            }
        } else {
            snprintf(mi->filesize, sizeof(mi->filesize), "%s", "--");
        }

        if(MediaInfo->pVideoStreamInfo != NULL) {
            width = MediaInfo->pVideoStreamInfo->nWidth;
            height = MediaInfo->pVideoStreamInfo->nHeight;
            if (width || height) {
                snprintf(mi->resolution, sizeof(mi->resolution), "%dx%d", width, height);
            } else {
                snprintf(mi->resolution, sizeof(mi->resolution), "%s", "--x--");
            }
        } else {
            snprintf(mi->resolution, sizeof(mi->resolution), "%s", "--x--");
        }

        mi->audio = MediaInfo->nAudioStreamNum;
        mi->subtitle = MediaInfo->nSubtitleStreamNum;
    } else {
        snprintf(mi->filesize, sizeof(mi->filesize), "%s", "--");
        snprintf(mi->resolution, sizeof(mi->resolution), "%s", "--x--");
        mi->audio = 0;
        mi->subtitle = 0;
    }
}

int aaa_movie_switch_audio(int nStreamIndex) {
    return player_switchaudio(nStreamIndex - 1);
}

int aaa_movie_switch_subtitle(int nStreamIndex) {
    return player_switchsubtitle(nStreamIndex - 1);
}

void aaa_music_get_info(struct MusicInfo *mi) {
    float filesize = 0;
    char album[MUSIC_INFO_MAXSIZE] = {0};
    char artist[MUSIC_INFO_MAXSIZE] = {0};
    MediaInfo* MediaInfo = NULL;
    MediaInfo = player_getmediainfo();
    if(MediaInfo != NULL) {
        if (MediaInfo->nFileSize) {
            if (MediaInfo->nFileSize < 1024) {
                filesize = MediaInfo->nFileSize;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fB", filesize);
            } else if ((MediaInfo->nFileSize >= 1024) && (MediaInfo->nFileSize < 1048576)) {
                filesize = MediaInfo->nFileSize / 1024.0;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fKB", filesize);
            } else if ((MediaInfo->nFileSize >= 1048576) && (MediaInfo->nFileSize < 1073741824)) {
                filesize = MediaInfo->nFileSize / 1048576.0;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fMB", filesize);
            } else {
                filesize = MediaInfo->nFileSize / 1073741824.0;
                snprintf(mi->filesize, sizeof(mi->filesize), "%.2fGB", filesize);
            }
        } else {
            snprintf(mi->filesize, sizeof(mi->filesize), "%s", "--");
        }

        if(MediaInfo->mId3Info.albumsz > 0) {
            string_fmt_conv_to_utf8(MediaInfo->mId3Info.album, &album, MUSIC_INFO_MAXSIZE/2);
            snprintf(mi->album, sizeof(mi->album), "%s", album);
        } else {
            snprintf(mi->album, sizeof(mi->album), "%s", "--");
        }

        if(MediaInfo->mId3Info.artistsz > 0) {
            string_fmt_conv_to_utf8(MediaInfo->mId3Info.artist, &artist, MUSIC_INFO_MAXSIZE/2);
            snprintf(mi->artist, sizeof(mi->artist), "%s", artist);
        } else {
            snprintf(mi->artist, sizeof(mi->artist), "%s", "--");
        }
    } else {
        snprintf(mi->filesize, sizeof(mi->filesize), "%s", "--");
        snprintf(mi->album, sizeof(mi->album), "%s", "--");
        snprintf(mi->artist, sizeof(mi->artist), "%s", "--");
    }
}

void aaa_media_get_percent(double *percent) {
    int curDuration, totalDuration;
    player_getduration(&totalDuration);
    player_getcurrentpos(&curDuration);
    *percent = (double) curDuration / totalDuration;
}

void aaa_media_get_time(char *curTime, char *totalTime, double *percent) {
    int ret, curDuration, totalDuration, timeMin, timeSec;
    double sec;

    ret = player_getduration(&totalDuration);

    if (!ret) {
        timeMin = totalDuration / 1000 / 60;
        sec = (double) totalDuration / 1000 / 60 - timeMin;
        timeSec = sec * 60;
        snprintf(totalTime, 6, "%02d:%02d", timeMin, timeSec);
        ret = player_getcurrentpos(&curDuration);

        if (!ret) {
            timeMin = curDuration / 1000 / 60;
            sec = (double) curDuration / 1000 / 60 - timeMin;
            timeSec = sec * 60;
            snprintf(curTime, 6, "%02d:%02d", timeMin, timeSec);

            *percent = (double) curDuration / totalDuration;
        } else {
            snprintf(curTime, 6, "%s", "00:00");
            *percent = 0;
        }
    } else {
        snprintf(totalTime, 6, "%s", "00:00");
        snprintf(curTime, 6, "%s", "00:00");
        *percent = 0;
    }

}

list_head_t* aaa_media_get_media_list() {
    return media_list;
}


//===============   隔离    ===============////===============   隔离    ===============////===============   隔离    ===============//
extern void tplayer_setratio(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

//===============   隔离TODO 客户连续点击播放或者其他   ===============//
//===============   隔离TODO 客户连续点击播放或者其他   ===============//
//===============   隔离TODO 客户连续点击播放或者其他   ===============//

//===============   播放(先停止,才能播放下一个)    ===============//

int ktv_play_mv(void *arg)
{
    int waitErr = 0;

    if (player_context.mTPlayer) {
        player_destroy();
    }

    lv_pro_get_screen_resolution(&movie_res.screen.w, &movie_res.screen.h);
    movie_res.screen_ratio = ((float)movie_res.screen.w / (float)movie_res.screen.h);
    dbg_print("media get screen size: width = %d, height = %d.", movie_res.screen.w, movie_res.screen.h);

    //* create a player.
    player_context.mTPlayer = TPlayerCreate(CEDARX_PLAYER);
    if (player_context.mTPlayer == NULL) {
        dbg_print("can not create tplayer, quit.\n");
        return -1;
    }

    //* set callback to player.
    TPlayerSetNotifyCallback(player_context.mTPlayer, CallbackForTPlayer, (void*) &player_context);

    //set player start status
    player_context.mError = 0;
    player_context.mSeekable = 1;
    player_context.mLoopFlag = 0;
    player_context.mSetLoop = 0;

    sem_init(&player_context.mPreparedSem, 0, 0);

    TPlayerReset(player_context.mTPlayer);
    TPlayerSetDebugFlag(player_context.mTPlayer, 0);

    //* set url to the tinaplayer.
    if (TPlayerSetDataSource(player_context.mTPlayer, (char*)arg, NULL) != 0) {
        dbg_print("TPlayerSetDataSource return fail.\n");
        return -1;
    } 

    if (player_context.mError) {
        dbg_print("error: open media source fail.\n");
        TPlayerReset(player_context.mTPlayer);
        return -1;
    }
    TPlayerSetG2dRotate(player_context.mTPlayer, 90);
    if (TPlayerPrepareAsync(player_context.mTPlayer) != 0) {
        dbg_print("TPlayerPrepareAsync() return fail.\n");
    } 

    waitErr = semTimedWait(&player_context.mPreparedSem, 10 * 1000);
    if (waitErr == -1) {
        dbg_print("prepare fail\n");
        return -1;
    }
    dbg_print("prepared ok\n");
    dbg_print("TPlayerSetHoldLastPicture()\n"); 
    TPlayerSetHoldLastPicture(player_context.mTPlayer, 1); 

    if (media_mode_single) {
        aaa_media_set_looping(true);
        
    }
        tplayer_setratio(0,0,800,1280);

    return TPlayerStart(player_context.mTPlayer);
}

void my_player_play(char *url) {
    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &attr, ktv_play_mv, url);
    pthread_attr_destroy(&attr);
}
//===============   暂停    ===============//
 int my_player_pause(void)
{
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    return TPlayerPause(player_context.mTPlayer);
}

//===============   恢复    ===============//
 int my_player_resume(void)
{
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    if (TPlayerIsPlaying(player_context.mTPlayer)) {
        dbg_print("already playing!\n");
        return 1;
    }
    return TPlayerStart(player_context.mTPlayer);
}

//===============   停止    ===============//
 int my_player_stop(void) 
{
    ISNULL(player_context.mTPlayer);
    CHECK_MEDIA_EXIT();
    TPlayerStop(player_context.mTPlayer);
    return TPlayerReset(player_context.mTPlayer);
}

//===============   销毁    ===============//
 void* MediaDeinitProc(void *arg) {
    pthread_mutex_lock(&media_play_mutex);
    if (!player_context.mTPlayer) {
        dbg_print("player not init.\n");
        return -1;
    }
    TPlayerReset(player_context.mTPlayer);
    TPlayerDestroy(player_context.mTPlayer);
    player_context.mTPlayer = NULL;
    sem_destroy(&player_context.mPreparedSem);

    pthread_mutex_unlock(&media_play_mutex);

    return NULL;
}

void player_destroy(void) {
    media_state = MEDIA_STATE_EXIT;
    // aaa_media_list_deinit();//字幕销毁
    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &attr, MediaDeinitProc, NULL);
    pthread_attr_destroy(&attr);
}


