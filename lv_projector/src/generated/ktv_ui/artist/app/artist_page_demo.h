#ifndef ARTIST_PAGE_DEMO_H
#define ARTIST_PAGE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_artist_adapter.h"
#include "lv_artist_view_style.h"

typedef struct {
    lv_obj_t *root;
    lv_vlist_t *vlist;
    lv_artist_adapter_t adapter;
    uint8_t ctrl_inited;
    uint8_t closing;
    uint8_t loader_inited;
} artist_page_demo_t;

int artist_page_demo_open(artist_page_demo_t *page,
                          lv_obj_t *parent,
                          uint32_t total_count,
                          uint32_t json_page_size,
                          const lv_artist_view_style_t *view_style);

void artist_page_demo_close(artist_page_demo_t *page);
void artist_page_demo_reset(artist_page_demo_t *page, uint32_t total_count);
void artist_page_demo_reset_to_page0(artist_page_demo_t *page, uint32_t total_count);

void app_ui_init(void);
void app_open_artist_page(lv_obj_t *parent);
void app_close_artist_page(void);
void app_prepare_close_artist_page(void);
void app_reset_artist_page_to_page0(uint32_t total_count);

#ifdef __cplusplus
}
#endif

#endif
