#ifndef LV_LINUX_FOLDER_DEMO_H
#define LV_LINUX_FOLDER_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

void demo_open_folder(lv_obj_t *parent);
void demo_close_folder(void);

void test_open_folder_browser(lv_obj_t *parent);
void test_close_folder_browser(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_LINUX_FOLDER_DEMO_H */
