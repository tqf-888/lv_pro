#include "ktv.h"
#include "md5_impl.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ================= 数据结构 ================= */

typedef struct {
    char key[MAX_URL_LEN];
    char value[MAX_URL_LEN];
} param_pair_t;

/* ================= 功能函数拆分 ================= */

// 1. 生成 signstr
static int generate_signstr(const char *sign_token, const char *key, char *out_signstr, size_t out_size) {
    if (!sign_token || !key || !out_signstr || out_size < 6) return -1;

    char buffer[MAX_URL_LEN];
    char md5_result[MY_MD5_HEX_LEN + 1];

    snprintf(buffer, sizeof(buffer), "%s%s", sign_token, key);
    my_md5_hex_string(buffer, md5_result);

    memcpy(out_signstr, md5_result, 5);
    out_signstr[5] = '\0';
    return 0;
}

// 2. 解析参数
static int parse_params(const char *url, param_pair_t params[], int max_count) {
    if (!url || !params || max_count <= 0) return -1;

    const char *query_start = strchr(url, '?');
    if (!query_start) return -1; // 必须包含 ?
    query_start++;

    char query_str[MAX_URL_LEN];
    strncpy(query_str, query_start, sizeof(query_str) - 1);
    query_str[sizeof(query_str) - 1] = '\0';

    char *hash = strchr(query_str, '#');
    if (hash) *hash = '\0';

    int count = 0;
    char *saveptr = NULL;
    char *token = strtok_r(query_str, "&", &saveptr);

    while (token && count < max_count) {
        char *eq = strchr(token, '=');
        if (eq) {
            *eq = '\0';
            snprintf(params[count].key, sizeof(params[count].key), "%s", token);
            snprintf(params[count].value, sizeof(params[count].value), "%s", eq + 1);
            count++;
        }
        token = strtok_r(NULL, "&", &saveptr);
    }
    return count;
}

// 3. 排序比较
static int compare_params(const void *a, const void *b) {
    return strcmp(((param_pair_t*)a)->key, ((param_pair_t*)b)->key);
}

// 4. 构建待签名字符串
// 【修改点】移除了 signstr 参数，因为 signstr 已经在 params 数组里了
static int build_sign_source(param_pair_t params[], int count, 
                             const char *sign_token, 
                             char *out_source, size_t out_size) {
    if (!params || !sign_token || !out_source) return -1;

    // 排序：此时 params 里已经包含了 signstr
    qsort(params, count, sizeof(param_pair_t), compare_params);

    char temp_buf[MAX_URL_LEN * 2];
    int offset = 0;

    for (int i = 0; i < count; i++) {
        offset += snprintf(temp_buf + offset, sizeof(temp_buf) - offset, 
                           "%s=%s", params[i].key, params[i].value);
        if (i < count - 1) {
            offset += snprintf(temp_buf + offset, sizeof(temp_buf) - offset, "&");
        }
    }

    // 【修改点】不再手动追加 &signstr=...，因为它已经在循环里排序输出了

    if ((size_t)offset >= sizeof(temp_buf)) return -1;

    for (char *p = temp_buf; *p; ++p) *p = tolower((unsigned char)*p);

    snprintf(out_source, out_size, "%s%s", temp_buf, sign_token);
    return 0;
}

// 5. 生成最终签名
static int generate_final_sign(const char *source, char *out_sign, size_t out_size) {
    if (!source || !out_sign || out_size < MY_MD5_HEX_LEN + 1) return -1;
    my_md5_hex_string(source, out_sign);
    return 0;
}

/* ================= 主入口函数 ================= */

int ktv_build_full_url(char *base_url, size_t buf_size, 
                     const char *sign_token, 
                     const char *key) 
{
    if (!base_url || !sign_token || !key) return -1;

    param_pair_t params[MAX_PARAM_NUM];
    char signstr[6] = {0};
    char sign_source[MAX_URL_LEN * 2];
    char sign[MY_MD5_HEX_LEN + 1];

    // 1. 生成 signstr
    if (generate_signstr(sign_token, key, signstr, sizeof(signstr)) != 0) return -1;

    // 2. 解析参数
    int count = parse_params(base_url, params, MAX_PARAM_NUM);
    if (count < 0) return -1;

    // 【关键修改】3. 将 signstr 添加到参数列表中
    // 这样 signstr 就会参与接下来的字母排序
    if (count >= MAX_PARAM_NUM) return -1; // 防止溢出
    snprintf(params[count].key, sizeof(params[count].key), "signstr");
    snprintf(params[count].value, sizeof(params[count].value), "%s", signstr);
    count++;

    // 4. 构建待签名源串 (传入修改后的 params 和 count)
    if (build_sign_source(params, count, sign_token, sign_source, sizeof(sign_source)) != 0) return -1;

    // 5. 计算 sign
    if (generate_final_sign(sign_source, sign, sizeof(sign)) != 0) return -1;

    // 6. 原地追加
    int cur_len = strlen(base_url);
    int remaining = buf_size - cur_len;

    if (remaining <= 50) return -1; 

    // 最终拼接：虽然 signstr 参与了签名计算，但 URL 里还是照常追加
    snprintf(base_url + cur_len, remaining, "&signstr=%s&sign=%s", signstr, sign);

    return 0;
}
