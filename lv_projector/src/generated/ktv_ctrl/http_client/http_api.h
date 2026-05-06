#ifndef HTTP_API_H
#define HTTP_API_H

#include "http_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 契约说明：
 *
 * 1) http_fetch():
 *    - response_data 只在 callback 回调期间有效
 *    - callback 返回后，框架自动释放 response_data
 *    - 如果业务需要长期持有，必须在 callback 内自行拷贝
 *
 * 2) http_download():
 *    - local_path 是下载资源的唯一键
 *    - 相同 local_path 的并发下载会自动合并为一个真实下载
 *    - 若相同 local_path 但 url 不同，直接返回 HTTP_ERR_INVALID_PARAM
 */

http_err_t http_init(const http_config_t *config);
void http_deinit(void);

http_err_t http_download(const char *url,
                         const char *local_path,
                         http_callback_t callback,
                         void *user_data);

/* 同 http_download，但允许指定优先级。HIGH 会在 http_pool 的 overflow
 * 队列里插队到队首，更快被 worker 消费。同路径多次提交仍然合并。 */
http_err_t http_download_priority(const char *url,
                                  const char *local_path,
                                  http_priority_t priority,
                                  http_callback_t callback,
                                  void *user_data);

http_err_t http_fetch(const char *url,
                      http_callback_t callback,
                      void *user_data);

/* 同 http_fetch，但允许指定优先级。HIGH 用于"内容里包着别人下载所需 URL"
 * 的关键 JSON（例如歌手页 page JSON：它一来才会解锁本页 50 张图的 URL，
 * 必须比图片下载更早跑完）。 */
http_err_t http_fetch_priority(const char *url,
                               http_priority_t priority,
                               http_callback_t callback,
                               void *user_data);

void http_wait(void);
void http_get_stats(http_stats_t *stats);
void http_print_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_API_H */