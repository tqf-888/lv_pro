#ifndef HTTP_POST_API_H
#define HTTP_POST_API_H

#include "http_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 新增独立 POST 模块：
 * - 不修改任何现有 http_* 源文件
 * - url / body / content_type 在提交时立即做深拷贝
 * - 调用方无需释放请求体，也无需释放响应体
 * - response_data 仅在 callback 回调期间有效
 * - callback 返回后，本模块自动释放 response_data
 *
 * 返回语义：
 * - http_post() 只表示“是否成功入队”
 * - 真正网络执行结果通过 callback 里的 task->err_code / task->http_status 获取
 *
 * HTTP 状态码语义：
 * - 2xx / 3xx：task->err_code = HTTP_OK，task->state = HTTP_STATE_COMPLETED
 * - 4xx / 5xx：保留 response_data 供业务读取，同时
 *               task->err_code = HTTP_ERR_NETWORK，task->state = HTTP_STATE_FAILED
 */
http_err_t http_post(const char *url,
                     const char *body,
                     const char *content_type,
                     http_callback_t callback,
                     void *user_data);

/* 可选：等待当前独立 POST 队列执行完毕 */
void http_post_wait_all(void);

/* 可选：关闭独立 POST worker。未调用也不影响 http_post() 使用。 */
void http_post_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_POST_API_H */
