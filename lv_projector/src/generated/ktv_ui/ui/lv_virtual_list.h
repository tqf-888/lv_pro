#ifndef LV_VIRTUAL_LIST_H
#define LV_VIRTUAL_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LV_VLIST_ENABLE_PAGE_SWIPE
#define LV_VLIST_ENABLE_PAGE_SWIPE 1
#endif

#ifndef LV_VLIST_PAGE_SWIPE_THRESHOLD_PX
#define LV_VLIST_PAGE_SWIPE_THRESHOLD_PX 60
#endif

#ifndef LV_VLIST_PAGE_SWIPE_STRICT
#define LV_VLIST_PAGE_SWIPE_STRICT 1
#endif


typedef enum {
    LV_VLIST_IMG_NONE = 0,
    LV_VLIST_IMG_LOADING,
    LV_VLIST_IMG_READY,
    LV_VLIST_IMG_FAILED
} lv_vlist_img_state_t;

typedef struct {
    uint32_t item_id;
    const char *title;
    const char *subtitle;
    lv_vlist_img_state_t img_state;
    bool image_requested;
    const void *img_src;
} lv_vlist_item_t;

typedef uint32_t (*lv_vlist_get_count_cb_t)(void *user_ctx);
typedef bool (*lv_vlist_get_item_cb_t)(uint32_t index, lv_vlist_item_t *out, void *user_ctx);
typedef void (*lv_vlist_request_image_cb_t)(uint32_t item_id, uint32_t index, void *user_ctx);
typedef void (*lv_vlist_item_click_cb_t)(void *user_ctx,
                                         uint32_t item_id,
                                         uint32_t bound_index,
                                         lv_coord_t rel_x,
                                         lv_coord_t rel_y);

typedef struct {
    lv_vlist_get_count_cb_t get_count;
    lv_vlist_get_item_cb_t get_item;
    lv_vlist_request_image_cb_t request_image;
} lv_vlist_ops_t;

struct lv_vlist;
typedef struct lv_vlist lv_vlist_t;

typedef struct {
    lv_obj_t *root;
    void *renderer_ctx;
    uint32_t bound_index;
    uint32_t bound_item_id;
    bool bound_valid;
} lv_vlist_cell_t;

typedef struct {
    void *(*create_cell)(lv_vlist_t *vlist, lv_obj_t *parent);
    void (*destroy_cell)(lv_vlist_t *vlist, void *renderer_ctx);
    lv_obj_t *(*get_root)(lv_vlist_t *vlist, void *renderer_ctx);
    void (*bind_cell)(lv_vlist_t *vlist, void *renderer_ctx, const lv_vlist_item_t *item);
    void (*set_pos)(lv_vlist_t *vlist, void *renderer_ctx, lv_coord_t x, lv_coord_t y);
} lv_vlist_renderer_ops_t;

typedef struct {
    lv_obj_t *parent;
    uint32_t visible_rows;
    uint32_t visible_cols;
    /*
     * 前向 overscan 行数。
     *
     * 注意：它表示“在当前 top_row 之前，最多额外保留多少行物理 cell”，
     * 不是“无条件把首屏整体上移多少行”。
     * 当 top_row=0 时，实际生效值必须被钳成 0。
     */
    uint32_t overscan_rows_front;
    uint32_t overscan_rows_back;
    uint32_t preload_before;
    uint32_t preload_after;
    lv_coord_t viewport_width;
    lv_coord_t viewport_height;
    lv_coord_t cell_width;
    lv_coord_t cell_height;
    lv_coord_t gap_x;
    lv_coord_t gap_y;
    const void *placeholder_img_src;
    const void *failed_img_src;
    void *user_ctx;
    const lv_vlist_renderer_ops_t *renderer_ops;
    const void *renderer_style;
    lv_vlist_item_click_cb_t on_item_click;
} lv_vlist_config_t;

lv_vlist_t *lv_vlist_create(const lv_vlist_config_t *cfg, const lv_vlist_ops_t *ops);
void lv_vlist_destroy(lv_vlist_t *vlist);

void lv_vlist_reload(lv_vlist_t *vlist);
void lv_vlist_scroll_to(lv_vlist_t *vlist, uint32_t top_index);
void lv_vlist_scroll_by(lv_vlist_t *vlist, int32_t delta_rows);
uint32_t lv_vlist_get_top_index(const lv_vlist_t *vlist);
void lv_vlist_scroll_by_px(lv_vlist_t *vlist, int32_t delta_px);
int32_t lv_vlist_get_scroll_offset_px(const lv_vlist_t *vlist);

void lv_vlist_notify_item_changed(lv_vlist_t *vlist, uint32_t item_id);
void lv_vlist_notify_range_changed(lv_vlist_t *vlist, uint32_t first_index, uint32_t count);
void lv_vlist_request_visible_images(lv_vlist_t *vlist);

const lv_vlist_config_t *lv_vlist_get_config(const lv_vlist_t *vlist);

#ifdef __cplusplus
}
#endif

#endif
