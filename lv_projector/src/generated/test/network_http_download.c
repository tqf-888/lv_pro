/*
 * network_http_download.c
 * 通用HTTP下载模块（深度重构版）
 * Created on: 2025/11/21
 * Author: Wu
 */

#include "network_http_download.h"
#include <curl/curl.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>  // 标准错误码头文件
#include <sys/time.h> // 替代clock_gettime的兼容性头文件

/* =============================================================================
 * 私有类型定义
 * ============================================================================= */

/**
 * @brief 动态内存缓冲区结构
 * 用于HTTP GET请求时动态存储接收到的数据
 */
typedef struct
{
    char *data;         /* 数据指针 */
    size_t total_size;  /* 缓冲区总容量 */
    size_t used_size;   /* 已使用字节数 */
} http_memory_buffer_t;

/**
 * @brief 下载进度上下文结构
 * 用于在libcurl回调中传递用户回调信息
 */
typedef struct
{
    network_progress_callback user_callback; /* 用户定义的进度回调函数 */
    void *user_data;                         /* 用户自定义数据指针 */
    double last_callback_timestamp;          /* 上次回调的时间戳（秒） */
    int last_reported_percent;               /* 上次回调的进度百分比 */
} download_progress_context_t;

/* =============================================================================
 * 私有函数声明
 * ============================================================================= */

static size_t internal_write_memory_callback(void *contents, size_t size, size_t nmemb, void *user_pointer);
static size_t internal_write_file_callback(void *contents, size_t size, size_t nmemb, void *file_pointer);
static int internal_progress_callback(void *client_pointer, double download_total, double download_now, double upload_total, double upload_now);
static void internal_curl_common_setup(CURL *curl_handle, const char *url, long timeout_seconds, long connect_timeout_seconds);
static network_result_t internal_map_curl_code_to_result(CURLcode curl_code);
static network_result_t internal_map_http_status_to_result(long http_status_code);

/* =============================================================================
 * 标准日志宏定义（替代自定义log接口）
 * ============================================================================= */
#define LOG_INFO(fmt, ...)  fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  fprintf(stderr, "[WARN] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt " (errno: %d, msg: %s)\n", ##__VA_ARGS__, errno, strerror(errno))
// 专用于curl错误的打印（不依赖errno）
#define LOG_CURL_ERROR(fmt, curl_err, ...) fprintf(stderr, "[ERROR] " fmt " (curl_err: %s)\n", ##__VA_ARGS__, curl_easy_strerror(curl_err))

/* =============================================================================
 * 私有函数实现
 * ============================================================================= */

/**
 * @brief libcurl内存写入回调（动态扩容）
 */
static size_t internal_write_memory_callback(void *contents, size_t size, size_t nmemb, void *user_pointer)
{
    size_t real_size = size * nmemb;
    http_memory_buffer_t *buffer = (http_memory_buffer_t *)user_pointer;

    /* 检查是否需要扩容：采用2倍扩容策略，最小1024字节 */
    size_t required_size = buffer->used_size + real_size + 1;
    if (required_size > buffer->total_size)
    {
        size_t new_size = (buffer->total_size == 0) ? 1024 : buffer->total_size;
        while (new_size < required_size)
        {
            new_size *= 2;
        }

        char *new_data = realloc(buffer->data, new_size);
        if (new_data == NULL)
        {
            LOG_ERROR("内存分配失败，无法扩展缓冲区");
            return 0; /* 返回0将导致libcurl中止传输 */
        }

        buffer->data = new_data;
        buffer->total_size = new_size;
    }

    /* 拷贝数据并更新偏移 */
    memcpy(buffer->data + buffer->used_size, contents, real_size);
    buffer->used_size += real_size;
    buffer->data[buffer->used_size] = '\0'; /* 确保字符串结尾 */

    return real_size;
}

/**
 * @brief libcurl文件写入回调
 */
static size_t internal_write_file_callback(void *contents, size_t size, size_t nmemb, void *file_pointer)
{
    return fwrite(contents, size, nmemb, (FILE *)file_pointer);
}

