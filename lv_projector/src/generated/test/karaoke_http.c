#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "test/ktv.h"
#include "network_http_download.h"
#include "cJSON.h"
#include "ktv_token_management.h"

#define KTV_SIGN_KEY   "e038dfbf841e3aac89cb2efa82e9d854"


/************************* 工具函数：获取参数（环境变量优先） *************************/
const char* ktv_get_param(const char* env_name, const char* default_val) {
    const char* env_val = getenv(env_name);
    return (env_val != NULL && env_val[0] != '\0') ? env_val : default_val;
}

/************************* 核心函数：拼接公共参数字符串 *************************/
void ktv_build_common_params(char* params_buf, int buf_size) {
    // 1. 获取基础参数（环境变量优先）
    const char* channel_version = ktv_get_param(KTV_ENV_CHANNEL_VERSION, KTV_DEFAULT_CHANNEL_VERSION);
    const char* channel_id = ktv_get_param(KTV_ENV_CHANNEL_ID, KTV_DEFAULT_CHANNEL_ID);
    const char* device_id = ktv_get_param(KTV_ENV_DEVICE_ID, KTV_DEFAULT_DEVICE_ID);
    const char* mac = ktv_get_param(KTV_ENV_MAC, KTV_DEFAULT_MAC);

    // 2. 生成实时时间戳并转字符串
    long long request_time_num = time(NULL);
    char request_time_str[20] = {0};
    snprintf(request_time_str, sizeof(request_time_str), "%lld", 1773363373);

    // 3. 拼接公共参数
    snprintf(params_buf, buf_size, KTV_PARAMS_FORMAT,
             channel_version, channel_id, device_id, mac, request_time_str);
}

/**
 * @brief  构建KTV请求基础URL（自动剔除无效参数,最后签名）
 * @param  api_format: API接口定义字符串 (如 "/sdkv2/song/search?name=%s&tag_id=%d")
 * @param  final_url: 最终生成的完整URL缓冲区
 * @param  url_buf_size: 缓冲区大小
 * @param  ...: 可变参数，顺序与api_format中的占位符一一对应
 *
 * @note   处理逻辑：
 *         1. 提取定义：解析api_format，提取基础路径与参数键名(key)。
 *         2. 剔除无效值：
 *            - 字符串参数(%s)：若传入NULL或空字符串("")，剔除该键值对。
 *            - 整型参数(%d)：若传入0xffff，剔除该键值对。
 *         3. 生成URL：按顺序拼接 [域名] + [路径] + [有效自定义参数] + [公共参数]。
 *         4. 自动清洗：最后修正连接符，防止出现 "?&" 或 "&&" 的情况。
 *
 * @return 0:成功 -1:失败
 */

 int ktv_build_base_url(const char* api_format, char* final_url, int url_buf_size, ...) {
    if (!api_format || !final_url || url_buf_size <= 0) return -1;

    char fmt_dup[1024];
    strncpy(fmt_dup, api_format, sizeof(fmt_dup) - 1);
    fmt_dup[sizeof(fmt_dup) - 1] = '\0';

    char *base_path = fmt_dup;
    char *query_start = strchr(fmt_dup, '?');
    
    char temp_params[2048] = {0};
    int temp_len = 0;

    va_list args;
    va_start(args, url_buf_size);

    if (query_start) {
        *query_start = '\0'; 
        char *p = query_start + 1;

        while (p && *p) {
            char *key = p;
            char *eq = strchr(key, '=');
            if (!eq) break;

            char *next_delim = strchr(key, '&');

            // 1. 截取 Key
            *eq = '\0'; 
            
            // 2. 关键修复：截取 Format (防止 strstr 匹配到后面的参数)
            if (next_delim) *next_delim = '\0'; 
            char *fmt_val = eq + 1;

            // 3. 判断类型
            int is_string = (strstr(fmt_val, "%s") != NULL);
            int skip = 0;
            char val_buf[512] = {0};

            if (is_string) {
                char *str_val = va_arg(args, char*);
                if (!str_val || strlen(str_val) == 0) skip = 1;
                else snprintf(val_buf, sizeof(val_buf), "%s", str_val);
            } else {
                int int_val = va_arg(args, int);
                if (int_val == IGNORE_NUM) skip = 1;
                else snprintf(val_buf, sizeof(val_buf), "%d", int_val);
            }

            // 4. 拼接
            if (!skip) {
                temp_len += snprintf(temp_params + temp_len, sizeof(temp_params) - temp_len, "&%s=%s", key, val_buf);
            }

            // 5. 移动指针
            p = next_delim ? (next_delim + 1) : NULL;
        }
    }

    va_end(args);

    // 6. 生成公共参数
    char common_params[512] = {0};
    ktv_build_common_params(common_params, sizeof(common_params));

    // 7. 整体拼接
    snprintf(final_url, url_buf_size, "%s%s?%s%s", 
             KTV_DOMAIN_URL, base_path, temp_params, common_params);

    // 8. 清洗多余的 &
    char *q_pos = strchr(final_url, '?');
    if (q_pos && *(q_pos + 1) == '&') {
        memmove(q_pos + 1, q_pos + 2, strlen(q_pos + 2) + 1);
    }

    //sign
    ktv_build_full_url(final_url, url_buf_size,ktv_get_token(), KTV_SIGN_KEY);
    dbg_print("拼接完成！最终URL：\n%s", final_url);
    return 0;
}












