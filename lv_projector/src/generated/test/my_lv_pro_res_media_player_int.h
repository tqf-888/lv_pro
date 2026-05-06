
#ifndef MY_aaa_MEDIA_PLAYER_INT_H
#define MY_aaa_MEDIA_PLAYER_INT_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/select.h>
#include "include/sys_msg.h"

#include "lv_pro_res_mid_list.h"
#include "tplayer.h"


struct MovieInfo {
    char resolution[11];
    int audio;
    int subtitle;
    char filesize[10];
};

typedef enum MovieRatio
{
    Movie_Ratio_Auto   = 0,
    Movie_Ratio_16_9   = 1,
    Movie_Ratio_4_3    = 2,
} MovieRatio;

#define MUSIC_INFO_MAXSIZE 64

struct MusicInfo {
    char album[MUSIC_INFO_MAXSIZE];
    char artist[MUSIC_INFO_MAXSIZE];
    char filesize[10];
};

#define MOVIE_SUBTITLE_SUPPORT
#ifdef MOVIE_SUBTITLE_SUPPORT
typedef struct {
	int type; //0: pic; 1: text
	int w;
	int h;
	char *data;
} lv_subtitle_t;

typedef enum subtitles_event_t_ {

    SUBTITLES_EVENT_SHOW,
    SUBTITLES_EVENT_HIDDEN,
    SUBTITLES_EVENT_PAUSE,
    SUBTITLES_EVENT_RESUME,
    SUBTITLES_EVENT_STOP,
} subtitles_event_t;

#define MAX_EXT_SUBTITLE_NUM 24
typedef struct ext_subtitle {
    int ext_subs_count;
    char ** uris;
} ext_subtitle_t ;

ext_subtitle_t * ext_subtitle_data_get(void);
int ext_subtitles_init(void);
int ext_subtitle_deinit(void);

void subtitles_event_send(int e, lv_subtitle_t *subtitle);
#endif

/* 初始化与反初始化 */
// int aaa_media_list_init(char *cur_path, char *fn);
// int aaa_media_list_deinit(void);

/* 播放控制接口 */

void my_player_play(char *url);
int my_player_pause(void);
int my_player_resume(void);
int my_player_stop(void);
int player_play_other(int index);
void player_destroy(void);

/* 音量控制 */
int aaa_media_get_volumn(void);
int aaa_media_set_volumn(int volume);

/* 循环播放设置 */
int aaa_media_set_looping(bool bLoop);

/* 进度控制 */
int aaa_media_set_forward(int nSeekTimeMs);
int aaa_media_set_backward(int nSeekTimeMs);
void aaa_media_set_seek(bool backward);
int aaa_media_set_speed(int speed);

/* 视频显示设置 */
int aaa_movie_set_ratio(MovieRatio ratio);
void aaa_movie_get_info(struct MovieInfo *mi);
int aaa_movie_switch_audio(int nStreamIndex);
int aaa_movie_switch_subtitle(int nStreamIndex);

/* 音乐信息获取 */
void aaa_music_get_info(struct MusicInfo *mi);

/* 播放进度获取 */
void aaa_media_get_percent(double *percent);
void aaa_media_get_time(char *curTime, char *totalTime, double *percent);

/* 播放列表获取 */
list_head_t* aaa_media_get_media_list(void);

/* 底层TPlayer包装接口（如需外部调用） */
int player_seekto(int nSeekTimeMs);
int player_setvolumn(int volumn);
int player_getvolumn(void);
int player_setlooping(bool bLoop);
int player_setspeed(TplayerPlaySpeedType nSpeed);
void player_setratio(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
MediaInfo* player_getmediainfo(void);
int player_switchaudio(int index);
int player_switchsubtitle(int index);
int player_getduration(int *msec);
int player_getcurrentpos(int *msec);
int player_videodisplayenable(int enable);
int player_setsrcrect(int x, int y, unsigned int width, unsigned int height);
int player_setbrightness(unsigned int grade);
int player_setcontrast(unsigned int grade);
int player_sethue(unsigned int grade);
int player_setsaturation(unsigned int grade);

#endif /* MY_aaa_MEDIA_PLAYER_INT_H */