/**
 * @brief 下载进度内部回调
 * @note 优化了逻辑，移除标志位，采用提前判断策略降低圈复杂度
 */
static int internal_progress_callback(void *client_pointer, 
                                      double download_total, 
                                      double download_now, 
                                      double upload_total, 
                                      double upload_now)
{
    download_progress_context_t *context = (download_progress_context_t *)client_pointer;

    if (context == NULL || context->user_callback == NULL)
    {
        return 0;
    }

    /* 计算当前百分比 */
    int current_percent = (download_total > 0) ? (int)((download_now * 100.0) / download_total) : 0;
    if (current_percent > 100)
    {
        current_percent = 100;
    }

    /* 获取单调时钟时间（兼容不同系统） */
    double current_timestamp = 0;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == 0)
    {
        current_timestamp = tv.tv_sec + (double)tv.tv_usec / 1e6;
    }
    // 备用方案：如果没有gettimeofday，用clock()
    else
    {
        current_timestamp = (double)clock() / CLOCKS_PER_SEC;
    }

    /* 
     * 频率控制逻辑：
     * 如果满足以下任一条件，则执行回调：
     * 1. 首次回调 (last_callback_timestamp == 0)
     * 2. 时间间隔 >= 0.1秒
     * 3. 进度变化 >= 1%
     * 4. 下载完成 (100%)
     * 否则跳过本次回调
     */
    if (context->last_callback_timestamp > 0)
    {
        double time_elapsed = current_timestamp - context->last_callback_timestamp;
        int percent_diff = current_percent - context->last_reported_percent;

        if (time_elapsed < 0.1 && percent_diff < 1 && current_percent != 100)
        {
            return 0; /* 不满足触发条件，直接返回 */
        }
    }

    /* 执行用户回调 */
    int user_abort_flag = context->user_callback(current_percent, context->user_data);

    /* 更新状态记录 */
    context->last_callback_timestamp = current_timestamp;
    context->last_reported_percent = current_percent;

    /* 非0返回值表示用户请求中止下载 */
    return (user_abort_flag != 0) ? 1 : 0;
}

/**
 * @brief 统一配置libcurl通用参数
 * @details 解耦核心：将重复的配置代码集中管理，便于维护和修改
 */
static void internal_curl_common_setup(CURL *curl_handle, const char *url, long timeout_seconds, long connect_timeout_seconds)
{
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L); /* 默认禁用进度回调 */
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);   /* 避免信号干扰多线程 */
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L); /* 支持重定向 */
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout_seconds);
    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, connect_timeout_seconds);
}

/**
 * @brief 映射libcurl错误码到业务错误码
 */
static network_result_t internal_map_curl_code_to_result(CURLcode curl_code)
{
    switch (curl_code)
    {
        case CURLE_OK:
            return RESULT_NETWORK_SUCCESS;
        case CURLE_OPERATION_TIMEDOUT:
            return RESULT_NETWORK_CURL_TIMEOUT;
        case CURLE_COULDNT_CONNECT:
        case CURLE_COULDNT_RESOLVE_PROXY:
            return RESULT_NETWORK_CURL_CONNECT_ERR;
        case CURLE_COULDNT_RESOLVE_HOST:
            return RESULT_NETWORK_CURL_DNS_ERR;
        case CURLE_SSL_CONNECT_ERROR:
        case CURLE_SSL_CERTPROBLEM:
            return RESULT_NETWORK_CURL_SSL_ERR;
        case CURLE_ABORTED_BY_CALLBACK:
            return RESULT_NETWORK_CURL_PARTIAL_FILE;
        default:
            return RESULT_NETWORK_HTTP_REQ_ERR;
    }
}

/**
 * @brief 映射HTTP状态码到业务错误码
 */
