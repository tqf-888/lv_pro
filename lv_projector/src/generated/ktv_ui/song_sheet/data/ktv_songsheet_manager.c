#include "ktv_songsheet_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <cjson/cJSON.h>

#define SONGSHEET_JSON_DIR_LINUX   "/tmp/songsheet_json/"
#define SONGSHEET_JSON_DIR_LVGL    "S:/tmp/songsheet_json/"

#define SONGSHEET_IMG_DIR_LINUX    "/tmp/songsheet/"
#define SONGSHEET_IMG_DIR_LVGL     "S:/tmp/songsheet/"

#define SONGSHEET_FILE_PREFIX      "songsheet_"
#define SONGSHEET_FILE_SUFFIX      ".json"

#define ITEMS_PER_FILE             50
#define MAX_JSON_FILES             7
#define MAX_TOTAL_ITEMS            (MAX_JSON_FILES * ITEMS_PER_FILE)

static int g_pic_available[MAX_TOTAL_ITEMS] = {0};
static cJSON *g_json_cache[MAX_JSON_FILES] = {0};
static pthread_mutex_t g_mgr_lock = PTHREAD_MUTEX_INITIALIZER;

/* ==================== 基础校验 ==================== */

static int is_valid_item_id(int id)
{
    return (id >= 0 && id < MAX_TOTAL_ITEMS) ? 1 : 0;
}

static int is_valid_file_index(int file_index)
{
    return (file_index >= 0 && file_index < MAX_JSON_FILES) ? 1 : 0;
}

static int get_file_and_index(int id, int *file_idx, int *item_idx)
{
    if (file_idx == NULL || item_idx == NULL) {
        return -1;
    }
    if (!is_valid_item_id(id)) {
        return -1;
    }

    *file_idx = id / ITEMS_PER_FILE;
    *item_idx = id % ITEMS_PER_FILE;
    return 0;
}

/* ==================== 路径构建 ==================== */

static int build_json_file_path_linux(int file_idx, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0 || !is_valid_file_index(file_idx)) {
        return -1;
    }

    n = snprintf(buf, size, "%s%s%d%s",
                 SONGSHEET_JSON_DIR_LINUX,
                 SONGSHEET_FILE_PREFIX,
                 file_idx,
                 SONGSHEET_FILE_SUFFIX);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

static int build_json_file_path_lvgl(int file_idx, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0 || !is_valid_file_index(file_idx)) {
        return -1;
    }

    n = snprintf(buf, size, "%s%s%d%s",
                 SONGSHEET_JSON_DIR_LVGL,
                 SONGSHEET_FILE_PREFIX,
                 file_idx,
                 SONGSHEET_FILE_SUFFIX);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

/* ==================== JSON 结构解析 ==================== */

static cJSON *get_songsheet_array(cJSON *root)
{
    if (root == NULL) {
        return NULL;
    }

    if (cJSON_IsArray(root)) {
        return root;
    }

    if (cJSON_IsObject(root)) {
        cJSON *result = cJSON_GetObjectItem(root, "result");
        if (result && cJSON_IsObject(result)) {
            cJSON *songsheet = cJSON_GetObjectItem(result, "songsheet");
            if (songsheet && cJSON_IsArray(songsheet)) {
                return songsheet;
            }
        }
    }

    return NULL;
}

