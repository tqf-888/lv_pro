#ifndef TOP100_SONG_PAGE_DEMO_H
#define TOP100_SONG_PAGE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_top100_song_adapter.h"
#include "lv_top100_song_view_style.h"

typedef struct {
    lv_obj_t *root;
    lv_vlist_t *vlist;
    lv_top100_song_adapter_t adapter;
} top100_song_page_demo_t;

int top100_song_page_demo_open(top100_song_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t batch_size,
                             const lv_top100_song_view_style_t *view_style);

void top100_song_page_demo_close(void);
void top100_song_page_demo_reset(uint32_t total_count);
void demo_app_top100_songs_list(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif
