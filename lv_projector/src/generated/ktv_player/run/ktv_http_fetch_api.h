#ifndef __KTV_HTTP_FETCH_API_H__
#define __KTV_HTTP_FETCH_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

/*
 * 通用 HTTP GET 到内存。
 * 约定：
 * 1. 成功返回 1，失败返回 0
 * 2. out_data 写入完整响应正文，并保证 '\0' 结尾
 * 3. 该文件专门负责 HTTP 请求，不处理业务 JSON 解析
 */
int ktv_http_get_to_memory(const char *url,
                           char *out_data,
                           size_t out_size,
                           int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