static cJSON *load_json_file_locked(int file_idx)
{
    char file_path[256];
    FILE *fp = NULL;
    char *content = NULL;
    long len;
    cJSON *root = NULL;
    cJSON *songsheet = NULL;
    struct stat st;

    if (!is_valid_file_index(file_idx)) {
        return NULL;
    }

    if (g_json_cache[file_idx] != NULL) {
        return g_json_cache[file_idx];
    }

    if (build_json_file_path_linux(file_idx, file_path, sizeof(file_path)) != 0) {
        return NULL;
    }

    if (stat(file_path, &st) != 0) {
        return NULL;
    }

    fp = fopen(file_path, "rb");
    if (fp == NULL) {
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }

    len = ftell(fp);
    if (len <= 0) {
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return NULL;
    }

    content = (char *)malloc((size_t)len + 1U);
    if (content == NULL) {
        fclose(fp);
        return NULL;
    }

    if (fread(content, 1, (size_t)len, fp) != (size_t)len) {
        free(content);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    content[len] = '\0';

    root = cJSON_Parse(content);
    free(content);
    if (root == NULL) {
        return NULL;
    }

    songsheet = get_songsheet_array(root);
    if (songsheet == NULL) {
        cJSON_Delete(root);
        return NULL;
    }

    g_json_cache[file_idx] = root;
    return root;
}

static cJSON *get_item_json_locked(int id)
{
    int file_idx = 0;
    int item_idx = 0;
    cJSON *root = NULL;
    cJSON *songsheet = NULL;
    int array_size;

    if (get_file_and_index(id, &file_idx, &item_idx) != 0) {
        return NULL;
    }

    root = load_json_file_locked(file_idx);
    if (root == NULL) {
        return NULL;
    }

    songsheet = get_songsheet_array(root);
    if (songsheet == NULL) {
        return NULL;
    }

    array_size = cJSON_GetArraySize(songsheet);
    if (item_idx < 0 || item_idx >= array_size) {
        return NULL;
    }

    return cJSON_GetArrayItem(songsheet, item_idx);
}

static int copy_json_string_field(cJSON *item, const char *field_name, char *buf, size_t size)
{
    cJSON *field = NULL;

    if (buf == NULL || size == 0) {
        return -1;
    }

    buf[0] = '\0';

    if (item == NULL || field_name == NULL) {
        return -1;
    }

    field = cJSON_GetObjectItem(item, field_name);
    if (field == NULL || !cJSON_IsString(field) || field->valuestring == NULL) {
        return -1;
    }

    if (field->valuestring[0] == '\0') {
        return -1;
    }

    if (snprintf(buf, size, "%s", field->valuestring) < 0) {
        buf[0] = '\0';
        return -1;
    }

    return 0;
}

static void copy_json_string_field_or_placeholder(cJSON *item,
                                                  const char *field_name,
                                                  char *buf,
                                                  size_t size)
{
    if (buf == NULL || size == 0U) {
        return;
    }

    if (copy_json_string_field(item, field_name, buf, size) != 0) {
        (void)snprintf(buf, size, "%s", KTV_SONGSHEET_EMPTY_TEXT_PLACEHOLDER);
    }
}

/* ==================== 对外接口 ==================== */

int ktv_get_songsheet_info(int id,
                           char *title_buf, size_t title_size,
                           char *desc_buf, size_t desc_size)
{
    cJSON *item = NULL;
    int rc = 0;

    if (!is_valid_item_id(id)) {
        return -1;
    }

    pthread_mutex_lock(&g_mgr_lock);

    item = get_item_json_locked(id);
    if (item == NULL) {
        pthread_mutex_unlock(&g_mgr_lock);
        return -1;
    }

    if (title_buf != NULL && title_size > 0) {
        copy_json_string_field_or_placeholder(item, "ss_title", title_buf, title_size);
    }

    if (desc_buf != NULL && desc_size > 0) {
        copy_json_string_field_or_placeholder(item, "description", desc_buf, desc_size);
    }

    pthread_mutex_unlock(&g_mgr_lock);
    return rc;
}

int ktv_get_songsheet_pic_url(int id, char *buf, size_t size)
{
    cJSON *item = NULL;
    int rc;

    if (buf == NULL || size == 0 || !is_valid_item_id(id)) {
        return -1;
    }

    pthread_mutex_lock(&g_mgr_lock);

    item = get_item_json_locked(id);
    if (item == NULL) {
        pthread_mutex_unlock(&g_mgr_lock);
        buf[0] = '\0';
        return -1;
    }

    rc = copy_json_string_field(item, "pic_url", buf, size);

    pthread_mutex_unlock(&g_mgr_lock);
    return rc;
}

/*
 * 给下载层 / fopen / stat / rename 用
 * 必须返回 Linux 路径，不能带 A:
 */
int app_data_make_songsheet_pic_path(int id, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0 || !is_valid_item_id(id)) {
        return -1;
    }

    n = snprintf(buf, size, "%s%d.png", SONGSHEET_IMG_DIR_LINUX, id);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

/*
 * 给 LVGL 显示层用
 * 返回 S:/ 前缀路径
 */
int app_data_make_songsheet_pic_lvgl_path(int id, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0 || !is_valid_item_id(id)) {
        return -1;
    }

    n = snprintf(buf, size, "%s%d.png", SONGSHEET_IMG_DIR_LVGL, id);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}

/*
 * 兼容旧接口：
 * 如果别处拿 json 文件路径做普通文件读取，就必须返回 Linux 路径
 */
int ktv_get_songsheet_json_path(int file_index, char *buf, size_t size)
{
    return build_json_file_path_linux(file_index, buf, size);
}

/*
 * 如果以后 LVGL 需要直接读 json，可用这个
 */
int ktv_get_songsheet_json_lvgl_path(int file_index, char *buf, size_t size)
{
    return build_json_file_path_lvgl(file_index, buf, size);
}

int ktv_set_songsheet_pic_available(int id, int available)
{
    if (!is_valid_item_id(id)) {
        return -1;
    }

    pthread_mutex_lock(&g_mgr_lock);
    g_pic_available[id] = (available != 0) ? 1 : 0;
    pthread_mutex_unlock(&g_mgr_lock);
    return 0;
}

/*
 * 保持旧语义：
 * - 成功后清掉 available 标记
 *
 * 注意：
 * 这里返回给显示层的应该是 LVGL 路径
 */
int ktv_get_songsheet_pic_path(int id, char *buf, size_t size)
{
    int rc;

    if (buf == NULL || size == 0 || !is_valid_item_id(id)) {
        return -1;
    }

    pthread_mutex_lock(&g_mgr_lock);

    if (g_pic_available[id] != 1) {
        pthread_mutex_unlock(&g_mgr_lock);
        return -1;
    }

    rc = app_data_make_songsheet_pic_lvgl_path(id, buf, size);
    if (rc == 0) {
        g_pic_available[id] = 0;
    }

    pthread_mutex_unlock(&g_mgr_lock);
    return rc;
}