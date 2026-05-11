#ifndef COVER_PAGE_DEMO_H
#define COVER_PAGE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_cover_adapter.h"
#include "lv_cover_view_style.h"

typedef struct {
    lv_obj_t *root;
    lv_vlist_t *vlist;
    lv_cover_adapter_t adapter;
    uint8_t ctrl_inited;
    uint8_t closing;
    uint8_t loader_inited;
} cover_page_demo_t;

int cover_page_demo_open(cover_page_demo_t *page,
                         lv_obj_t *parent,
                         uint32_t total_count,
                         uint32_t json_page_size,
                         const lv_cover_view_style_t *view_style);

void cover_page_demo_close(cover_page_demo_t *page);
void cover_page_demo_reset(cover_page_demo_t *page, uint32_t total_count);
void cover_page_demo_reset_to_page0(cover_page_demo_t *page, uint32_t total_count);

void app_cover_ui_init(void);
void app_cover_set_category_position(int category_pos);
void app_open_cover_page(lv_obj_t *parent);
void app_close_cover_page(void);
void app_prepare_close_cover_page(void);
void app_reset_cover_page_to_page0(uint32_t total_count);

/* 可选：外部实现这个函数，点击封面后会回调视频 id/title/play_url/description。 */
void app_cover_on_video_clicked(uint32_t video_id,
                                const char *title,
                                const char *play_url,
                                const char *description);

#ifdef __cplusplus
}
#endif

#endif
