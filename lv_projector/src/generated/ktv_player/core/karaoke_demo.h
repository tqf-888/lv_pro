#ifndef KARAOKE_DEMO_H
#define KARAOKE_DEMO_H

#include "lvgl/lvgl.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef KARAOKE_TOP_LINE_ALIGN
#define KARAOKE_TOP_LINE_ALIGN            LV_ALIGN_TOP_LEFT
#endif

#ifndef KARAOKE_TOP_LINE_OFS_X
#define KARAOKE_TOP_LINE_OFS_X            20
#endif

#ifndef KARAOKE_TOP_LINE_OFS_Y
#define KARAOKE_TOP_LINE_OFS_Y            600
#endif

#ifndef KARAOKE_BOTTOM_LINE_ALIGN
#define KARAOKE_BOTTOM_LINE_ALIGN         LV_ALIGN_BOTTOM_RIGHT
#endif

#ifndef KARAOKE_BOTTOM_LINE_OFS_X
#define KARAOKE_BOTTOM_LINE_OFS_X         -20
#endif

#ifndef KARAOKE_BOTTOM_LINE_OFS_Y
#define KARAOKE_BOTTOM_LINE_OFS_Y         -80
#endif

#ifndef KARAOKE_TEXT_COLOR_BASE_HEX
#define KARAOKE_TEXT_COLOR_BASE_HEX       0x2F80FF
#endif

#ifndef KARAOKE_TEXT_COLOR_HIGHLIGHT_HEX
#define KARAOKE_TEXT_COLOR_HIGHLIGHT_HEX  0xFFFFFF
#endif

LV_FONT_DECLARE(lv_font_Regular_20);

void karaoke_demo_open(lv_obj_t *parent);
bool karaoke_demo_enqueue(const char *mv_url, const char *subtitle_path);

bool karaoke_demo_bind_subtitle(const char *subtitle_path);
bool karaoke_demo_is_render_enabled(void);

void karaoke_demo_play(void);
void karaoke_demo_pause(void);
void karaoke_demo_stop(void);

bool karaoke_demo_set_positions(lv_align_t top_align,
                                lv_coord_t top_ofs_x,
                                lv_coord_t top_ofs_y,
                                lv_align_t bottom_align,
                                lv_coord_t bottom_ofs_x,
                                lv_coord_t bottom_ofs_y);

/* 外部时钟驱动：播放器线程写入真实播放位置，字幕层通过异步刷新更新 UI */
void karaoke_demo_set_time_ms(uint32_t play_ms);
void karaoke_demo_request_refresh_async(void);

#ifdef __cplusplus
}
#endif

#endif /* KARAOKE_DEMO_H */