/* =============================================================================
 * 自定义回调函数示例
 * ============================================================================= */

/**
 * 示例1：简单的进度打印回调
 */
int simple_progress_callback(int percent, void *user_data)
{
    printf("当前下载进度: %d%%\n", percent);
    
    /* 返回 0 表示继续下载 */
    return 0;
}

/**
 * 示例2：带用户数据的回调 (演示如何中断下载)
 */
typedef struct
{
    int cancel_at_percent; /* 在达到此百分比时取消下载 */
} my_download_context_t;

int smart_progress_callback(int percent, void *user_data)
{
    my_download_context_t *ctx = (my_download_context_t *)user_data;
    
    printf("[智能回调] 进度: %d%%\n", percent);
    
    /* 如果达到指定进度，返回非0值中断下载 */
    if (ctx->cancel_at_percent > 0 && percent >= ctx->cancel_at_percent)
    {
        printf("[警告] 用户在 %d%% 处取消下载！\n", percent);
        return 1; /* 返回非0，下载将中止 */
    }
    
    return 0;
}




int get_song_info(int songid, char *user_token, char *buf, int buf_len)
{
    char *response_data = NULL;
    size_t response_len = 0;
    char final_url[2048];
    int result;
    cJSON *root = NULL;
    cJSON *result_obj = NULL;
    cJSON *songinfo = NULL;
    cJSON *mv_url = NULL;
    int ret = 0;

    if (user_token == NULL || buf == NULL || buf_len <= 0)
    {
        return -1;
    }

    buf[0] = '\0';

    ktv_build_base_url(KTV_4_30_GET_SONG_INFO,
                       final_url,
                       sizeof(final_url),
                       songid,
                       user_token,
                       326);

    result = network_http_get(final_url, &response_data, &response_len);
    if (result != RESULT_NETWORK_SUCCESS)
    {
        goto EXIT;
    }

    if (response_data == NULL || response_len == 0)
    {
        goto EXIT;
    }

    root = cJSON_Parse(response_data);
    if (root == NULL)
    {
        goto EXIT;
    }

    result_obj = cJSON_GetObjectItem(root, "result");
    if (result_obj == NULL)
    {
        goto EXIT;
    }

    songinfo = cJSON_GetObjectItem(result_obj, "songinfo");
    if (songinfo == NULL)
    {
        goto EXIT;
    }

    mv_url = cJSON_GetObjectItem(songinfo, "mv_url");
    if (mv_url == NULL || !cJSON_IsString(mv_url) || mv_url->valuestring == NULL)
    {
        goto EXIT;
    }

    if (strncmp(mv_url->valuestring, "https://", 8) == 0)
    {
        snprintf(buf, buf_len, "http://%s", mv_url->valuestring + 8);
        ret = 1;
    }
    else
    {
        snprintf(buf, buf_len, "%s", mv_url->valuestring);
    }

EXIT:
    if (root != NULL)
    {
        cJSON_Delete(root);
        root = NULL;
    }

    if (response_data != NULL)
    {
        free(response_data);
        response_data = NULL;
    }

    return ret;
}