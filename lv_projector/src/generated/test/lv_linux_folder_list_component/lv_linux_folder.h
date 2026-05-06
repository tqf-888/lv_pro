#ifndef LV_LINUX_FOLDER_H
#define LV_LINUX_FOLDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct lv_linux_folder lv_linux_folder_t;

typedef void (*lv_linux_folder_video_cb_t)(const char *video_path, void *user_data);
typedef void (*lv_linux_folder_file_cb_t)(const char *file_path, void *user_data);

typedef struct {
    const char *start_dir;                  /* 起始目录，例如 /mnt/SDCARD */
    uint32_t page_size;                     /* 保留字段：当前组件强制每页 8 行，UI 按 1280x800 适配 */
    bool show_hidden;                       /* 是否显示隐藏文件 */
    const char *lvgl_img_path_prefix;       /* 兜底用：例如 "A:"；优先走内存 set_src，不依赖它 */
    lv_linux_folder_video_cb_t video_cb;    /* 视频点击回调：外部跳转播放器 */
    lv_linux_folder_file_cb_t other_file_cb;/* 其他文件点击回调 */
    void *user_data;
} lv_linux_folder_cfg_t;

lv_linux_folder_t *lv_linux_folder_create(lv_obj_t *parent,
                                          const lv_linux_folder_cfg_t *cfg);
void lv_linux_folder_destroy(lv_linux_folder_t **folder);

const char *lv_linux_folder_get_current_dir(lv_linux_folder_t *folder);
int lv_linux_folder_reload_first_page(lv_linux_folder_t *folder);
int lv_linux_folder_set_dir(lv_linux_folder_t *folder, const char *dir_path);
int lv_linux_folder_close_preview(lv_linux_folder_t *folder);

#ifdef __cplusplus
}
#endif

#endif
