#include "lv_linux_folder_demo.h"
#include "lv_linux_folder.h"
#include <stdio.h>

static lv_linux_folder_t *g_folder = NULL;

static void video_jump_cb(const char *video_path, void *user_data)
{
    (void)user_data;
    printf("[APP] jump video player: %s\n", video_path);

    /* 这里跳转你自己的视频播放器页面 */
}

static void other_file_cb(const char *file_path, void *user_data)
{
    (void)user_data;
    printf("[APP] other file clicked: %s\n", file_path);
}

void demo_open_folder(lv_obj_t *parent)
{
    lv_linux_folder_cfg_t cfg;

    if (g_folder) {
        lv_linux_folder_destroy(&g_folder);
    }

    cfg.start_dir = "/mnt/SDCARD";
    cfg.page_size = 8;              /* 保留字段：组件内部强制 8 行 */
    cfg.show_hidden = false;

    /* 图片预览优先读 Linux 文件到内存，再 lv_img_set_src(&dsc)。
     * 所以这里可以填 NULL，不需要 A:。
     * 如果内存方式失败，会再用这个 prefix 做路径兜底。
     */
    cfg.lvgl_img_path_prefix = NULL;

    cfg.video_cb = video_jump_cb;
    cfg.other_file_cb = other_file_cb;
    cfg.user_data = NULL;

    g_folder = lv_linux_folder_create(parent, &cfg);
}

void demo_close_folder(void)
{
    lv_linux_folder_destroy(&g_folder);
}

/* 兼容你之前调用的函数名 */
void test_open_folder_browser(lv_obj_t *parent)
{
    demo_open_folder(parent);
}

void test_close_folder_browser(void)
{
    demo_close_folder();
}
