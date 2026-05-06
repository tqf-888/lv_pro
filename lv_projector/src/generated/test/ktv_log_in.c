#include "ktv_log_in.h"
#include "ktv.h"
#include "network_http_download.h"
#include "gui_guider.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义静态变量存储结果
static char s_phone_num[256] = {0};
static char account_check_response[2048] = {0};
static char api_token[256] = {0};

// 获取验证码
static void get_verification_code() {
    char *response_data = NULL;
    size_t response_len = 0;
    char final_url[2048];

    // 构建请求URL
    ktv_build_base_url(KTV_4_34_1_SEND_CODE, final_url, sizeof(final_url), s_phone_num);
    dbg_print("%s",final_url);
    // 发起HTTP GET请求以获取验证码
    int result = network_http_get(final_url, &response_data, &response_len);
    if (result == RESULT_NETWORK_SUCCESS) {
        // 释放内存
        if (response_data != NULL) {
            free(response_data);
            response_data = NULL;
        }
    }
}

// 通过手机号检查账户
static char* check_account(const char *verification_code, const char *phone_num) {
    char *response_data = NULL;
    size_t response_len = 0;
    char final_url[2048];

    // 构建请求URL，传入验证码
    ktv_build_base_url(KTV_4_34_2_CHECK_ACCOUNT_BY_PHONE, final_url, sizeof(final_url), 1000000, phone_num, verification_code);

    // 发起HTTP GET请求检查账户
    int result = network_http_get(final_url, &response_data, &response_len);
    if (result == RESULT_NETWORK_SUCCESS) {
        printf("获取成功，长度: %zu 字节\n", response_len);
        printf("内容摘要: %s\n", response_data);

        // 存储账户检查响应到静态变量
        strncpy(account_check_response, response_data, sizeof(account_check_response) - 1);

        // 释放内存
        if (response_data != NULL) {
            free(response_data);
            response_data = NULL;
        }
    }

    return account_check_response;
}

// 获取API令牌
static char* _get_api_token() {
    char *response_data = NULL;
    size_t response_len = 0;
    char final_url[2048];

    // 构建请求URL以获取API令牌
    ktv_build_base_url(KTV_SIGN_TOKEN_URL, final_url, sizeof(final_url));

    // 发起HTTP GET请求以获取API令牌
    int result = network_http_get(final_url, &response_data, &response_len);
    if (result == RESULT_NETWORK_SUCCESS) {
        printf("获取成功，长度: %zu 字节\n", response_len);
        printf("API Token: %.50s...\n", response_data);

        // 存储API令牌到静态变量
        strncpy(api_token, response_data, sizeof(api_token) - 1);

        // 释放内存
        if (response_data != NULL) {
            free(response_data);
            response_data = NULL;
        }
    }

    return api_token;
}


// 获取API令牌
char* get_api_token() {
    _get_api_token();
    return api_token;
}

void set_verification_code(const char *code) {
    char str[50];
    strncpy(str, code, sizeof(str) - 1);
    check_account(str, s_phone_num);
}

void set_phone_num(const char *phone_num) {
    dbg_print("%s",phone_num);
    strncpy(s_phone_num, phone_num, sizeof(s_phone_num) - 1);
    get_verification_code();
}

