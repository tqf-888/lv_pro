#ifndef LV_WIFI_LIST_DEMO_H
#define LV_WIFI_LIST_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

void app_wifi_list_open(lv_obj_t *current_screen);
void app_wifi_list_close(void);
void app_wifi_list_refresh(void);
void app_wifi_list_refresh_connected(void);
void app_wifi_list_set_password(const char *password);

#ifdef __cplusplus
}
#endif

#endif /* LV_WIFI_LIST_DEMO_H */
