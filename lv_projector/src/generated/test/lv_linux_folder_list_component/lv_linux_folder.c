#include "lv_linux_folder.h"

LV_FONT_DECLARE(lv_font_Regular_20);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#ifndef LV_SYMBOL_DIRECTORY
#define LV_SYMBOL_DIRECTORY ""
#endif
#ifndef LV_SYMBOL_FILE
#define LV_SYMBOL_FILE ""
#endif
#ifndef LV_SYMBOL_IMAGE
#define LV_SYMBOL_IMAGE ""
#endif
#ifndef LV_SYMBOL_VIDEO
#define LV_SYMBOL_VIDEO ""
#endif
#ifndef LV_SYMBOL_RIGHT
#define LV_SYMBOL_RIGHT ">"
#endif
#ifndef LV_SYMBOL_CLOSE
#define LV_SYMBOL_CLOSE "X"
#endif

#define FOLDER_LOG(fmt, ...) printf("[linux_folder] " fmt "\n", ##__VA_ARGS__)
#define FOLDER_PAGE_SIZE 8

/* 1280x800 UI layout */
#define FOLDER_ROOT_PAD      18
#define FOLDER_ROOT_GAP      10
#define FOLDER_HEADER_H      86
#define FOLDER_FOOTER_H      64
#define FOLDER_ROW_H         64
#define FOLDER_ROW_GAP       6
#define FOLDER_LIST_PAD      8
#define FOLDER_LIST_H        (FOLDER_PAGE_SIZE * FOLDER_ROW_H + \
                              (FOLDER_PAGE_SIZE - 1) * FOLDER_ROW_GAP + \
                              FOLDER_LIST_PAD * 2)

#define FOLDER_BTN_H         46
#define FOLDER_BTN_RADIUS    12
#define FOLDER_PANEL_RADIUS  16

#define COL_BG        0x101318
#define COL_PANEL     0x171B24
#define COL_ROW       0x202632
#define COL_ROW_PRESS 0x2D3748
#define COL_TEXT      0xF8FAFC
#define COL_SUBTEXT   0x9CA3AF
#define COL_LINE      0x303846

typedef struct {
    char name[NAME_MAX + 1];
    char full_path[PATH_MAX];
    bool is_dir;
} folder_item_t;

struct lv_linux_folder {
    lv_obj_t *root;
    lv_obj_t *header;
    lv_obj_t *path_label;
    lv_obj_t *list;
    lv_obj_t *footer;
    lv_obj_t *page_label;
    lv_obj_t *btn_back;
    lv_obj_t *btn_home;
    lv_obj_t *btn_prev;
    lv_obj_t *btn_next;

    lv_obj_t *preview_mask;
    lv_obj_t *preview_panel;
    lv_obj_t *preview_title;
    lv_obj_t *preview_img;
    lv_obj_t *preview_hint;
    lv_obj_t *preview_close_btn;

    char current_dir[PATH_MAX];
    char root_dir[PATH_MAX];
    char preview_path[PATH_MAX + 16];

    const char *img_prefix;

    uint8_t *preview_data;
    uint32_t preview_data_size;
    lv_img_dsc_t preview_dsc;

    uint32_t page_index;
    uint32_t page_size;
    bool show_hidden;
    bool has_next;

    lv_linux_folder_video_cb_t video_cb;
    lv_linux_folder_file_cb_t other_file_cb;
    void *user_data;
};

typedef struct {
    lv_linux_folder_t *folder;
    folder_item_t item;
} item_click_ctx_t;

static int folder_render(lv_linux_folder_t *folder);

static void safe_copy(char *dst, size_t sz, const char *src)
{
    if (!dst || sz == 0) return;
    snprintf(dst, sz, "%s", src ? src : "");
}

static int path_join(char *out, size_t out_size, const char *dir, const char *name)
{
    if (!out || !dir || !name) return -1;
    if (strcmp(dir, "/") == 0) {
        return snprintf(out, out_size, "/%s", name) >= (int)out_size ? -1 : 0;
    }
    return snprintf(out, out_size, "%s/%s", dir, name) >= (int)out_size ? -1 : 0;
}

