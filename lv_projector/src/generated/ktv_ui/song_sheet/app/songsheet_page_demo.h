#ifndef SONGSHEET_PAGE_DEMO_H
#define SONGSHEET_PAGE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "lv_songsheet_adapter.h"
#include "lv_songsheet_view_style.h"

typedef struct
{
    lv_songsheet_adapter_t adapter;
    lv_obj_t *root;
} songsheet_page_demo_t;

int songsheet_page_demo_open(songsheet_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t page_size,
                             const lv_songsheet_view_style_t *view_style,
                             const void *placeholder_img_src,
                             const void *failed_img_src);

void songsheet_page_demo_close(songsheet_page_demo_t *page);
void demo_app_songsheet_list_tmp(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif
