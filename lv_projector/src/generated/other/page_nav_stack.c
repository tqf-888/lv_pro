#include "page_nav_stack.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char            name[PAGE_NAV_NAME_MAX];
    lv_obj_t       **scr;
    bool           *scr_del;
    ui_setup_scr_t  setup_scr;
} page_nav_item_t;

static page_nav_item_t g_page_stack[PAGE_NAV_STACK_MAX];
static int g_page_stack_depth = 0;
static bool g_page_nav_home_registered = false;

/*
 * 防止 setup_scr_xxx() 里残留的 page_nav_push() 再次入栈。
 *
 * 新规则：
 * - page_nav_register_home() 只记录桌面。
 * - page_nav_push() 负责正向切屏和入栈。
 * - page_nav_back() 负责返回和出栈。
 * - setup_scr_xxx() 不应该再主动 page_nav_push()。
 */
static bool g_page_nav_loading = false;

extern void ui_load_scr_animation(lv_ui *ui,
                                  lv_obj_t **new_scr,
                                  bool new_scr_del,
                                  bool *old_scr_del,
                                  ui_setup_scr_t setup_scr,
                                  lv_scr_load_anim_t anim_type,
                                  uint32_t time,
                                  uint32_t delay,
                                  bool is_clean,
                                  bool auto_del);

static void page_nav_store_item(page_nav_item_t *item,
                                const char *page_name,
                                lv_obj_t **scr,
                                bool *scr_del,
                                ui_setup_scr_t setup_scr)
{
    memset(item, 0, sizeof(*item));

    snprintf(item->name, sizeof(item->name), "%s", page_name);
    item->scr = scr;
    item->scr_del = scr_del;
    item->setup_scr = setup_scr;
}

int page_nav_register_home(const char *page_name,
                           lv_obj_t **scr,
                           bool *scr_del,
                           ui_setup_scr_t setup_scr)
{
    if (g_page_nav_home_registered || g_page_stack_depth > 0) {
        return PAGE_NAV_ERR_BUSY;
    }

    if (page_name == NULL || page_name[0] == '\0' ||
        scr == NULL || *scr == NULL ||
        scr_del == NULL || setup_scr == NULL) {
        return PAGE_NAV_ERR_PARAM;
    }

    memset(g_page_stack, 0, sizeof(g_page_stack));

    page_nav_store_item(&g_page_stack[0],
                        page_name,
                        scr,
                        scr_del,
                        setup_scr);

    g_page_stack_depth = 1;
    g_page_nav_home_registered = true;

    /*
     * 注册桌面只记录，不跳转。
     * 这里仅修正桌面状态：当前桌面已经存在，所以 del=false。
     */
    *scr_del = false;

    return PAGE_NAV_OK;
}

int page_nav_push(const char *page_name,
                  lv_obj_t **new_scr,
                  bool *new_scr_del,
                  ui_setup_scr_t setup_scr)
{
    page_nav_item_t *old;
    page_nav_item_t *item;

    if (g_page_nav_loading) {
        return PAGE_NAV_OK;
    }

    if (page_name == NULL || page_name[0] == '\0' ||
        new_scr == NULL || new_scr_del == NULL || setup_scr == NULL) {
        return PAGE_NAV_ERR_PARAM;
    }

    if (!g_page_nav_home_registered || g_page_stack_depth <= 0) {
        return PAGE_NAV_ERR_EMPTY;
    }

    if (g_page_stack_depth >= PAGE_NAV_STACK_MAX) {
        return PAGE_NAV_ERR_FULL;
    }

    old = &g_page_stack[g_page_stack_depth - 1];

    if (old->scr_del == NULL) {
        return PAGE_NAV_ERR_PARAM;
    }

    g_page_nav_loading = true;

    ui_load_scr_animation(&guider_ui,
                          new_scr,
                          *new_scr_del,
                          old->scr_del,
                          setup_scr,
                          LV_SCR_LOAD_ANIM_NONE,
                          0,
                          0,
                          0,
                          0);

    g_page_nav_loading = false;

    if (*new_scr == NULL) {
        return PAGE_NAV_ERR_PARAM;
    }

    *new_scr_del = false;

    item = &g_page_stack[g_page_stack_depth];

    page_nav_store_item(item,
                        page_name,
                        new_scr,
                        new_scr_del,
                        setup_scr);

    g_page_stack_depth++;

    return PAGE_NAV_OK;
}

int page_nav_back(void)
{
    page_nav_item_t *cur;
    page_nav_item_t *prev;

    if (g_page_stack_depth <= 1) {
        return PAGE_NAV_ERR_EMPTY;
    }

    cur = &g_page_stack[g_page_stack_depth - 1];
    prev = &g_page_stack[g_page_stack_depth - 2];

    if (cur->scr_del == NULL || prev->scr == NULL ||
        prev->scr_del == NULL || prev->setup_scr == NULL) {
        return PAGE_NAV_ERR_PARAM;
    }

    g_page_nav_loading = true;

    ui_load_scr_animation(&guider_ui,
                          prev->scr,
                          *(prev->scr_del),
                          cur->scr_del,
                          prev->setup_scr,
                          LV_SCR_LOAD_ANIM_NONE,
                          0,
                          0,
                          0,
                          0);

    g_page_nav_loading = false;

    /*
     * prev 已经成为当前页。
     * 如果 prev 原来 del=true，ui_load_scr_animation() 已经 setup_scr(prev)。
     */
    *(prev->scr_del) = false;

    memset(cur, 0, sizeof(*cur));
    g_page_stack_depth--;

    return PAGE_NAV_OK;
}

int page_nav_get_depth(void)
{
    return g_page_stack_depth;
}

const char *page_nav_get_current_name(void)
{
    if (g_page_stack_depth <= 0) {
        return NULL;
    }

    return g_page_stack[g_page_stack_depth - 1].name;
}