static network_result_t internal_map_http_status_to_result(long http_status_code)
{
    if (http_status_code == 200)
    {
        return RESULT_NETWORK_SUCCESS;
    }

    switch (http_status_code)
    {
        case 400: return RESULT_NETWORK_HTTP_400;
        case 401: return RESULT_NETWORK_HTTP_401;
        case 403: return RESULT_NETWORK_HTTP_403;
        case 404: return RESULT_NETWORK_HTTP_404;
        case 500: return RESULT_NETWORK_HTTP_500;
        case 502: return RESULT_NETWORK_HTTP_502;
        case 503: return RESULT_NETWORK_HTTP_503;
        case 504: return RESULT_NETWORK_HTTP_504;
        default:  return RESULT_NETWORK_HTTP_REQ_ERR;
    }
}

/* =============================================================================
 * 公共接口实现
 * ============================================================================= */

/**
 * @brief 执行HTTP GET请求并获取字符串数据
 */
network_result_t network_http_get(const char *url, char **out_data, size_t *out_len)
{
    // 重置errno，避免历史错误干扰
    errno = 0;

    if (url == NULL || out_data == NULL || out_len == NULL)
    {
        LOG_ERROR("HTTP GET: 参数非法（url=%p, out_data=%p, out_len=%p）", url, out_data, out_len);
        return RESULT_NETWORK_FUNC_ERR;
    }

    CURL *curl_handle = curl_easy_init();
    if (curl_handle == NULL)
    {
        LOG_ERROR("HTTP GET: Curl初始化失败");
        return RESULT_NETWORK_CURL_INIT_ERR;
    }

    http_memory_buffer_t memory_buffer = {NULL, 0, 0};
    network_result_t final_result = RESULT_NETWORK_FAIL;

    /* 配置参数：短连接超时10s，连接超时30s */
    internal_curl_common_setup(curl_handle, url, 10L, 30L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, internal_write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &memory_buffer);

    CURLcode curl_result = curl_easy_perform(curl_handle);

    if (curl_result != CURLE_OK)
    {
        LOG_CURL_ERROR("HTTP GET: 请求失败, URL=%s", curl_result, url);
        final_result = internal_map_curl_code_to_result(curl_result);
    }
    else
    {
        long http_status_code = 0;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_status_code);

        if (http_status_code == 200)
        {
            *out_data = memory_buffer.data;
            *out_len = memory_buffer.used_size;
            memory_buffer.data = NULL; /* 转移内存所有权，防止被释放 */
            final_result = RESULT_NETWORK_SUCCESS;
            LOG_INFO("HTTP GET: 成功, URL=%s, 数据大小=%zu字节", url, *out_len);
        }
        else
        {
            LOG_ERROR("HTTP GET: 状态码错误, URL=%s, 状态码=%ld", url, http_status_code);
            final_result = internal_map_http_status_to_result(http_status_code);
        }
    }

    /* 资源清理 */
    if (memory_buffer.data != NULL)
    {
        free(memory_buffer.data);
        memory_buffer.data = NULL;
    }
    curl_easy_cleanup(curl_handle);

    return final_result;
}

/**
 * @brief 执行HTTP文件下载
 */
#include <unistd.h>
#include <errno.h>

