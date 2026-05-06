#ifndef RECOMMEND_VIDEO_FETCH_H
#define RECOMMEND_VIDEO_FETCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    int id;
    char title[128];
    char play_url[256];
} VideoInfo_t;

int get_recommend_video_list(VideoInfo_t **videos);
void fetch_video_list(void);
void record_last_clicked_url(const char *url);
const char* get_last_clicked_url(void);

#ifdef __cplusplus
}
#endif

#endif