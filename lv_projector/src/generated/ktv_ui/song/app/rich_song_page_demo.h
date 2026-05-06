#ifndef RICH_SONG_PAGE_DEMO_H
#define RICH_SONG_PAGE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_rich_song_adapter.h"
#include "lv_rich_song_view_style.h"

typedef struct {
    lv_obj_t *parent;
    lv_obj_t *root;
    lv_vlist_t *vlist;
    lv_rich_song_adapter_t adapter;
    uint32_t batch_size;
    const lv_rich_song_view_style_t *view_style;
} rich_song_page_demo_t;

int rich_song_page_demo_open(rich_song_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t batch_size,
                             const lv_rich_song_view_style_t *view_style);

void rich_song_page_demo_close(void);
void rich_song_page_demo_reset(uint32_t total_count);
void demo_app_songs_list(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif
