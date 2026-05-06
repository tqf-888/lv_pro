#include "songsheet_page_demo.h"

#include <string.h>

#include "ktv_ctrl.h"
#include "lv_renderer_generic.h"

int songsheet_page_demo_open(songsheet_page_demo_t *page,
                             lv_obj_t *parent,
                             uint32_t total_count,
                             uint32_t page_size,
                             const lv_songsheet_view_style_t *view_style,
                             const void *placeholder_img_src,
                             const void *failed_img_src)
{
    int ret;

    if (page == NULL || parent == NULL || total_count == 0U || page_size == 0U || view_style == NULL) {
        return -1;
    }

    memset(page, 0, sizeof(*page));

    ret = Ktv_Ctrl_Init();
    if (ret != 0) {
        return -2;
    }

    page->root = lv_obj_create(parent);
    if (page->root == NULL) {
        Ktv_Ctrl_Deinit();
        return -3;
    }

    lv_obj_set_size(page->root, view_style->viewport_width, view_style->viewport_height);
    lv_obj_set_pos(page->root, 0, 0);
    lv_obj_clear_flag(page->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(page->root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(page->root, 0, 0);
    lv_obj_set_style_border_width(page->root, 0, 0);
    lv_obj_set_style_radius(page->root, 0, 0);
    lv_obj_set_style_bg_opa(page->root, LV_OPA_TRANSP, 0);

    ret = lv_songsheet_adapter_start(&page->adapter,
                                     page->root,
                                     view_style,
                                     total_count,
                                     page_size,
                                     placeholder_img_src,
                                     failed_img_src,
                                     &g_lv_renderer_generic_ops,
                                     NULL);
    if (ret != 0) {
        lv_obj_del(page->root);
        page->root = NULL;
        Ktv_Ctrl_Deinit();
        return -4;
    }

    return 0;
}

void songsheet_page_demo_close(songsheet_page_demo_t *page)
{
    if (page == NULL) {
        return;
    }

    lv_songsheet_adapter_stop(&page->adapter);

    if (page->root != NULL) {
        lv_obj_del(page->root);
        page->root = NULL;
    }

    Ktv_Ctrl_Deinit();
    memset(page, 0, sizeof(*page));
}


#include "songsheet_page_demo.h"
#include "lv_songsheet_view_style.h"
#include "lv_songsheet_style_3x3.h"
#include "lv_songsheet_style_1x5.h"
#include "lv_songsheet_style_2x3.h"
#include "lv_songsheet_style_3x2.h"

static songsheet_page_demo_t g_page;

static void app_ui_init(void)
{


    static int initial_flag = 0;
    if(initial_flag == 0)
    {
        lv_songsheet_styles_init();
        lv_songsheet_style_2x3_init();
        lv_songsheet_style_3x3_init();
        lv_songsheet_style_1x5_init();
        lv_songsheet_style_3x2_init();
        initial_flag ++;
    }
}

void demo_app_songsheet_list_tmp(lv_obj_t *parent)
{
    int ret;
    app_ui_init();
    ret = songsheet_page_demo_open(&g_page,
                                   parent,
                                   300,
                                   50,
                                   &g_lv_songsheet_style_3x2,
                                   NULL,
                                   NULL);
    if (ret != 0) {
        printf("open failed:%d\n", ret);
    }
}

void app_close_songsheet_page(void)
{
    songsheet_page_demo_close(&g_page);
}