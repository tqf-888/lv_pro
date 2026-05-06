#ifndef ORDER_SONG_PAGE_DEMO_H
#define ORDER_SONG_PAGE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_order_song_adapter.h"
#include "lv_order_song_view_style.h"

typedef struct {
    lv_obj_t *root;
    lv_vlist_t *vlist;
    lv_order_song_adapter_t adapter;
} order_song_page_demo_t;

int order_song_page_demo_open(order_song_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t batch_size,
                             const lv_order_song_view_style_t *view_style);

void order_song_page_demo_close(void);
void order_song_page_demo_reset(uint32_t total_count);
void demo_app_order_songs_list(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif
