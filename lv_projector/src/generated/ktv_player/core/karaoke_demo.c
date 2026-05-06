#include "karaoke_demo.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef KARAOKE_LOG_ENABLE
#define KARAOKE_LOG_ENABLE 1
#endif

#if KARAOKE_LOG_ENABLE
#define KARAOKE_LOG(fmt, ...) \
    printf("[karaoke] " fmt "\n", ##__VA_ARGS__)
#else
#define KARAOKE_LOG(fmt, ...)
#endif

#ifndef KARAOKE_MAX_QUEUE
#define KARAOKE_MAX_QUEUE          8
#endif

#ifndef KARAOKE_MAX_LINES
#define KARAOKE_MAX_LINES          512
#endif

#ifndef KARAOKE_MAX_TEXT
#define KARAOKE_MAX_TEXT           256
#endif

#ifndef KARAOKE_MAX_PATH
#define KARAOKE_MAX_PATH           256
#endif

#ifndef KARAOKE_MAX_SEGMENTS
#define KARAOKE_MAX_SEGMENTS       128
#endif
LV_FONT_DECLARE(lv_font_Regular_40)
#define KARAOKE_FONT               (&lv_font_Regular_40)
#define KARAOKE_LINE_SLOT_TOP      0
#define KARAOKE_LINE_SLOT_BOTTOM   1

typedef struct {
    uint32_t offset_ms;
    uint32_t duration_ms;
    uint16_t byte_start;
    uint16_t byte_len;
    int32_t start_px;
    int32_t width_px;
} karaoke_char_seg_t;

typedef struct {
    uint32_t start_ms;
    uint32_t duration_ms;
    char text[KARAOKE_MAX_TEXT];
    uint16_t seg_count;
    karaoke_char_seg_t segs[KARAOKE_MAX_SEGMENTS];
    int32_t total_width_px;
    int32_t text_height_px;
} karaoke_sub_line_t;

typedef struct {
    char mv_url[KARAOKE_MAX_PATH];
    char subtitle_path[KARAOKE_MAX_PATH];
} karaoke_song_t;

#ifndef KARAOKE_LINE_TRACK_WIDTH
#define KARAOKE_LINE_TRACK_WIDTH      1280
#endif

typedef struct {
    lv_obj_t *root;
    lv_obj_t *blue_layer;
    lv_obj_t *blue_label;
    lv_obj_t *white_clip;
    lv_obj_t *white_label;

    lv_align_t align;
    lv_coord_t ofs_x;
    lv_coord_t ofs_y;
    lv_align_t root_align;
    lv_coord_t text_margin_x;
    uint8_t text_anchor_right;

    int32_t text_x;
    int32_t text_w;
    int32_t text_h;
    int16_t bound_line_idx;
} karaoke_line_widget_t;

typedef struct {
    lv_obj_t *parent;
    lv_obj_t *btn_play;
    lv_obj_t *btn_pause;
    lv_obj_t *status_label;
    lv_obj_t *measure_label;

    bool is_playing;
    bool song_loaded;
    bool render_enabled;
    bool async_refresh_pending;

    uint32_t play_ms;

    karaoke_song_t queue[KARAOKE_MAX_QUEUE];
    uint16_t q_head;
    uint16_t q_tail;
    uint16_t q_count;

    karaoke_song_t current_song;

    karaoke_sub_line_t lines[KARAOKE_MAX_LINES];
    uint16_t line_count;
    int16_t current_idx;
    int16_t shown_top_idx;
    int16_t shown_bottom_idx;

    karaoke_line_widget_t top_line;
    karaoke_line_widget_t bottom_line;

    pthread_mutex_t lock;
    int lock_init;
} karaoke_player_t;

static karaoke_player_t g_karaoke;

static void karaoke_line_widget_bind_line(karaoke_line_widget_t *w,
                                          const karaoke_sub_line_t *line,
                                          int16_t line_idx);
static int16_t karaoke_pick_slot_line_idx(const karaoke_player_t *player, int slot);
static void karaoke_refresh_line_slot(karaoke_line_widget_t *w,
                                      karaoke_player_t *player,
                                      int16_t target_idx,
                                      int16_t *shown_idx,
                                      const char *slot_name);
static void karaoke_update_line_reveal(karaoke_line_widget_t *w,
                                       const karaoke_player_t *player,
                                       int16_t line_idx,
                                       const char *slot_name);
static void karaoke_async_refresh_cb(void *data);
static void karaoke_line_widget_apply_position_config(karaoke_line_widget_t *w,
                                                      lv_align_t align,
                                                      lv_coord_t ofs_x,
                                                      lv_coord_t ofs_y);

static void karaoke_lock(karaoke_player_t *player)
{
    if (player != NULL && player->lock_init)
    {
        pthread_mutex_lock(&player->lock);
    }
}

static void karaoke_unlock(karaoke_player_t *player)
{
    if (player != NULL && player->lock_init)
    {
        pthread_mutex_unlock(&player->lock);
    }
}

static void karaoke_trim_line_end(char *s)
{
    size_t len;

    if(s == NULL) return;

    len = strlen(s);
    while(len > 0) {
        char c = s[len - 1];
        if(c == '\r' || c == '\n' || c == ' ' || c == '\t') {
            s[len - 1] = '\0';
            len--;
        } else {
            break;
        }
    }
}

static uint8_t karaoke_utf8_char_len(const char *s)
{
    unsigned char c;

    if(s == NULL || *s == '\0') return 0;

    c = (unsigned char)s[0];
    if((c & 0x80) == 0x00) return 1;
    if((c & 0xE0) == 0xC0) return 2;
    if((c & 0xF0) == 0xE0) return 3;
    if((c & 0xF8) == 0xF0) return 4;
    return 1;
}

static bool karaoke_parse_subtitle_line(const char *src, karaoke_sub_line_t *out)
{
    unsigned start_ms = 0;
    unsigned duration_ms = 0;
    const char *p;
    size_t text_len = 0;

    if(src == NULL || out == NULL) return false;

    memset(out, 0, sizeof(*out));

    if(sscanf(src, "[%u,%u]", &start_ms, &duration_ms) != 2) {
        return false;
    }

    p = strchr(src, ']');
    if(p == NULL) return false;
    p++;

    out->start_ms = (uint32_t)start_ms;
    out->duration_ms = (uint32_t)duration_ms;

    while(*p != '\0' && out->seg_count < KARAOKE_MAX_SEGMENTS) {
        if(*p == '<') {
            unsigned off = 0;
            unsigned dur = 0;
            unsigned unused = 0;
            const char *gt = strchr(p, '>');
            uint8_t ch_len;
            karaoke_char_seg_t *seg;

            if(gt == NULL) break;

            if(sscanf(p, "<%u,%u,%u>", &off, &dur, &unused) != 3) {
                p = gt + 1;
                continue;
            }

            p = gt + 1;
            if(*p == '\0') break;

            ch_len = karaoke_utf8_char_len(p);
            if(ch_len == 0) break;

            if(text_len + ch_len >= sizeof(out->text)) {
                KARAOKE_LOG("subtitle text too long, truncate line start=%u", (unsigned)out->start_ms);
                break;
            }

            seg = &out->segs[out->seg_count];
            memset(seg, 0, sizeof(*seg));
            seg->offset_ms = (uint32_t)off;
            seg->duration_ms = (uint32_t)dur;
            seg->byte_start = (uint16_t)text_len;
            seg->byte_len = ch_len;

            memcpy(&out->text[text_len], p, ch_len);
            text_len += ch_len;
            out->text[text_len] = '\0';

            out->seg_count++;
            p += ch_len;
            continue;
        }

        if(text_len + 1 < sizeof(out->text)) {
            out->text[text_len++] = *p;
            out->text[text_len] = '\0';
        }
        p++;
    }

    karaoke_trim_line_end(out->text);
    return (out->text[0] != '\0');
}

static void karaoke_set_status(karaoke_player_t *player, const char *txt)
{
    // if(player == NULL || player->status_label == NULL) return;
    // lv_label_set_text(player->status_label, txt ? txt : "");
}

static void karaoke_clear_lines(karaoke_player_t *player)
{
    if(player == NULL) return;
    memset(player->lines, 0, sizeof(player->lines));
    player->line_count = 0;
    player->current_idx = -1;
    player->shown_top_idx = -2;
    player->shown_bottom_idx = -2;
}

static void karaoke_disable_render(karaoke_player_t *player, const char *reason)
{
    if(player == NULL) return;

    player->render_enabled = false;
    karaoke_clear_lines(player);
    karaoke_line_widget_bind_line(&player->top_line, NULL, -1);
    karaoke_line_widget_bind_line(&player->bottom_line, NULL, -1);

    KARAOKE_LOG("render disabled: %s", reason ? reason : "unknown");
}

static bool karaoke_queue_push(karaoke_player_t *player, const char *mv_url, const char *subtitle_path)
{
    karaoke_song_t *slot;

    if(player == NULL || mv_url == NULL) {
        KARAOKE_LOG("queue_push failed: invalid args");
        return false;
    }

    if(player->q_count >= KARAOKE_MAX_QUEUE) {
        KARAOKE_LOG("queue full, push failed: mv_url=%s, subtitle=%s", mv_url, subtitle_path ? subtitle_path : "");
        return false;
    }

    slot = &player->queue[player->q_tail];
    memset(slot, 0, sizeof(*slot));
    strncpy(slot->mv_url, mv_url, sizeof(slot->mv_url) - 1);
    strncpy(slot->subtitle_path, subtitle_path ? subtitle_path : "", sizeof(slot->subtitle_path) - 1);

    player->q_tail = (player->q_tail + 1U) % KARAOKE_MAX_QUEUE;
    player->q_count++;

    KARAOKE_LOG("enqueue ok: mv_url=%s, subtitle=%s, q_count=%u",
                slot->mv_url, slot->subtitle_path, (unsigned)player->q_count);
    return true;
}

static bool karaoke_queue_pop(karaoke_player_t *player, karaoke_song_t *out_song)
{
    karaoke_song_t *slot;

    if(player == NULL || out_song == NULL) return false;
    if(player->q_count == 0) return false;

    slot = &player->queue[player->q_head];
    *out_song = *slot;

    player->q_head = (player->q_head + 1U) % KARAOKE_MAX_QUEUE;
    player->q_count--;

    KARAOKE_LOG("dequeue ok: mv_url=%s, subtitle=%s, q_count=%u",
                out_song->mv_url, out_song->subtitle_path, (unsigned)player->q_count);
    return true;
}

static int32_t karaoke_measure_text_width(karaoke_player_t *player, const char *txt)
{
    if(player == NULL || player->measure_label == NULL || txt == NULL) return 0;

    lv_label_set_text(player->measure_label, txt);
    lv_obj_update_layout(player->measure_label);
    return lv_obj_get_width(player->measure_label);
}

static int32_t karaoke_measure_text_height(karaoke_player_t *player, const char *txt)
{
    if(player == NULL || player->measure_label == NULL || txt == NULL) return 0;

    lv_label_set_text(player->measure_label, txt);
    lv_obj_update_layout(player->measure_label);
    return lv_obj_get_height(player->measure_label);
}

static void karaoke_precompute_line_metrics(karaoke_player_t *player, karaoke_sub_line_t *line)
{
    uint16_t i;
    char prefix[KARAOKE_MAX_TEXT];
    char one[KARAOKE_MAX_TEXT];

    if(player == NULL || line == NULL) return;

    memset(prefix, 0, sizeof(prefix));
    memset(one, 0, sizeof(one));

    line->total_width_px = karaoke_measure_text_width(player, line->text);
    line->text_height_px = karaoke_measure_text_height(player, line->text);
    if(line->text_height_px <= 0) line->text_height_px = 1;
    if(line->total_width_px <= 0) line->total_width_px = 1;

    for(i = 0; i < line->seg_count; i++) {
        karaoke_char_seg_t *seg = &line->segs[i];

        memset(prefix, 0, sizeof(prefix));
        memset(one, 0, sizeof(one));

        if(seg->byte_start < sizeof(prefix)) {
            memcpy(prefix, line->text, seg->byte_start);
            prefix[seg->byte_start] = '\0';
        }

        if(seg->byte_len < sizeof(one) && (size_t)(seg->byte_start + seg->byte_len) <= strlen(line->text)) {
            memcpy(one, &line->text[seg->byte_start], seg->byte_len);
            one[seg->byte_len] = '\0';
        }

        seg->start_px = karaoke_measure_text_width(player, prefix);
        seg->width_px = karaoke_measure_text_width(player, one);
        if(seg->width_px <= 0) seg->width_px = 1;
    }
}

static bool karaoke_load_subtitle_file(karaoke_player_t *player, const char *path)
{
    FILE *fp;
    char buf[2048];
    uint16_t count = 0;

    if(player == NULL || path == NULL) return false;

    fp = fopen(path, "rb");
    if(fp == NULL) {
        KARAOKE_LOG("open subtitle failed: %s", path);
        return false;
    }

    karaoke_clear_lines(player);

    while(fgets(buf, sizeof(buf), fp) != NULL) {
        karaoke_sub_line_t line;
        karaoke_trim_line_end(buf);

        if(karaoke_parse_subtitle_line(buf, &line)) {
            if(count < KARAOKE_MAX_LINES) {
                player->lines[count] = line;
                karaoke_precompute_line_metrics(player, &player->lines[count]);
                count++;
            } else {
                KARAOKE_LOG("subtitle line overflow, ignore rest");
                break;
            }
        }
    }

    fclose(fp);

    player->line_count = count;
    player->current_idx = -1;
    player->shown_top_idx = -2;
    player->shown_bottom_idx = -2;
    player->play_ms = 0;

    if(player->line_count == 0) {
        KARAOKE_LOG("subtitle parsed 0 lines: %s", path);
        return false;
    }

    KARAOKE_LOG("subtitle loaded ok: path=%s, line_count=%u, first_start=%u ms, first_text=%s",
                path,
                (unsigned)player->line_count,
                (unsigned)player->lines[0].start_ms,
                player->lines[0].text);
    return true;
}

static bool karaoke_load_next_song(karaoke_player_t *player)
{
    karaoke_song_t song;

    if(player == NULL) return false;
    memset(&song, 0, sizeof(song));

    if(!karaoke_queue_pop(player, &song)) {
        KARAOKE_LOG("load_next_song failed: queue empty");
        return false;
    }

    player->current_song = song;
    player->song_loaded = true;
    player->play_ms = 0;

    if(song.subtitle_path[0] == '\0') {
        karaoke_disable_render(player, "subtitle empty, skip renderer");
        KARAOKE_LOG("song loaded without subtitle: mv_url=%s", player->current_song.mv_url);
        return true;
    }

    if(!karaoke_load_subtitle_file(player, song.subtitle_path)) {
        karaoke_disable_render(player, "subtitle load failed, fallback no-render");
        KARAOKE_LOG("song loaded but subtitle invalid: mv_url=%s, subtitle=%s",
                    song.mv_url, song.subtitle_path);
        return true;
    }

    player->render_enabled = true;
    KARAOKE_LOG("song loaded with subtitle: mv_url=%s, subtitle=%s",
                player->current_song.mv_url,
                player->current_song.subtitle_path);
    return true;
}

static void karaoke_apply_obj_base_style(lv_obj_t *obj)
{
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_outline_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

/*
 * 白字层和蓝字层必须使用完全一致的字体/间距/尺寸基准，
 * 否则裁剪推进时，底层蓝字会在底边露出一条“蓝边”。
 */
static void karaoke_apply_label_base_style(lv_obj_t *label, lv_color_t color)
{
    if(label == NULL) return;

    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_outline_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(label, 0, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_font(label, KARAOKE_FONT, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_letter_space(label, 0, 0);
    lv_obj_set_style_text_line_space(label, 0, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
}

static lv_align_t karaoke_root_align_from_text_align(lv_align_t align)
{
    switch(align) {
        case LV_ALIGN_TOP_RIGHT: return LV_ALIGN_TOP_LEFT;
        case LV_ALIGN_BOTTOM_RIGHT: return LV_ALIGN_BOTTOM_LEFT;
        case LV_ALIGN_RIGHT_MID: return LV_ALIGN_LEFT_MID;
        default: return align;
    }
}

static uint8_t karaoke_is_right_text_anchor(lv_align_t align)
{
    switch(align) {
        case LV_ALIGN_TOP_RIGHT:
        case LV_ALIGN_BOTTOM_RIGHT:
        case LV_ALIGN_RIGHT_MID:
            return 1;
        default:
            return 0;
    }
}

static lv_coord_t karaoke_abs_coord(lv_coord_t v)
{
    return (v < 0) ? (lv_coord_t)(-v) : v;
}

static int32_t karaoke_calc_text_origin_x(const karaoke_line_widget_t *w)
{
    int32_t x;

    if(w == NULL) return 0;

    if(w->text_anchor_right) {
        x = (int32_t)KARAOKE_LINE_TRACK_WIDTH - (int32_t)w->text_margin_x - w->text_w;
    } else {
        x = (int32_t)w->text_margin_x;
    }

    if(x < 0) x = 0;
    return x;
}


static void karaoke_line_widget_apply_position_config(karaoke_line_widget_t *w,
                                                      lv_align_t align,
                                                      lv_coord_t ofs_x,
                                                      lv_coord_t ofs_y)
{
    if(w == NULL) return;

    w->align = align;
    w->ofs_x = ofs_x;
    w->ofs_y = ofs_y;
    w->root_align = karaoke_root_align_from_text_align(align);
    w->text_margin_x = karaoke_abs_coord(ofs_x);
    w->text_anchor_right = karaoke_is_right_text_anchor(align);

    if(w->text_w > 0) {
        w->text_x = karaoke_calc_text_origin_x(w);
    } else {
        w->text_x = 0;
    }

    if(w->root != NULL) {
        lv_obj_align(w->root, w->root_align, 0, w->ofs_y);
    }

    if(w->white_clip != NULL) {
        lv_obj_set_pos(w->white_clip, w->text_x, 0);
    }

    if(w->blue_layer != NULL) {
        lv_obj_set_pos(w->blue_layer, w->text_x, 0);
    }
}

static void karaoke_line_widget_create(karaoke_line_widget_t *w,
                                       lv_obj_t *parent,
                                       lv_align_t align,
                                       lv_coord_t ofs_x,
                                       lv_coord_t ofs_y)
{
    memset(w, 0, sizeof(*w));
    w->bound_line_idx = -1;

    w->root = lv_obj_create(parent);
    karaoke_apply_obj_base_style(w->root);
    lv_obj_set_size(w->root, 10, 10);
    lv_obj_add_flag(w->root, LV_OBJ_FLAG_HIDDEN);

    /*
     * 严格结构：
     * 1. 先创建 A*B 白字层
     * 2. 再创建 A*B 蓝字层
     * 3. 两层位置完全一样
     * 4. 两层文字都左对齐
     */
    w->white_clip = lv_obj_create(w->root);
    karaoke_apply_obj_base_style(w->white_clip);
    lv_obj_set_size(w->white_clip, 10, 10);

    w->white_label = lv_label_create(w->white_clip);
    karaoke_apply_label_base_style(w->white_label, lv_color_hex(KARAOKE_TEXT_COLOR_HIGHLIGHT_HEX));
    lv_obj_set_size(w->white_label, 10, 10);
    lv_obj_set_pos(w->white_label, 0, 0);

    w->blue_layer = lv_obj_create(w->root);
    karaoke_apply_obj_base_style(w->blue_layer);
    lv_obj_set_size(w->blue_layer, 10, 10);

    w->blue_label = lv_label_create(w->blue_layer);
    karaoke_apply_label_base_style(w->blue_label, lv_color_hex(KARAOKE_TEXT_COLOR_BASE_HEX));
    lv_obj_set_size(w->blue_label, 10, 10);
    lv_obj_set_pos(w->blue_label, 0, 0);

    karaoke_line_widget_apply_position_config(w, align, ofs_x, ofs_y);
}

static void karaoke_line_widget_bind_line(karaoke_line_widget_t *w,
                                          const karaoke_sub_line_t *line,
                                          int16_t line_idx)
{
    int32_t actual_blue_w;
    int32_t actual_blue_h;
    int32_t actual_white_w;
    int32_t actual_white_h;

    if(w == NULL) return;

    if(line == NULL) {
        lv_obj_add_flag(w->root, LV_OBJ_FLAG_HIDDEN);
        w->text_x = 0;
        w->text_w = 0;
        w->text_h = 0;
        w->bound_line_idx = -1;
        return;
    }

    if(w->bound_line_idx == line_idx) {
        return;
    }

    lv_label_set_text(w->white_label, line->text);
    lv_label_set_text(w->blue_label, line->text);

    lv_obj_set_size(w->white_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_size(w->blue_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_update_layout(w->white_label);
    lv_obj_update_layout(w->blue_label);

    actual_white_w = lv_obj_get_width(w->white_label);
    actual_white_h = lv_obj_get_height(w->white_label);
    actual_blue_w = lv_obj_get_width(w->blue_label);
    actual_blue_h = lv_obj_get_height(w->blue_label);

    w->text_w = actual_white_w;
    if(actual_blue_w > w->text_w) {
        w->text_w = actual_blue_w;
    }
    if(line->total_width_px > w->text_w) {
        w->text_w = line->total_width_px;
    }
    if(w->text_w <= 0) {
        w->text_w = 1;
    }

    w->text_h = actual_white_h;
    if(actual_blue_h > w->text_h) {
        w->text_h = actual_blue_h;
    }
    if(line->text_height_px > w->text_h) {
        w->text_h = line->text_height_px;
    }
    if((int32_t)KARAOKE_FONT->line_height > w->text_h) {
        w->text_h = KARAOKE_FONT->line_height;
    }
    if(w->text_h <= 0) {
        w->text_h = 1;
    }

    w->bound_line_idx = line_idx;
    w->text_x = karaoke_calc_text_origin_x(w);

    lv_obj_set_size(w->root, KARAOKE_LINE_TRACK_WIDTH, w->text_h);

    /* A*B 白字层 */
    lv_obj_set_size(w->white_clip, w->text_w, w->text_h);
    lv_obj_set_pos(w->white_clip, w->text_x, 0);
    lv_obj_set_size(w->white_label, w->text_w, w->text_h);
    lv_obj_set_pos(w->white_label, 0, 0);

    /* A*B 蓝字层，和白字层同位置、同尺寸 */
    lv_obj_set_size(w->blue_layer, w->text_w, w->text_h);
    lv_obj_set_pos(w->blue_layer, w->text_x, 0);
    lv_obj_set_size(w->blue_label, w->text_w, w->text_h);
    lv_obj_set_pos(w->blue_label, 0, 0);

    lv_obj_align(w->root, w->root_align, 0, w->ofs_y);
    lv_obj_clear_flag(w->root, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(w->white_clip, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(w->blue_layer, LV_OBJ_FLAG_HIDDEN);

    KARAOKE_LOG("slot bind: idx=%d, track_w=%d, text_w=%d, text_h=%d, text_x=%d, anchor_right=%d",
                line_idx,
                (int)KARAOKE_LINE_TRACK_WIDTH,
                (int)w->text_w,
                (int)w->text_h,
                (int)w->text_x,
                (int)w->text_anchor_right);
}

static void karaoke_line_widget_set_reveal_px(karaoke_line_widget_t *w, int32_t reveal_px)
{
    if(w == NULL) return;
    if(lv_obj_has_flag(w->root, LV_OBJ_FLAG_HIDDEN)) return;
    if(w->text_w <= 0 || w->text_h <= 0) return;

    if(reveal_px < 0) reveal_px = 0;
    if(reveal_px > w->text_w) reveal_px = w->text_w;

    if(reveal_px <= 0) {
        lv_obj_add_flag(w->blue_layer, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_clear_flag(w->blue_layer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(w->blue_layer, w->text_x, 0);
    lv_obj_set_size(w->blue_layer, reveal_px, w->text_h);
    lv_obj_set_pos(w->blue_label, 0, 0);
}

static int32_t karaoke_calc_reveal_px(const karaoke_sub_line_t *line, uint32_t line_elapsed_ms)
{
    uint16_t i;
    int32_t reveal_px = 0;

    if(line == NULL) return 0;

    if(line->seg_count == 0) {
        if(line->duration_ms == 0) return line->total_width_px;
        if(line_elapsed_ms >= line->duration_ms) return line->total_width_px;
        return (int32_t)(((uint64_t)line->total_width_px * line_elapsed_ms) / line->duration_ms);
    }

    for(i = 0; i < line->seg_count; i++) {
        const karaoke_char_seg_t *seg = &line->segs[i];
        uint32_t seg_start = seg->offset_ms;
        uint32_t seg_end = seg->offset_ms + seg->duration_ms;

        if(line_elapsed_ms < seg_start) {
            break;
        }

        if(seg->duration_ms == 0) {
            reveal_px = seg->start_px + seg->width_px;
            continue;
        }

        if(line_elapsed_ms >= seg_end) {
            reveal_px = seg->start_px + seg->width_px;
            continue;
        }

        reveal_px = seg->start_px + (int32_t)(((uint64_t)seg->width_px * (line_elapsed_ms - seg_start)) / seg->duration_ms);
        return reveal_px;
    }

    if(line_elapsed_ms >= line->duration_ms) {
        return line->total_width_px;
    }

    if(reveal_px < 0) reveal_px = 0;
    if(reveal_px > line->total_width_px) reveal_px = line->total_width_px;
    return reveal_px;
}

static void karaoke_show_line_by_index(karaoke_line_widget_t *w,
                                       karaoke_player_t *player,
                                       int16_t idx)
{
    if(w == NULL || player == NULL) return;

    if(idx < 0 || idx >= (int16_t)player->line_count) {
        karaoke_line_widget_bind_line(w, NULL, -1);
        return;
    }

    karaoke_line_widget_bind_line(w, &player->lines[idx], idx);
}

static int16_t karaoke_pick_slot_line_idx(const karaoke_player_t *player, int slot)
{
    int16_t idx_a;
    int16_t idx_b;

    if(player == NULL) return -1;

    idx_a = player->current_idx;
    idx_b = player->current_idx + 1;

    if(slot == KARAOKE_LINE_SLOT_TOP) {
        if(idx_a >= 0 && idx_a < (int16_t)player->line_count && ((idx_a & 1) == 0)) {
            return idx_a;
        }
        if(idx_b >= 0 && idx_b < (int16_t)player->line_count && ((idx_b & 1) == 0)) {
            return idx_b;
        }
        return -1;
    }

    if(idx_a >= 0 && idx_a < (int16_t)player->line_count && ((idx_a & 1) == 1)) {
        return idx_a;
    }
    if(idx_b >= 0 && idx_b < (int16_t)player->line_count && ((idx_b & 1) == 1)) {
        return idx_b;
    }
    return -1;
}

static void karaoke_refresh_line_slot(karaoke_line_widget_t *w,
                                      karaoke_player_t *player,
                                      int16_t target_idx,
                                      int16_t *shown_idx,
                                      const char *slot_name)
{
    if(w == NULL || player == NULL || shown_idx == NULL) return;

    if(*shown_idx != target_idx) {
        karaoke_show_line_by_index(w, player, target_idx);
        *shown_idx = target_idx;

        if(target_idx >= 0 && target_idx < (int16_t)player->line_count) {
            KARAOKE_LOG("slot switch: %s <= idx=%d, start=%u, duration=%u, text=%s",
                        slot_name ? slot_name : "unknown",
                        target_idx,
                        (unsigned)player->lines[target_idx].start_ms,
                        (unsigned)player->lines[target_idx].duration_ms,
                        player->lines[target_idx].text);
        }
    }
}

static void karaoke_update_line_reveal(karaoke_line_widget_t *w,
                                       const karaoke_player_t *player,
                                       int16_t line_idx,
                                       const char *slot_name)
{
    const karaoke_sub_line_t *line;
    uint32_t line_elapsed_ms = 0;
    int32_t reveal_px;

    if(w == NULL || player == NULL) return;

    if(line_idx < 0 || line_idx >= (int16_t)player->line_count) {
        karaoke_line_widget_set_reveal_px(w, 0);
        return;
    }

    line = &player->lines[line_idx];
    if(player->play_ms > line->start_ms) {
        line_elapsed_ms = player->play_ms - line->start_ms;
    }

    reveal_px = karaoke_calc_reveal_px(line, line_elapsed_ms);
    karaoke_line_widget_set_reveal_px(w, reveal_px);

    (void)slot_name;
}

static void karaoke_refresh_display(karaoke_player_t *player)
{
    int16_t top_idx;
    int16_t bottom_idx;

    if(player == NULL) return;
    if(!player->song_loaded || !player->render_enabled || player->line_count == 0) {
        karaoke_line_widget_bind_line(&player->top_line, NULL, -1);
        karaoke_line_widget_bind_line(&player->bottom_line, NULL, -1);
        return;
    }

    while((player->current_idx + 1) < (int16_t)player->line_count &&
          player->play_ms >= player->lines[player->current_idx + 1].start_ms) {
        player->current_idx++;
        KARAOKE_LOG("line switch: idx=%d, start=%u, duration=%u, seg_count=%u, text=%s",
                    player->current_idx,
                    (unsigned)player->lines[player->current_idx].start_ms,
                    (unsigned)player->lines[player->current_idx].duration_ms,
                    (unsigned)player->lines[player->current_idx].seg_count,
                    player->lines[player->current_idx].text);
    }

    top_idx = karaoke_pick_slot_line_idx(player, KARAOKE_LINE_SLOT_TOP);
    bottom_idx = karaoke_pick_slot_line_idx(player, KARAOKE_LINE_SLOT_BOTTOM);

    karaoke_refresh_line_slot(&player->top_line, player, top_idx, &player->shown_top_idx, "top_left");
    karaoke_refresh_line_slot(&player->bottom_line, player, bottom_idx, &player->shown_bottom_idx, "bottom_right");

    karaoke_update_line_reveal(&player->top_line, player, top_idx, "top_left");
    karaoke_update_line_reveal(&player->bottom_line, player, bottom_idx, "bottom_right");
}

static void karaoke_async_refresh_cb(void *data)
{
    karaoke_player_t *player = (karaoke_player_t *)data;

    if (player == NULL)
    {
        return;
    }

    karaoke_lock(player);
    player->async_refresh_pending = false;
    if (player->render_enabled && player->song_loaded)
    {
        karaoke_refresh_display(player);
    }
    else
    {
        karaoke_line_widget_bind_line(&player->top_line, NULL, -1);
        karaoke_line_widget_bind_line(&player->bottom_line, NULL, -1);
    }
    karaoke_unlock(player);
}

static void karaoke_btn_play_event_cb(lv_event_t *e)
{
    (void)e;
    karaoke_demo_play();
}

static void karaoke_btn_pause_event_cb(lv_event_t *e)
{
    (void)e;
    karaoke_demo_pause();
}

static lv_obj_t *karaoke_create_btn(lv_obj_t *parent, const char *txt)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_t *lab = lv_label_create(btn);

    lv_obj_set_size(btn, 100, 48);
    lv_label_set_text(lab, txt);
    lv_obj_center(lab);

    return btn;
}

void karaoke_demo_open(lv_obj_t *parent)
{
    karaoke_player_t *player = &g_karaoke;

    if (player->lock_init)
    {
        pthread_mutex_destroy(&player->lock);
        player->lock_init = 0;
    }

    memset(player, 0, sizeof(*player));
    player->parent = parent;
    player->current_idx = -1;
    player->shown_top_idx = -2;
    player->shown_bottom_idx = -2;
    if (pthread_mutex_init(&player->lock, NULL) == 0)
    {
        player->lock_init = 1;
    }

    player->measure_label = lv_label_create(parent);
    karaoke_apply_label_base_style(player->measure_label, lv_color_hex(KARAOKE_TEXT_COLOR_HIGHLIGHT_HEX));
    lv_obj_set_style_text_opa(player->measure_label, LV_OPA_0, 0);
    lv_obj_set_size(player->measure_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(player->measure_label, -4096, -4096);
    lv_label_set_text(player->measure_label, "测");

    karaoke_line_widget_create(&player->top_line, parent, KARAOKE_TOP_LINE_ALIGN, KARAOKE_TOP_LINE_OFS_X, KARAOKE_TOP_LINE_OFS_Y);
    karaoke_line_widget_create(&player->bottom_line, parent, KARAOKE_BOTTOM_LINE_ALIGN, KARAOKE_BOTTOM_LINE_OFS_X, KARAOKE_BOTTOM_LINE_OFS_Y);

    // player->btn_play = karaoke_create_btn(parent, "播放");
    // lv_obj_align(player->btn_play, LV_ALIGN_BOTTOM_MID, -70, -15);
    // lv_obj_add_event_cb(player->btn_play, karaoke_btn_play_event_cb, LV_EVENT_CLICKED, player);

    // player->btn_pause = karaoke_create_btn(parent, "暂停");
    // lv_obj_align(player->btn_pause, LV_ALIGN_BOTTOM_MID, 70, -15);
    // lv_obj_add_event_cb(player->btn_pause, karaoke_btn_pause_event_cb, LV_EVENT_CLICKED, player);

    KARAOKE_LOG("demo open ok, external clock mode, track_w=%d, top=(align=%d,x=%d,y=%d), bottom=(align=%d,x=%d,y=%d), color_base=0x%06X, color_highlight=0x%06X",
                (int)KARAOKE_LINE_TRACK_WIDTH,
                (int)KARAOKE_TOP_LINE_ALIGN,
                (int)KARAOKE_TOP_LINE_OFS_X,
                (int)KARAOKE_TOP_LINE_OFS_Y,
                (int)KARAOKE_BOTTOM_LINE_ALIGN,
                (int)KARAOKE_BOTTOM_LINE_OFS_X,
                (int)KARAOKE_BOTTOM_LINE_OFS_Y,
                (unsigned)KARAOKE_TEXT_COLOR_BASE_HEX,
                (unsigned)KARAOKE_TEXT_COLOR_HIGHLIGHT_HEX);
}

bool karaoke_demo_enqueue(const char *mv_url, const char *subtitle_path)
{
    karaoke_player_t *player = &g_karaoke;
    bool ret;

    karaoke_lock(player);
    ret = karaoke_queue_push(player, mv_url, subtitle_path);
    karaoke_unlock(player);
    return ret;
}

bool karaoke_demo_bind_subtitle(const char *subtitle_path)
{
    karaoke_player_t *player = &g_karaoke;
    bool ret = true;

    if(player->parent == NULL) {
        KARAOKE_LOG("bind_subtitle ignored: demo not open");
        return false;
    }

    karaoke_lock(player);
    player->play_ms = 0;
    player->song_loaded = true;
    player->is_playing = false;
    memset(&player->current_song, 0, sizeof(player->current_song));
    if(subtitle_path != NULL) {
        strncpy(player->current_song.subtitle_path, subtitle_path, sizeof(player->current_song.subtitle_path) - 1);
    }

    if(subtitle_path == NULL || subtitle_path[0] == '\0') {
        karaoke_disable_render(player, "bind_subtitle empty, skip renderer");
        // karaoke_set_status(player, "IDLE");
        karaoke_unlock(player);
        karaoke_demo_request_refresh_async();
        return true;
    }

    if(!karaoke_load_subtitle_file(player, subtitle_path)) {
        karaoke_disable_render(player, "bind_subtitle load failed, fallback no-render");
        karaoke_set_status(player, "IDLE");
        karaoke_unlock(player);
        karaoke_demo_request_refresh_async();
        return true;
    }

    player->render_enabled = true;
    karaoke_set_status(player, "READY");
    KARAOKE_LOG("bind_subtitle success: %s", subtitle_path);
    karaoke_unlock(player);
    karaoke_demo_request_refresh_async();
    return ret;
}

bool karaoke_demo_is_render_enabled(void)
{
    bool enabled;
    karaoke_player_t *player = &g_karaoke;

    karaoke_lock(player);
    enabled = player->render_enabled;
    karaoke_unlock(player);
    return enabled;
}

void karaoke_demo_play(void)
{
    karaoke_player_t *player = &g_karaoke;

    karaoke_lock(player);
    if(!player->song_loaded) {
        if(!karaoke_load_next_song(player)) {
            KARAOKE_LOG("play ignored: no song in queue");
            karaoke_set_status(player, "IDLE");
            karaoke_unlock(player);
            karaoke_demo_request_refresh_async();
            return;
        }
    }

    player->is_playing = true;
    // karaoke_set_status(player, player->render_enabled ? "PLAYING" : "IDLE");
    KARAOKE_LOG("play start: subtitle=%s, render_enabled=%d, play_ms=%u",
                player->current_song.subtitle_path,
                player->render_enabled ? 1 : 0,
                (unsigned)player->play_ms);
    karaoke_unlock(player);
    karaoke_demo_request_refresh_async();
}

void karaoke_demo_pause(void)
{
    karaoke_player_t *player = &g_karaoke;

    karaoke_lock(player);
    player->is_playing = false;
    // karaoke_set_status(player, "PAUSED");
    KARAOKE_LOG("pause: subtitle=%s, render_enabled=%d, play_ms=%u",
                player->current_song.subtitle_path,
                player->render_enabled ? 1 : 0,
                (unsigned)player->play_ms);
    karaoke_unlock(player);
}

void karaoke_demo_stop(void)
{
    karaoke_player_t *player = &g_karaoke;

    if(player->parent == NULL) {
        KARAOKE_LOG("stop ignored: demo not open");
        return;
    }

    karaoke_lock(player);
    player->is_playing = false;
    player->song_loaded = false;
    player->play_ms = 0;
    memset(&player->current_song, 0, sizeof(player->current_song));
    karaoke_disable_render(player, "stop");
    karaoke_set_status(player, "IDLE");
    karaoke_unlock(player);
    karaoke_demo_request_refresh_async();
    KARAOKE_LOG("stop");
}

bool karaoke_demo_set_positions(lv_align_t top_align,
                                lv_coord_t top_ofs_x,
                                lv_coord_t top_ofs_y,
                                lv_align_t bottom_align,
                                lv_coord_t bottom_ofs_x,
                                lv_coord_t bottom_ofs_y)
{
    karaoke_player_t *player = &g_karaoke;

    if(player->parent == NULL) {
        KARAOKE_LOG("set_positions ignored: demo not open");
        return false;
    }

    karaoke_lock(player);
    karaoke_line_widget_apply_position_config(&player->top_line,
                                              top_align,
                                              top_ofs_x,
                                              top_ofs_y);
    karaoke_line_widget_apply_position_config(&player->bottom_line,
                                              bottom_align,
                                              bottom_ofs_x,
                                              bottom_ofs_y);
    karaoke_unlock(player);

    karaoke_demo_request_refresh_async();
    KARAOKE_LOG("set_positions ok: top=(align=%d,x=%d,y=%d), bottom=(align=%d,x=%d,y=%d)",
                (int)top_align,
                (int)top_ofs_x,
                (int)top_ofs_y,
                (int)bottom_align,
                (int)bottom_ofs_x,
                (int)bottom_ofs_y);
    return true;
}

void karaoke_demo_set_time_ms(uint32_t play_ms)
{
    karaoke_player_t *player = &g_karaoke;

    karaoke_lock(player);
    player->play_ms = play_ms;
    karaoke_unlock(player);
}

void karaoke_demo_request_refresh_async(void)
{
    karaoke_player_t *player = &g_karaoke;
    int need_post = 0;

    if (player->parent == NULL)
    {
        return;
    }

    karaoke_lock(player);
    if (!player->async_refresh_pending)
    {
        player->async_refresh_pending = true;
        need_post = 1;
    }
    karaoke_unlock(player);

    if (need_post)
    {
        lv_async_call(karaoke_async_refresh_cb, player);
    }
}