static void get_parent_dir(char *path)
{
    size_t len;
    char *p;

    if (!path || path[0] == '\0') {
        safe_copy(path, PATH_MAX, "/");
        return;
    }

    len = strlen(path);
    while (len > 1 && path[len - 1] == '/') {
        path[--len] = '\0';
    }

    if (strcmp(path, "/") == 0) return;

    p = strrchr(path, '/');
    if (!p) {
        safe_copy(path, PATH_MAX, "/");
    } else if (p == path) {
        path[1] = '\0';
    } else {
        *p = '\0';
    }
}

static bool should_skip_name(const char *name, bool show_hidden)
{
    if (!name) return true;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return true;
    if (!show_hidden && name[0] == '.') return true;
    return false;
}

static int stat_path(const char *path, bool *is_dir)
{
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    if (is_dir) *is_dir = S_ISDIR(st.st_mode) ? true : false;
    return 0;
}

static const char *file_ext_lower(const char *name)
{
    const char *dot = strrchr(name ? name : "", '.');
    return dot ? dot + 1 : "";
}

static bool str_ieq(const char *a, const char *b)
{
    if (!a || !b) return false;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}

static bool is_image_file(const char *name)
{
    const char *ext = file_ext_lower(name);
    return str_ieq(ext, "jpg") || str_ieq(ext, "jpeg") ||
           str_ieq(ext, "png") || str_ieq(ext, "bmp");
}

static bool is_video_file(const char *name)
{
    const char *ext = file_ext_lower(name);
    return str_ieq(ext, "mp4") || str_ieq(ext, "avi") ||
           str_ieq(ext, "mkv") || str_ieq(ext, "mov") ||
           str_ieq(ext, "ts")  || str_ieq(ext, "flv") ||
           str_ieq(ext, "m4v") || str_ieq(ext, "3gp");
}

static const char *item_icon(const folder_item_t *item)
{
    if (!item) return LV_SYMBOL_FILE;
    if (item->is_dir) return LV_SYMBOL_DIRECTORY;
    if (is_image_file(item->name)) return LV_SYMBOL_IMAGE;
    if (is_video_file(item->name)) return LV_SYMBOL_VIDEO;
    return LV_SYMBOL_FILE;
}

static void build_lvgl_img_src(lv_linux_folder_t *folder, const char *linux_path,
                               char *out, size_t out_sz)
{
    if (folder && folder->img_prefix && folder->img_prefix[0] != '\0') {
        snprintf(out, out_sz, "%s%s", folder->img_prefix, linux_path);
    } else {
        snprintf(out, out_sz, "%s", linux_path ? linux_path : "");
    }
}

static int scan_dir_page(const char *dir_path,
                         uint32_t page_index,
                         uint32_t page_size,
                         bool show_hidden,
                         folder_item_t *out_items,
                         uint32_t out_cap,
                         uint32_t *out_count,
                         bool *out_has_next)
{
    uint64_t start;
    uint64_t end;
    uint64_t logical_index = 0;
    int pass;

    if (!dir_path || !out_items || !out_count || !out_has_next || page_size == 0) {
        return -1;
    }

    *out_count = 0;
    *out_has_next = false;

    start = (uint64_t)page_index * (uint64_t)page_size;
    end = start + page_size;

    for (pass = 0; pass < 2; pass++) {
        DIR *dir = opendir(dir_path);
        struct dirent *entry;

        if (!dir) {
            FOLDER_LOG("opendir failed: %s errno=%d", dir_path, errno);
            return -1;
        }

        while ((entry = readdir(dir)) != NULL) {
            char full_path[PATH_MAX];
            bool is_dir = false;

            if (should_skip_name(entry->d_name, show_hidden)) continue;
            if (path_join(full_path, sizeof(full_path), dir_path, entry->d_name) != 0) continue;
            if (stat_path(full_path, &is_dir) != 0) continue;

            if (pass == 0 && !is_dir) continue;
            if (pass == 1 && is_dir) continue;

            if (logical_index >= start && logical_index < end && *out_count < out_cap) {
                safe_copy(out_items[*out_count].name, sizeof(out_items[*out_count].name), entry->d_name);
                safe_copy(out_items[*out_count].full_path, sizeof(out_items[*out_count].full_path), full_path);
                out_items[*out_count].is_dir = is_dir;
                (*out_count)++;
            }

            logical_index++;

            if (logical_index > end) {
                *out_has_next = true;
                closedir(dir);
                return 0;
            }
        }

        closedir(dir);
    }

    return 0;
}

