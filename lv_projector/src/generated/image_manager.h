#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/*
 * 正式工程分层说明：
 * 1. UI 只认 slot_id（屏幕物理坑位）。
 * 2. 业务内容使用 content_id（例如歌手ID、歌单ID）。
 * 3. 本管理器不负责下载 JSON，也不负责下载图片。
 * 4. 本管理器只负责：
 *    - 页请求去重
 *    - slot/content/url/path 状态管理
 *    - 最后一页状态
 *    - reset 代次失效
 *    - 缓存淘汰
 *    - 线程安全
 * 5. 外部业务层在解析 JSON 后，把 slot_id/content_id/name/url 喂进来。
 * 6. 外部图片下载成功后，把 local_path 回填进来。
 */

#ifndef IMG_URL_MAX
#define IMG_URL_MAX             512
#endif

#ifndef IMG_PATH_MAX
#define IMG_PATH_MAX            256
#endif

#ifndef IMG_NAME_MAX
#define IMG_NAME_MAX            64
#endif

#ifndef IMG_MAX_ENTRIES
#define IMG_MAX_ENTRIES         1024
#endif

#ifndef IMG_MAX_PAGES
#define IMG_MAX_PAGES           256
#endif

typedef enum
{
    IMG_STATE_EMPTY = 0,
    IMG_STATE_WAIT_URL,
    IMG_STATE_WAIT_IMAGE,
    IMG_STATE_READY
} img_state_t;

typedef int (*img_request_page_cb)(
    void *user_ctx,
    uint32_t page_index,
    uint32_t page_size,
    uint32_t generation);

/*
 * high_priority：1 = 这张图正落在 UI 当前"可见范围"内，下载层应当尽快拉到；
 *                0 = 预拉/远景，按普通 FIFO 处理。
 *
 * "可见范围"由 img_mgr_set_visible_range() 设置，与下载是否提交无关，所以
 * 即便是页 JSON 回来的瞬间触发的首次 need_image，也能根据当前可见范围
 * 正确分类。
 */
typedef void (*img_need_image_cb)(
    void *user_ctx,
    uint32_t slot_id,
    uint32_t content_id,
    const char *url,
    uint32_t generation,
    int high_priority);

typedef int (*img_remove_file_cb)(
    void *user_ctx,
    const char *path);

typedef void (*img_cleanup_session_cb)(
    void *user_ctx,
    uint32_t generation);

typedef struct
{
    uint32_t page_size;
    uint32_t max_cached_items;
    uint32_t max_cached_pages;

    img_request_page_cb    request_page;
    img_need_image_cb      need_image;
    img_remove_file_cb     remove_file;
    img_cleanup_session_cb cleanup_session;
    void                  *user_ctx;
} img_mgr_cfg_t;

typedef struct
{
    uint32_t slot_id;
    uint32_t content_id;
    img_state_t state;
    char name[IMG_NAME_MAX];
    char url[IMG_URL_MAX];
    char local_path[IMG_PATH_MAX];
} img_item_info_t;

int      img_mgr_init(const img_mgr_cfg_t *cfg);
void     img_mgr_deinit(void);
void     img_mgr_reset(void);
int      img_mgr_is_inited(void);
uint32_t img_mgr_get_generation(void);

void img_mgr_access(uint32_t slot_id);
void img_mgr_access_range(uint32_t start_slot_id, uint32_t end_slot_id);

/*
 * 设置 UI 当前可见的 slot 范围（含端点）。落在该范围里的 slot 在触发
 * need_image 通知时会带 high_priority=1，下沉到 http 层后会插队。
 *
 * 调用方约定：UI 滑动时立即调一次，通常和 demo_ui_scroll_range 配套。
 * end < start 或 start == 0 表示清空，所有下载都走 LOW。
 */
void img_mgr_set_visible_range(uint32_t start_slot_id, uint32_t end_slot_id);

const char *img_mgr_pull_tls(uint32_t slot_id);
int         img_mgr_pull_copy(uint32_t slot_id, char *buf, size_t buf_size);
img_state_t img_mgr_get_state(uint32_t slot_id);
int         img_mgr_get_item_info(uint32_t slot_id, img_item_info_t *out_info);

int img_mgr_add_page_item(uint32_t generation,
                          uint32_t slot_id,
                          uint32_t content_id,
                          const char *name,
                          const char *url);

int img_mgr_set_image_path(uint32_t generation,
                           uint32_t slot_id,
                           uint32_t content_id,
                           const char *local_path);

void img_mgr_mark_page_ready(uint32_t generation, uint32_t page_index);
void img_mgr_mark_last_page(uint32_t generation, uint32_t page_index);

/*
 * 按当前项目约定：
 * 返回 1 表示“还能继续翻页”
 * 返回 0 表示“已经最后一页”
 */
int img_mgr_is_last(void);

#ifdef __cplusplus
}
#endif

#endif /* IMAGE_MANAGER_H */