network_result_t network_http_download_media(void *parameter)
{
    errno = 0;

    network_http_download_param_t *download_param = (network_http_download_param_t *)parameter;
    CURL *curl_handle = NULL;
    FILE *output_file = NULL;
    network_result_t final_result = RESULT_NETWORK_FAIL;

    if (download_param == NULL || download_param->url == NULL || download_param->save_path == NULL)
    {
        LOG_ERROR("HTTP下载: 参数非法（param=%p, url=%p, save_path=%p）",
                  download_param,
                  (download_param ? download_param->url : NULL),
                  (download_param ? download_param->save_path : NULL));
        return RESULT_NETWORK_FUNC_ERR;
    }

    curl_handle = curl_easy_init();
    if (curl_handle == NULL)
    {
        LOG_ERROR("HTTP下载: Curl初始化失败");
        return RESULT_NETWORK_CURL_INIT_ERR;
    }

    // ==== 新增：确保父目录存在 ====
    char *dir_path = strdup(download_param->save_path);
    if (dir_path) {
        char *last_slash = strrchr(dir_path, '/');
        if (last_slash) {
            *last_slash = '\0';
            char tmp[256];
            char *p;
            snprintf(tmp, sizeof(tmp), "%s", dir_path);
            for (p = tmp + 1; *p; p++) {
                if (*p == '/') {
                    *p = '\0';
                    mkdir(tmp, 0755);
                    *p = '/';
                }
            }
            mkdir(tmp, 0755);
        }
        free(dir_path);
    }
    // ==== 新增结束 ====
    
    output_file = fopen(download_param->save_path, "wb");
    if (output_file == NULL)
    {
        LOG_ERROR("HTTP下载: 无法打开文件 %s, errno=%d",
                  download_param->save_path, errno);
        curl_easy_cleanup(curl_handle);
        return RESULT_NETWORK_FILE_ERR;
    }

    internal_curl_common_setup(curl_handle, download_param->url, 300L, 30L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, internal_write_file_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, output_file);

    if (download_param->progress_callback != NULL)
    {
        download_progress_context_t progress_context =
        {
            .user_callback = download_param->progress_callback,
            .user_data = download_param->user_data,
            .last_callback_timestamp = 0,
            .last_reported_percent = 0
        };

        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, internal_progress_callback);
        curl_easy_setopt(curl_handle, CURLOPT_PROGRESSDATA, &progress_context);
    }

    CURLcode curl_result = curl_easy_perform(curl_handle);
    if (curl_result != CURLE_OK)
    {
        LOG_CURL_ERROR("HTTP下载: 失败, URL=%s, 保存路径=%s",
                       curl_result, download_param->url, download_param->save_path);
        final_result = internal_map_curl_code_to_result(curl_result);
        goto EXIT;
    }

    long http_status_code = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_status_code);
    if (http_status_code != 200)
    {
        LOG_ERROR("HTTP下载: 状态码错误, URL=%s, 状态码=%ld",
                  download_param->url, http_status_code);
        final_result = internal_map_http_status_to_result(http_status_code);
        goto EXIT;
    }

    if (fflush(output_file) != 0)
    {
        LOG_ERROR("HTTP下载: fflush失败, 路径=%s, errno=%d",
                  download_param->save_path, errno);
        final_result = RESULT_NETWORK_FILE_ERR;
        goto EXIT;
    }

    if (fsync(fileno(output_file)) != 0)
    {
        LOG_ERROR("HTTP下载: fsync失败, 路径=%s, errno=%d",
                  download_param->save_path, errno);
        final_result = RESULT_NETWORK_FILE_ERR;
        goto EXIT;
    }

    final_result = RESULT_NETWORK_SUCCESS;

    if (download_param->progress_callback != NULL)
    {
        download_param->progress_callback(100, download_param->user_data);
    }

    LOG_INFO("HTTP下载: 成功且已同步到磁盘, 保存路径=%s",
             download_param->save_path);

EXIT:
    if (output_file != NULL)
    {
        if (fclose(output_file) != 0)
        {
            LOG_WARN("HTTP下载: fclose失败, 路径=%s, errno=%d",
                     download_param->save_path, errno);

            if (final_result == RESULT_NETWORK_SUCCESS)
            {
                final_result = RESULT_NETWORK_FILE_ERR;
            }
        }
        output_file = NULL;
    }

    if (curl_handle != NULL)
    {
        curl_easy_cleanup(curl_handle);
        curl_handle = NULL;
    }

    if (final_result != RESULT_NETWORK_SUCCESS)
    {
        if (remove(download_param->save_path) != 0)
        {
            LOG_WARN("HTTP下载: 删除不完整文件失败 %s, errno=%d",
                     download_param->save_path, errno);
        }
        else
        {
            LOG_WARN("HTTP下载: 已删除不完整文件 %s",
                     download_param->save_path);
        }
    }

    return final_result;
}