static void set_font20(lv_obj_t *obj)
{
    if (!obj) return;
    lv_obj_set_style_text_font(obj, &lv_font_Regular_20, 0);
}

static void set_text_color(lv_obj_t *obj, uint32_t color)
{
    if (!obj) return;
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    set_font20(obj);
}

static lv_obj_t *create_btn(lv_obj_t *parent, const char *txt, lv_coord_t w)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_t *lab = lv_label_create(btn);

    lv_obj_set_size(btn, w, FOLDER_BTN_H);
    lv_obj_set_style_radius(btn, FOLDER_BTN_RADIUS, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(COL_ROW), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(COL_ROW_PRESS), LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(COL_LINE), 0);
    lv_obj_set_style_pad_left(btn, 22, 0);
    lv_obj_set_style_pad_right(btn, 22, 0);

    lv_label_set_text(lab, txt);
    lv_obj_center(lab);
    set_text_color(lab, COL_TEXT);

    return btn;
}

static lv_obj_t *create_row(lv_obj_t *parent, const folder_item_t *item)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_t *icon = lv_label_create(btn);
    lv_obj_t *name = lv_label_create(btn);
    lv_obj_t *right = lv_label_create(btn);

    lv_obj_set_width(btn, LV_PCT(100));
    lv_obj_set_height(btn, FOLDER_ROW_H);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(COL_ROW), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(COL_ROW_PRESS), LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_pad_left(btn, 22, 0);
    lv_obj_set_style_pad_right(btn, 22, 0);
    lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_style_pad_bottom(btn, 0, 0);

    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(btn, 18, 0);

    lv_label_set_text(icon, item_icon(item));
    lv_obj_set_width(icon, 38);
    set_text_color(icon, COL_SUBTEXT);

    lv_label_set_text(name, item ? item->name : "");
    lv_label_set_long_mode(name, LV_LABEL_LONG_DOT);
    lv_obj_set_flex_grow(name, 1);
    set_text_color(name, COL_TEXT);

    lv_label_set_text(right, (item && item->is_dir) ? LV_SYMBOL_RIGHT : "");
    lv_obj_set_width(right, 34);
    set_text_color(right, COL_SUBTEXT);

    return btn;
}

static void set_btn_disabled(lv_obj_t *btn, bool disabled)
{
    if (!btn) return;
    if (disabled) lv_obj_add_state(btn, LV_STATE_DISABLED);
    else lv_obj_clear_state(btn, LV_STATE_DISABLED);
}

static void free_preview_data(lv_linux_folder_t *folder)
{
    if (!folder) return;
    if (folder->preview_data) {
        free(folder->preview_data);
        folder->preview_data = NULL;
    }
    folder->preview_data_size = 0;
    memset(&folder->preview_dsc, 0, sizeof(folder->preview_dsc));
}

