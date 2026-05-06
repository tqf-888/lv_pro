#ifndef KTV_CTRL_H
#define KTV_CTRL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    KTV_REQ_FETCH_JSON = 0,
    KTV_REQ_FETCH_MEMORY,
    KTV_REQ_DOWNLOAD_FILE,
    KTV_REQ_DOWNLOAD_IMAGE
} KtvReqType_t;

/* 与 http_priority_t 一一对应，但 ktv 层不暴露 http_common.h，单独定义。
 * 旧代码 memset(&req,0,...) 后保持 LOW 行为不变。 */
typedef enum
{
    KTV_PRIORITY_LOW  = 0,
    KTV_PRIORITY_HIGH = 1
} KtvPriority_t;

struct KtvRequest;
typedef struct KtvRequest KtvRequest_t;

typedef void (*KtvReqCompleteCb)(KtvRequest_t *req,
                                 int result,
                                 const void *data,
                                 size_t data_len);

typedef struct
{
    KtvReqType_t type;
    KtvReqCompleteCb on_complete;
} KtvReqOps_t;

struct KtvRequest
{
    const KtvReqOps_t *ops;

    int id;
    int page_index;

    char url[512];
    char local_path[512];

    void *user_data;

    /* 0 = LOW（默认），非零 = HIGH。仅 KTV_REQ_DOWNLOAD_IMAGE / DOWNLOAD_FILE
     * 路径会下沉到 http_download_priority；fetch_json/memory 路径忽略。 */
    KtvPriority_t priority;
};

typedef struct
{
    uint32_t thread_num;
    uint32_t queue_size;
    uint32_t timeout_sec;
    uint32_t connect_timeout_sec;
    uint32_t max_redirects;
} KtvCtrlConfig_t;

/* 安全默认初始化：旧代码直接调用这个 */
int Ktv_Ctrl_Init(void);

/* 自定义配置初始化：新代码用这个 */
int Ktv_Ctrl_InitEx(const KtvCtrlConfig_t *cfg);

void Ktv_Ctrl_Deinit(void);
int Ktv_Ctrl_PostTask(const KtvRequest_t *req);
void Ktv_Ctrl_WaitAll(void);
void Ktv_Ctrl_PrintStats(void);

#ifdef __cplusplus
}
#endif

#endif /* KTV_CTRL_H */