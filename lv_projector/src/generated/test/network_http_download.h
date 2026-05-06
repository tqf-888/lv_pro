#ifndef NETWORK_HTTP_DOWNLOAD_H
#define NETWORK_HTTP_DOWNLOAD_H

#include <stddef.h>

/* =============================================================================
 * 错误码定义
 * ============================================================================= */
typedef enum
{
    RESULT_NETWORK_SUCCESS = 0,         /* 成功 */
    RESULT_NETWORK_FAIL = -1,           /* 通用失败 */
    RESULT_NETWORK_FUNC_ERR = -2,       /* 函数参数错误 */
    RESULT_NETWORK_MEMORY_LOW = -3,     /* 内存不足 */
    RESULT_NETWORK_FILE_ERR = -4,       /* 文件操作失败 */
    RESULT_NETWORK_CURL_INIT_ERR = -5,  /* Curl初始化失败 */
    
    /* Curl 相关错误 */
    RESULT_NETWORK_CURL_CONNECT_ERR = -100,
    RESULT_NETWORK_CURL_TIMEOUT = -101,
    RESULT_NETWORK_CURL_DNS_ERR = -102,
    RESULT_NETWORK_CURL_SSL_ERR = -103,
    RESULT_NETWORK_CURL_PARTIAL_FILE = -104,
    
    /* HTTP 状态码错误 */
    RESULT_NETWORK_HTTP_REQ_ERR = -200,
    RESULT_NETWORK_HTTP_400 = -400,
    RESULT_NETWORK_HTTP_401 = -401,
    RESULT_NETWORK_HTTP_403 = -403,
    RESULT_NETWORK_HTTP_404 = -404,
    RESULT_NETWORK_HTTP_500 = -500,
    RESULT_NETWORK_HTTP_502 = -502,
    RESULT_NETWORK_HTTP_503 = -503,
    RESULT_NETWORK_HTTP_504 = -504

} network_result_t;

/* =============================================================================
 * 类型定义
 * ============================================================================= */

/**
 * @brief 进度回调函数类型
 * @param percent 当前进度百分比 (0-100)
 * @param user_data 用户自定义数据指针
 * @return 返回 0 表示继续下载；返回非 0 表示取消下载
 */
typedef int (*network_progress_callback)(int percent, void *user_data);

/**
 * @brief HTTP下载参数结构体
 */
typedef struct
{
    const char *url;            /* 下载地址 */
    const char *save_path;      /* 文件保存路径 (例如: "/tmp/update.bin") */
    network_progress_callback progress_callback; /* 进度回调函数 (可为NULL) */
    void *user_data;            /* 回调函数的用户数据 (可为NULL) */
} network_http_download_param_t;

/* =============================================================================
 * 公共接口声明
 * ============================================================================= */

/**
 * @brief 执行HTTP GET请求 (获取字符串数据)
 * @param url 请求地址
 * @param out_data 输出数据指针 (需要调用者 free 释放)
 * @param out_len 输出数据长度
 * @return network_result_t 执行结果
 */
network_result_t network_http_get(const char *url, char **out_data, size_t *out_len);

/**
 * @brief 执行HTTP文件下载
 * @param parameter 下载参数结构体指针
 * @return network_result_t 执行结果
 */
network_result_t network_http_download_media(void *parameter);

#endif /* NETWORK_HTTP_DOWNLOAD_H */
