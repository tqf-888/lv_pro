#ifndef LV_ORDER_SONG_CATALOG_H
#define LV_ORDER_SONG_CATALOG_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#define LV_ORDER_SONG_NAME_MAX 64
typedef struct {
    uint32_t slot_id;
    uint32_t song_id;
    uint8_t ready;
    uint8_t loading;
    uint8_t selected;
    char index_text[16];
    char name[LV_ORDER_SONG_NAME_MAX];
    char artist[LV_ORDER_SONG_NAME_MAX];
    char text_a[32];
    char text_b[32];
    char text_c[32];
    char text_d[32];
    char text_e[32];
    char text_f[32];
    char ai_mv_url[100];
    uint32_t color_idx;     lv_opa_t opa_idx;     const lv_font_t *font_idx;
    uint32_t color_name;    lv_opa_t opa_name;    const lv_font_t *font_name;
    uint32_t color_artist;  lv_opa_t opa_artist;  const lv_font_t *font_artist;
    uint32_t color_a;       lv_opa_t opa_a;       const lv_font_t *font_a;
    uint32_t color_b;       lv_opa_t opa_b;       const lv_font_t *font_b;
    uint32_t color_c;       lv_opa_t opa_c;       const lv_font_t *font_c;
    uint32_t color_d;       lv_opa_t opa_d;       const lv_font_t *font_d;
    uint32_t color_e;       lv_opa_t opa_e;       const lv_font_t *font_e;
    uint32_t color_f;       lv_opa_t opa_f;       const lv_font_t *font_f;
} lv_order_song_item_t;
typedef struct {
    uint32_t total_items;
    uint32_t capacity_items;
    lv_order_song_item_t *items;
    pthread_mutex_t mutex;
    uint8_t mutex_inited;
} lv_order_song_catalog_t;
bool lv_order_song_catalog_init(lv_order_song_catalog_t *catalog, uint32_t total_items);
void lv_order_song_catalog_deinit(lv_order_song_catalog_t *catalog);
bool lv_order_song_catalog_reset(lv_order_song_catalog_t *catalog, uint32_t total_items);
uint32_t lv_order_song_catalog_count(const lv_order_song_catalog_t *catalog);
bool lv_order_song_catalog_get_item(const lv_order_song_catalog_t *catalog, uint32_t index, lv_order_song_item_t *out);
bool lv_order_song_catalog_set_item_by_slot(lv_order_song_catalog_t *catalog, uint32_t slot, const lv_order_song_item_t *item);
/*
 * 仅用于把总条数向下收缩到已知末页，不重新分配内存。
 *
 * 警示：
 * - 当服务端返回空 data，或返回条数小于 batch_size 时，说明已经到最后一页；
 * - 这时必须把 catalog 的 total_items 同步缩小，否则 vlist 仍会认为后面还有“下一页”；
 * - 这里只允许缩小，不允许扩大。扩大仍然走 reset/init。
 */
bool lv_order_song_catalog_truncate(lv_order_song_catalog_t *catalog, uint32_t new_total_items);
bool lv_order_song_catalog_get_vlist_item(const lv_order_song_catalog_t *catalog, uint32_t index, lv_vlist_item_t *out);
void lv_order_song_catalog_set_selected(lv_order_song_catalog_t *catalog, uint32_t index, bool selected);
#ifdef __cplusplus
}
#endif
#endif
