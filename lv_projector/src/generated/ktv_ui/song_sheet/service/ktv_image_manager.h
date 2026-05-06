#ifndef KTV_IMAGE_MANAGER_H
#define KTV_IMAGE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#ifndef KTV_IMG_URL_MAX
#define KTV_IMG_URL_MAX             512
#endif

#ifndef KTV_IMG_PATH_MAX
#define KTV_IMG_PATH_MAX            256
#endif

#ifndef KTV_IMG_MAX_ENTRIES
#define KTV_IMG_MAX_ENTRIES         1024
#endif

typedef enum
{
    KTV_IMG_STATE_EMPTY = 0,
    KTV_IMG_STATE_DOWNLOADING,
    KTV_IMG_STATE_READY,
    KTV_IMG_STATE_FAILED
} ktv_img_state_t;

typedef enum
{
    KTV_IMG_PUSH_OK = 0,
    KTV_IMG_PUSH_ALREADY_EXISTS = 1,
    KTV_IMG_PUSH_INVALID_ARG = -1,
    KTV_IMG_PUSH_URL_MISMATCH = -2,
    KTV_IMG_PUSH_NO_SPACE = -3,
    KTV_IMG_PUSH_PATH_FAIL = -4,
    KTV_IMG_PUSH_SEND_FAIL = -5,
    KTV_IMG_PUSH_NOT_INIT = -6
} ktv_img_push_ret_t;

struct ktv_img_mgr;

typedef int (*ktv_img_make_save_path_cb)(
    void *user_ctx,
    uint32_t id,
    char *buf,
    size_t buf_size);

typedef int (*ktv_img_send_request_cb)(
    void *user_ctx,
    uint32_t id,
    const char *url,
    const char *save_path);

typedef int (*ktv_img_cancel_request_cb)(
    void *user_ctx,
    uint32_t id);

typedef int (*ktv_img_remove_file_cb)(
    void *user_ctx,
    const char *path);

typedef void (*ktv_img_cleanup_session_cb)(
    void *user_ctx);

typedef struct
{
    uint32_t id;
    char url[KTV_IMG_URL_MAX];
    char save_path[KTV_IMG_PATH_MAX];
    uint8_t in_use;
    uint8_t retry_count;
    uint8_t max_retry;
    uint8_t request_sent;
    ktv_img_state_t state;
} ktv_img_entry_t;

typedef struct
{
    uint8_t max_retry;
    ktv_img_make_save_path_cb make_save_path;
    ktv_img_send_request_cb send_request;
    ktv_img_cancel_request_cb cancel_request;
    ktv_img_remove_file_cb remove_file;
    ktv_img_cleanup_session_cb cleanup_session;
    void *user_ctx;
} ktv_img_mgr_cfg_t;

typedef struct ktv_img_mgr
{
    ktv_img_mgr_cfg_t cfg;
    ktv_img_entry_t entries[KTV_IMG_MAX_ENTRIES];
    void *lock;      /* pthread_mutex_t*，内部管理 */
    uint8_t started;
} ktv_img_mgr_t;

int  ktv_img_mgr_init(ktv_img_mgr_t *mgr, const ktv_img_mgr_cfg_t *cfg);
void ktv_img_mgr_deinit(ktv_img_mgr_t *mgr);

int  ktv_img_mgr_push(ktv_img_mgr_t *mgr, uint32_t id, const char *url);
int  ktv_img_mgr_pull_copy(ktv_img_mgr_t *mgr, uint32_t id, char *buf, size_t buf_size);
const char *ktv_img_mgr_pull_tls(ktv_img_mgr_t *mgr, uint32_t id);

void ktv_img_mgr_notify_ok(ktv_img_mgr_t *mgr, uint32_t id);
void ktv_img_mgr_notify_fail(ktv_img_mgr_t *mgr, uint32_t id);

// ktv_img_state_t ktv_img_mgr_get_state(ktv_img_mgr_t *mgr, uint32_t id);
int  ktv_img_mgr_has(ktv_img_mgr_t *mgr, uint32_t id);
int  ktv_img_mgr_retry(ktv_img_mgr_t *mgr, uint32_t id);
void ktv_img_mgr_remove(ktv_img_mgr_t *mgr, uint32_t id);
void ktv_img_mgr_clear(ktv_img_mgr_t *mgr);

#ifdef __cplusplus
}
#endif

#endif /* KTV_IMAGE_MANAGER_H */
