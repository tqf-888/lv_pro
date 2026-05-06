#ifndef KTV_SONGSHEET_MANAGER_H
#define KTV_SONGSHEET_MANAGER_H

#include <stddef.h>

#define KTV_SONGSHEET_PLACEHOLDER_IMG_PATH_LINUX    "/tmp/songsheet/212.png"
#define KTV_SONGSHEET_PLACEHOLDER_IMG_PATH_LVGL     "S:/tmp/songsheet/212.png"
#define KTV_SONGSHEET_EMPTY_TEXT_PLACEHOLDER        "暂时无"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * songsheet 业务说明：
 *
 * 1) 数据按 JSON 分页组织，每页最多 50 条
 * 2) 通过 id 自动映射到对应 json 文件和页内下标
 * 3) 只缓存成功解析的 json
 * 4) 图片状态通过内部标记维护
 */

/* 获取歌单标题和描述 */
int ktv_get_songsheet_info(int id,
                           char *title_buf,
                           size_t title_size,
                           char *desc_buf,
                           size_t desc_size);

/* 获取歌单图片 URL */
int ktv_get_songsheet_pic_url(int id, char *buf, size_t size);

/*
 * 给下载层 / fopen / stat / rename 用
 * 返回 Linux 路径：
 *   /tmp/songsheet/<id>.png
 */
int app_data_make_songsheet_pic_path(int id, char *buf, size_t size);

/*
 * 给 LVGL 显示层用
 * 返回 LVGL 文件系统路径：
 *   S:/tmp/songsheet/<id>.png
 */
int app_data_make_songsheet_pic_lvgl_path(int id, char *buf, size_t size);

/*
 * 给普通文件读取用的 json 路径
 * 返回 Linux 路径：
 *   /tmp/songsheet_json/songsheet_<file_index>.json
 */
int ktv_get_songsheet_json_path(int file_index, char *buf, size_t size);

/*
 * 如果以后显示层需要直接用 LVGL 路径访问 json，可用这个
 * 返回：
 *   S:/tmp/songsheet_json/songsheet_<file_index>.json
 */
int ktv_get_songsheet_json_lvgl_path(int file_index, char *buf, size_t size);

/* 设置图片可用状态 */
int ktv_set_songsheet_pic_available(int id, int available);

/*
 * 兼容旧语义：
 * - 只有当图片 available == 1 时才返回成功
 * - 成功返回后，会把 available 清 0
 *
 * 注意：
 * - 这里返回的是给 LVGL 显示用的路径
 */
int ktv_get_songsheet_pic_path(int id, char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* KTV_SONGSHEET_MANAGER_H */