static int read_file_to_memory(lv_linux_folder_t *folder, const char *path)
{
    FILE *fp;
    long sz;
    size_t got;

    if (!folder || !path) return -1;

    free_preview_data(folder);

    fp = fopen(path, "rb");
    if (!fp) {
        FOLDER_LOG("image fopen failed: %s errno=%d", path, errno);
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }

    sz = ftell(fp);
    if (sz <= 0) {
        fclose(fp);
        return -1;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    folder->preview_data = (uint8_t *)malloc((size_t)sz);
    if (!folder->preview_data) {
        fclose(fp);
        FOLDER_LOG("image malloc failed: size=%ld", sz);
        return -1;
    }

    got = fread(folder->preview_data, 1, (size_t)sz, fp);
    fclose(fp);

    if (got != (size_t)sz) {
        FOLDER_LOG("image fread failed: got=%u expect=%ld", (unsigned)got, sz);
        free_preview_data(folder);
        return -1;
    }

    folder->preview_data_size = (uint32_t)sz;
    memset(&folder->preview_dsc, 0, sizeof(folder->preview_dsc));
    folder->preview_dsc.header.always_zero = 0;
    folder->preview_dsc.header.w = 0;
    folder->preview_dsc.header.h = 0;
    folder->preview_dsc.header.cf = LV_IMG_CF_RAW;
    folder->preview_dsc.data_size = folder->preview_data_size;
    folder->preview_dsc.data = folder->preview_data;

    return 0;
}

static void fit_preview_img(lv_obj_t *img, const lv_img_header_t *header)
{
    lv_disp_t *disp;
    lv_coord_t sw;
    lv_coord_t sh;
    lv_coord_t max_w;
    lv_coord_t max_h;
    uint32_t zoom_w;
    uint32_t zoom_h;
    uint32_t zoom;

    if (!img || !header || header->w == 0 || header->h == 0) return;

    disp = lv_disp_get_default();
    sw = lv_disp_get_hor_res(disp);
    sh = lv_disp_get_ver_res(disp);
    max_w = sw - 140;
    max_h = sh - 170;
    if (max_w < 320) max_w = (sw * 86) / 100;
    if (max_h < 240) max_h = (sh * 72) / 100;

    zoom_w = ((uint32_t)max_w * 256U) / header->w;
    zoom_h = ((uint32_t)max_h * 256U) / header->h;
    zoom = zoom_w < zoom_h ? zoom_w : zoom_h;

    if (zoom == 0) zoom = 1;
    if (zoom > 256) zoom = 256;

    lv_img_set_zoom(img, (uint16_t)zoom);
    lv_img_set_antialias(img, true);
}

static int close_preview_impl(lv_linux_folder_t *folder)
{
    if (!folder) return -1;

    if (folder->preview_mask) {
        lv_obj_del(folder->preview_mask);
        folder->preview_mask = NULL;
        folder->preview_panel = NULL;
        folder->preview_title = NULL;
        folder->preview_img = NULL;
        folder->preview_hint = NULL;
        folder->preview_close_btn = NULL;
        FOLDER_LOG("close image preview");
    }

    free_preview_data(folder);
    folder->preview_path[0] = '\0';
    return 0;
}

static void preview_close_cb(lv_event_t *e)
{
    lv_linux_folder_t *folder = lv_event_get_user_data(e);
    close_preview_impl(folder);
}

static void create_preview_shell(lv_linux_folder_t *folder, const char *name)
{
    folder->preview_mask = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(folder->preview_mask);
    lv_obj_set_size(folder->preview_mask, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(folder->preview_mask, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(folder->preview_mask, LV_OPA_90, 0);
    lv_obj_add_flag(folder->preview_mask, LV_OBJ_FLAG_CLICKABLE);

    folder->preview_panel = lv_obj_create(folder->preview_mask);
    lv_obj_set_size(folder->preview_panel, 1160, 720);
    lv_obj_center(folder->preview_panel);
    lv_obj_set_style_bg_color(folder->preview_panel, lv_color_hex(COL_BG), 0);
    lv_obj_set_style_bg_opa(folder->preview_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(folder->preview_panel, 1, 0);
    lv_obj_set_style_border_color(folder->preview_panel, lv_color_hex(COL_LINE), 0);
    lv_obj_set_style_radius(folder->preview_panel, FOLDER_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_all(folder->preview_panel, 18, 0);

    folder->preview_title = lv_label_create(folder->preview_panel);
    lv_label_set_text(folder->preview_title, name ? name : "图片预览");
    lv_label_set_long_mode(folder->preview_title, LV_LABEL_LONG_DOT);
    lv_obj_set_width(folder->preview_title, 950);
    lv_obj_align(folder->preview_title, LV_ALIGN_TOP_LEFT, 8, 6);
    set_text_color(folder->preview_title, COL_TEXT);

    folder->preview_close_btn = create_btn(folder->preview_panel, LV_SYMBOL_CLOSE, 68);
    lv_obj_align(folder->preview_close_btn, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(folder->preview_close_btn, preview_close_cb, LV_EVENT_CLICKED, folder);

    folder->preview_img = lv_img_create(folder->preview_panel);
    lv_obj_align(folder->preview_img, LV_ALIGN_CENTER, 0, 30);
}

static void show_preview_hint(lv_linux_folder_t *folder, const char *txt)
{
    if (!folder || !folder->preview_panel) return;
    folder->preview_hint = lv_label_create(folder->preview_panel);
    lv_label_set_text(folder->preview_hint, txt ? txt : "图片无法显示");
    lv_obj_set_width(folder->preview_hint, LV_PCT(86));
    lv_label_set_long_mode(folder->preview_hint, LV_LABEL_LONG_WRAP);
    lv_obj_align(folder->preview_hint, LV_ALIGN_CENTER, 0, 0);
    set_text_color(folder->preview_hint, COL_SUBTEXT);
}

static int open_preview(lv_linux_folder_t *folder, const char *linux_path, const char *name)
{
    lv_img_header_t header;
    lv_res_t res;

    if (!folder || !linux_path) return -1;

    FOLDER_LOG("show image request: linux_path=%s", linux_path);

    close_preview_impl(folder);
    create_preview_shell(folder, name);

    /* 关键改动：先读 Linux 文件到内存，再 lv_img_set_src(&lv_img_dsc_t)。
     * 这样不依赖 LVGL FS 盘符，不需要 S:/mnt/xxx 也能走 set_src。
     */
    if (read_file_to_memory(folder, linux_path) == 0) {
        memset(&header, 0, sizeof(header));
        res = lv_img_decoder_get_info(&folder->preview_dsc, &header);
        if (res == LV_RES_OK) {
            lv_img_set_src(folder->preview_img, &folder->preview_dsc);
            fit_preview_img(folder->preview_img, &header);
            lv_obj_align(folder->preview_img, LV_ALIGN_CENTER, 0, 30);
            FOLDER_LOG("image preview ok: src=memory size=%u w=%u h=%u", folder->preview_data_size, header.w, header.h);
            return 0;
        }

        FOLDER_LOG("memory image decoder failed: %s", linux_path);
        free_preview_data(folder);
    }

    /* 兜底：如果你工程注册了 LVGL FS，比如 A:，这里还能走路径 set_src。 */
    build_lvgl_img_src(folder, linux_path, folder->preview_path, sizeof(folder->preview_path));
    FOLDER_LOG("fallback image source: lvgl_src=%s", folder->preview_path);

    memset(&header, 0, sizeof(header));
    res = lv_img_decoder_get_info(folder->preview_path, &header);
    if (res == LV_RES_OK) {
        lv_img_set_src(folder->preview_img, folder->preview_path);
        fit_preview_img(folder->preview_img, &header);
        lv_obj_align(folder->preview_img, LV_ALIGN_CENTER, 0, 30);
        FOLDER_LOG("image preview ok: src=path w=%u h=%u", header.w, header.h);
        return 0;
    }

    show_preview_hint(folder,
        "图片无法显示\n"
        "已尝试：\n"
        "1. 读 Linux 文件到内存后 lv_img_set_src(&dsc)\n"
        "2. 直接 lv_img_set_src(path) 兜底\n\n"
        "如果还是失败，说明当前 LVGL 没开 JPG/PNG 解码器。JPG 需要 LV_USE_TJPGD=1，PNG 需要 LV_USE_PNG=1。");

    FOLDER_LOG("image decoder failed: linux=%s fallback=%s", linux_path, folder->preview_path);
    return -1;
}

static void item_ctx_free_cb(lv_event_t *e)
{
    item_click_ctx_t *ctx = lv_event_get_user_data(e);
    if (ctx) free(ctx);
}

static void item_click_cb(lv_event_t *e)
{
    item_click_ctx_t *ctx = lv_event_get_user_data(e);
    lv_linux_folder_t *folder;

    if (!ctx || !ctx->folder) return;
    folder = ctx->folder;

    if (ctx->item.is_dir) {
        FOLDER_LOG("click dir: %s", ctx->item.full_path);
        safe_copy(folder->current_dir, sizeof(folder->current_dir), ctx->item.full_path);
        folder->page_index = 0;
        folder_render(folder);
        return;
    }

    if (is_image_file(ctx->item.name)) {
        FOLDER_LOG("click image: %s", ctx->item.full_path);
        open_preview(folder, ctx->item.full_path, ctx->item.name);
        return;
    }

    if (is_video_file(ctx->item.name)) {
        FOLDER_LOG("click video: %s", ctx->item.full_path);
        if (folder->video_cb) folder->video_cb(ctx->item.full_path, folder->user_data);
        return;
    }

    FOLDER_LOG("click file: %s", ctx->item.full_path);
    if (folder->other_file_cb) folder->other_file_cb(ctx->item.full_path, folder->user_data);
}

static void back_click_cb(lv_event_t *e)
{
    lv_linux_folder_t *folder = lv_event_get_user_data(e);
    if (!folder) return;

    if (strcmp(folder->current_dir, "/") == 0) {
        FOLDER_LOG("already at linux root: %s", folder->current_dir);
        return;
    }

    FOLDER_LOG("back parent from: %s", folder->current_dir);
    get_parent_dir(folder->current_dir);
    folder->page_index = 0;
    FOLDER_LOG("current dir: %s", folder->current_dir);
    folder_render(folder);
}

static void home_click_cb(lv_event_t *e)
{
    lv_linux_folder_t *folder = lv_event_get_user_data(e);
    if (!folder) return;

    safe_copy(folder->current_dir, sizeof(folder->current_dir), "/");
    folder->page_index = 0;
    FOLDER_LOG("back linux root: %s", folder->current_dir);
    folder_render(folder);
}

static void prev_click_cb(lv_event_t *e)
{
    lv_linux_folder_t *folder = lv_event_get_user_data(e);
    if (!folder || folder->page_index == 0) return;

    folder->page_index--;
    FOLDER_LOG("prev page: dir=%s page=%u", folder->current_dir, folder->page_index);
    folder_render(folder);
}

static void next_click_cb(lv_event_t *e)
{
    lv_linux_folder_t *folder = lv_event_get_user_data(e);
    if (!folder || !folder->has_next) return;

    folder->page_index++;
    FOLDER_LOG("next page: dir=%s page=%u", folder->current_dir, folder->page_index);
    folder_render(folder);
}

static void apply_base_styles(lv_linux_folder_t *folder)
{
    lv_obj_set_style_bg_color(folder->root, lv_color_hex(COL_BG), 0);
    lv_obj_set_style_bg_opa(folder->root, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(folder->root, 0, 0);
    lv_obj_set_style_pad_all(folder->root, FOLDER_ROOT_PAD, 0);
    lv_obj_set_style_pad_row(folder->root, FOLDER_ROOT_GAP, 0);

    lv_obj_set_style_bg_color(folder->header, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_bg_opa(folder->header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(folder->header, 1, 0);
    lv_obj_set_style_border_color(folder->header, lv_color_hex(COL_LINE), 0);
    lv_obj_set_style_radius(folder->header, FOLDER_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_left(folder->header, 14, 0);
    lv_obj_set_style_pad_right(folder->header, 14, 0);
    lv_obj_set_style_pad_top(folder->header, 8, 0);
    lv_obj_set_style_pad_bottom(folder->header, 8, 0);
    lv_obj_set_style_pad_row(folder->header, 6, 0);

    lv_obj_set_style_bg_color(folder->list, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_bg_opa(folder->list, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(folder->list, 1, 0);
    lv_obj_set_style_border_color(folder->list, lv_color_hex(COL_LINE), 0);
    lv_obj_set_style_radius(folder->list, FOLDER_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_all(folder->list, FOLDER_LIST_PAD, 0);
    lv_obj_set_style_pad_row(folder->list, FOLDER_ROW_GAP, 0);
    lv_obj_set_scrollbar_mode(folder->list, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_style_bg_color(folder->footer, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_bg_opa(folder->footer, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(folder->footer, 1, 0);
    lv_obj_set_style_border_color(folder->footer, lv_color_hex(COL_LINE), 0);
    lv_obj_set_style_radius(folder->footer, FOLDER_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_all(folder->footer, 8, 0);

    set_text_color(folder->path_label, COL_SUBTEXT);
    set_text_color(folder->page_label, COL_TEXT);
}

static int folder_render(lv_linux_folder_t *folder)
{
    folder_item_t *items;
    uint32_t count = 0;
    uint32_t i;
    bool has_next = false;

    if (!folder) return -1;

    lv_obj_clean(folder->list);
    lv_label_set_text(folder->path_label, folder->current_dir);

    items = (folder_item_t *)calloc(folder->page_size, sizeof(folder_item_t));
    if (!items) return -1;

    if (scan_dir_page(folder->current_dir, folder->page_index, folder->page_size,
                      folder->show_hidden, items, folder->page_size,
                      &count, &has_next) != 0) {
        lv_obj_t *lab = lv_label_create(folder->list);
        lv_label_set_text_fmt(lab, "打开目录失败\n%s", folder->current_dir);
        set_text_color(lab, COL_SUBTEXT);
        free(items);
        return -1;
    }

    folder->has_next = has_next;

    if (count == 0) {
        lv_obj_t *lab = lv_label_create(folder->list);
        lv_label_set_text(lab, "此目录为空");
        set_text_color(lab, COL_SUBTEXT);
    }

    for (i = 0; i < count; i++) {
        lv_obj_t *btn = create_row(folder->list, &items[i]);
        item_click_ctx_t *ctx = (item_click_ctx_t *)calloc(1, sizeof(item_click_ctx_t));
        if (!ctx) continue;

        ctx->folder = folder;
        memcpy(&ctx->item, &items[i], sizeof(folder_item_t));
        lv_obj_add_event_cb(btn, item_click_cb, LV_EVENT_CLICKED, ctx);
        lv_obj_add_event_cb(btn, item_ctx_free_cb, LV_EVENT_DELETE, ctx);
    }

    lv_label_set_text_fmt(folder->page_label, "%u", folder->page_index + 1);
    set_btn_disabled(folder->btn_prev, folder->page_index == 0);
    set_btn_disabled(folder->btn_next, !folder->has_next);

    FOLDER_LOG("render dir=%s page=%u count=%u has_next=%d", folder->current_dir, folder->page_index, count, has_next ? 1 : 0);

    free(items);
    return 0;
}

lv_linux_folder_t *lv_linux_folder_create(lv_obj_t *parent,
                                          const lv_linux_folder_cfg_t *cfg)
{
    lv_linux_folder_t *folder;
    lv_obj_t *title_row;
    lv_obj_t *title;
    lv_obj_t *btn_row;

    if (!parent || !cfg || !cfg->start_dir) return NULL;

    folder = (lv_linux_folder_t *)calloc(1, sizeof(lv_linux_folder_t));
    if (!folder) return NULL;

    folder->page_size = FOLDER_PAGE_SIZE;
    folder->show_hidden = cfg->show_hidden;
    folder->img_prefix = cfg->lvgl_img_path_prefix;
    folder->video_cb = cfg->video_cb;
    folder->other_file_cb = cfg->other_file_cb;
    folder->user_data = cfg->user_data;
    folder->page_index = 0;

    safe_copy(folder->current_dir, sizeof(folder->current_dir), cfg->start_dir);
    safe_copy(folder->root_dir, sizeof(folder->root_dir), cfg->start_dir);

    folder->root = lv_obj_create(parent);
    lv_obj_set_size(folder->root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(folder->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(folder->root, LV_SCROLLBAR_MODE_OFF);

    folder->header = lv_obj_create(folder->root);
    lv_obj_set_width(folder->header, LV_PCT(100));
    lv_obj_set_height(folder->header, FOLDER_HEADER_H);
    lv_obj_set_flex_flow(folder->header, LV_FLEX_FLOW_COLUMN);

    title_row = lv_obj_create(folder->header);
    lv_obj_remove_style_all(title_row);
    lv_obj_set_width(title_row, LV_PCT(100));
    lv_obj_set_height(title_row, FOLDER_BTN_H);
    lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    title = lv_label_create(title_row);
    lv_label_set_text(title, "文件");
    lv_obj_set_width(title, 160);
    set_text_color(title, COL_TEXT);

    /* 右上角操作区：上一级和根目录固定在右侧。 */
    btn_row = lv_obj_create(title_row);
    lv_obj_remove_style_all(btn_row);
    lv_obj_set_size(btn_row, 286, FOLDER_BTN_H);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(btn_row, 12, 0);

    folder->btn_back = create_btn(btn_row, "上一级", 126);
    folder->btn_home = create_btn(btn_row, "根目录", 126);
    lv_obj_add_event_cb(folder->btn_back, back_click_cb, LV_EVENT_CLICKED, folder);
    lv_obj_add_event_cb(folder->btn_home, home_click_cb, LV_EVENT_CLICKED, folder);

    folder->path_label = lv_label_create(folder->header);
    lv_label_set_long_mode(folder->path_label, LV_LABEL_LONG_DOT);
    lv_obj_set_width(folder->path_label, LV_PCT(100));

    folder->list = lv_obj_create(folder->root);
    lv_obj_set_width(folder->list, LV_PCT(100));
    lv_obj_set_height(folder->list, FOLDER_LIST_H);
    lv_obj_set_flex_flow(folder->list, LV_FLEX_FLOW_COLUMN);

    folder->footer = lv_obj_create(folder->root);
    lv_obj_set_width(folder->footer, LV_PCT(100));
    lv_obj_set_height(folder->footer, FOLDER_FOOTER_H);
    lv_obj_set_flex_flow(folder->footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(folder->footer, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    folder->btn_prev = create_btn(folder->footer, "上一页", 132);
    folder->page_label = lv_label_create(folder->footer);
    lv_obj_set_width(folder->page_label, 120);
    lv_obj_set_style_text_align(folder->page_label, LV_TEXT_ALIGN_CENTER, 0);
    folder->btn_next = create_btn(folder->footer, "下一页", 132);

    lv_obj_add_event_cb(folder->btn_prev, prev_click_cb, LV_EVENT_CLICKED, folder);
    lv_obj_add_event_cb(folder->btn_next, next_click_cb, LV_EVENT_CLICKED, folder);

    apply_base_styles(folder);
    folder_render(folder);

    FOLDER_LOG("create ok: start_dir=%s page_size=%u", folder->current_dir, folder->page_size);
    return folder;
}

void lv_linux_folder_destroy(lv_linux_folder_t **pfolder)
{
    lv_linux_folder_t *folder;

    if (!pfolder || !*pfolder) return;
    folder = *pfolder;

    close_preview_impl(folder);

    if (folder->root) {
        lv_obj_del(folder->root);
        folder->root = NULL;
    }

    FOLDER_LOG("destroy: %s", folder->current_dir);
    free(folder);
    *pfolder = NULL;
}

const char *lv_linux_folder_get_current_dir(lv_linux_folder_t *folder)
{
    return folder ? folder->current_dir : NULL;
}

int lv_linux_folder_reload_first_page(lv_linux_folder_t *folder)
{
    if (!folder) return -1;
    folder->page_index = 0;
    return folder_render(folder);
}

int lv_linux_folder_set_dir(lv_linux_folder_t *folder, const char *dir_path)
{
    bool is_dir = false;

    if (!folder || !dir_path) return -1;
    if (stat_path(dir_path, &is_dir) != 0 || !is_dir) {
        FOLDER_LOG("set dir failed: %s errno=%d", dir_path, errno);
        return -1;
    }

    safe_copy(folder->current_dir, sizeof(folder->current_dir), dir_path);
    folder->page_index = 0;
    return folder_render(folder);
}

int lv_linux_folder_close_preview(lv_linux_folder_t *folder)
{
    return close_preview_impl(folder);
}
