#include "ktv_log_in.h"
#include "ktv.h"
#include "ktv_http_fetch_api.h"
#include "gui_guider.h"
#include "page_nav_stack.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// 定义静态变量存储结果
static char s_phone_num[256] = {0};
static char usr_token[2048] = {0};

#define USR_TOKEN_FILE_PATH "/usr/share/lv_projector/usr_token.txt"

static int normalize_usr_token(const char *input_str)
{
    cJSON *root = NULL;
    cJSON *code_obj = NULL;
    cJSON *data_obj = NULL;
    cJSON *result_obj = NULL;
    cJSON *token_obj = NULL;
    int ret = 0;

    if (input_str == NULL || input_str[0] == '\0') {
        return 0;
    }

    if (input_str[0] != '{') {
        snprintf(usr_token, sizeof(usr_token), "%s", input_str);
        return (usr_token[0] != '\0') ? 1 : 0;
    }

    root = cJSON_Parse(input_str);
    if (root == NULL) {
        printf("解析 token 响应失败\n");
        return 0;
    }

    code_obj = cJSON_GetObjectItem(root, "code");
    if (code_obj == NULL || !cJSON_IsNumber(code_obj) || code_obj->valueint != 0) {
        printf("token 响应 code 非 0\n");
        goto EXIT;
    }

    data_obj = cJSON_GetObjectItem(root, "data");
    if (data_obj != NULL && cJSON_IsObject(data_obj)) {
        token_obj = cJSON_GetObjectItem(data_obj, "token");
    }

    if (token_obj == NULL) {
        result_obj = cJSON_GetObjectItem(root, "result");
        if (result_obj != NULL && cJSON_IsObject(result_obj)) {
            token_obj = cJSON_GetObjectItem(result_obj, "token");
        }
    }

    if (token_obj == NULL || !cJSON_IsString(token_obj) || token_obj->valuestring == NULL ||
        token_obj->valuestring[0] == '\0') {
        printf("token 响应缺少有效 token\n");
        goto EXIT;
    }

    snprintf(usr_token, sizeof(usr_token), "%s", token_obj->valuestring);
    ret = 1;

EXIT:
    cJSON_Delete(root);
    return ret;
}

static void save_usr_token_to_file(void)
{
    FILE *fp;
    size_t token_len;

    fp = fopen(USR_TOKEN_FILE_PATH, "wb");
    if (fp == NULL) {
        printf("打开 token 文件失败: %s\n", USR_TOKEN_FILE_PATH);
        return;
    }

    token_len = strlen(usr_token);
    if (token_len > 0U) {
        if (fwrite(usr_token, 1, token_len, fp) != token_len) {
            printf("写入 token 文件失败: %s\n", USR_TOKEN_FILE_PATH);
        }
    }

    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
}

static void jump_to_log_in_page_async(void *arg)
{
    (void)arg;

    page_nav_push("screen_log_in",
                  &guider_ui.screen_log_in,
                  &guider_ui.screen_log_in_del,
                  setup_scr_screen_log_in);
}

static void jump_to_log_in_page(void)
{
    lv_async_call(jump_to_log_in_page_async, NULL);
}

void load_usr_token_from_file(void)
{
    FILE *fp;
    size_t read_len;
    char file_buf[2048];

    usr_token[0] = '\0';
    file_buf[0] = '\0';

    fp = fopen(USR_TOKEN_FILE_PATH, "rb");
    if (fp == NULL) {
        fp = fopen(USR_TOKEN_FILE_PATH, "wb");
        if (fp != NULL) {
            fclose(fp);
        }
        jump_to_log_in_page();
        return;
    }

    read_len = fread(file_buf, 1, sizeof(file_buf) - 1U, fp);
    fclose(fp);
    file_buf[read_len] = '\0';

    while (read_len > 0U &&
           (file_buf[read_len - 1U] == '\r' || file_buf[read_len - 1U] == '\n')) {
        file_buf[read_len - 1U] = '\0';
        read_len--;
    }

    if (!normalize_usr_token(file_buf)) {
        usr_token[0] = '\0';
    } else if (strcmp(file_buf, usr_token) != 0) {
        save_usr_token_to_file();
    }

    if (usr_token[0] == '\0') {
        jump_to_log_in_page();
    }
}

const char *get_usr_token(void)
{
    return usr_token;
}


// 获取验证码
static void get_verification_code() {
    char response_data[512];
    char final_url[2048];

    // 构建请求URL
    ktv_build_base_url(KTV_4_34_1_SEND_CODE, final_url, sizeof(final_url), s_phone_num);
    dbg_print("%s",final_url);
    response_data[0] = '\0';

    // 发起HTTP GET请求以获取验证码
    (void)ktv_http_get_to_memory(final_url, response_data, sizeof(response_data), 10000);
}

// 通过手机号检查账户
static char* check_account(const char *verification_code, const char *phone_num) {
    char final_url[2048];
    char response_buf[2048];

    // 构建请求URL，传入验证码
    ktv_build_base_url(KTV_4_34_2_CHECK_ACCOUNT_BY_PHONE, final_url, sizeof(final_url), 1000000, phone_num, verification_code);

    usr_token[0] = '\0';
    response_buf[0] = '\0';

    // 发起HTTP GET请求检查账户
    if (ktv_http_get_to_memory(final_url,
                               response_buf,
                               sizeof(response_buf),
                               10000) != 0) {
        printf("获取成功，长度: %zu 字节\n", strlen(response_buf));
        if (normalize_usr_token(response_buf)) {
            printf("提取 token 成功: %s\n", usr_token);
            save_usr_token_to_file();
        }
    }

    return usr_token;
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

