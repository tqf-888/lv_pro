#include "page_manager.h"
#include <string.h>
#include <stdio.h>   // 添加 printf 支持
#include "ktv.h"

static int current_page = IGNORE_NUM;
static int current_sub = IGNORE_NUM;
static int current_singer_id = IGNORE_NUM;
static char current_name[64] = {0};

void page_manager_init(void)
{
    printf("[page_manager] page_manager_init()\n");
    current_page = IGNORE_NUM;
    current_sub = IGNORE_NUM;
    current_singer_id = IGNORE_NUM;
    current_name[0] = '\0';
    extern void set_order_num(int num) ;
    set_order_num(0);
}

void page_set(int page) {
    printf("[page_manager] page_set(%d)\n", page);
    current_page = page;
    printf("[page_manager] current_page now = %d\n", current_page);
}

int page_get(void) {
    printf("[page_manager] page_get() -> %d\n", current_page);
    return current_page;
}

void subpage_set(int sub) {
    printf("[page_manager] subpage_set(%d)\n", sub);
    current_sub = sub;
    printf("[page_manager] current_sub now = %d\n", current_sub);
}

int subpage_get(void) {
    printf("[page_manager] subpage_get() -> %d\n", current_sub);
    return current_sub;
}

void singer_id_set(int singer_id) {
    current_singer_id = singer_id;
    printf("[page_manager] current_singer_id now = %d\n", current_singer_id);
}

int singer_id_get(void) {
    printf("[page_manager] singer_id_get() -> %d\n", current_singer_id);
    return current_singer_id;
}

void name_set(const char *name) {
    printf("[page_manager] name_set('%s')\n", name ? name : "(null)");
    if (name) {
        strncpy(current_name, name, sizeof(current_name) - 1);
        current_name[sizeof(current_name) - 1] = '\0';
    } else {
        current_name[0] = '\0';
    }
    printf("[page_manager] current_name now = '%s'\n", current_name);
}

const char* name_get(void) {
    printf("[page_manager] name_get() -> '%s'\n", current_name);
    return current_name;
}