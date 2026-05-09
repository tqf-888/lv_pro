#include "auto_wifi.h"
#include "lv_pro_launcher.h"
#include "widget/lv_pro_res.h"
#include "widget/lv_pro_set_btn_style1.h"
#include "Media/lv_pro_media.h"
#include "Source/lv_pro_source.h"
#include "WiredSP/lv_pro_wiredsp.h"
#include "WirelessSP/lv_pro_wirelesssp.h"
#include "Setting/lv_pro_setting.h"
#include "Network/lv_pro_wifi_activity.h"
#include "Bluetooth/lv_pro_bt_activity.h"
#include "AWCast/awcast.h"
#include "usbcast.h"
#include "Layer/focus/lv_ui_focuswindow.h"
#include "Layer/volume/volume.h"
#include "Layer/wifi_bt/lv_ui_wifi.h"
#include "Layer/usb/lv_ui_usb.h"
#include "Common/setting/system_setting.h"
#include "Common/language/string/lv_string_id.h"
#include "lv_common.h"
#include "sys_param.h"
#include "page.h"
#include "key_event.h"
// #include "brightness_window.h"
#if USE_BSD_EVDEV
#include <dev/evdev/input.h>
#else
#include <linux/input.h>
#endif
#include "auto_wifi.h"
// #include "wifi_common.h"
#include "NetWork_WIFI_Function.h"
#include "ktv_ctrl.h"

extern load_wifi_module(const char *script_path);
// Internal thread function (static)
static void* _auto_wifi_start_proc(void* arg)
{
    printf("\n[AutoWi-Fi] 1. Loading Wi-Fi kernel modules via script...\n");

    char* script_path = get_wireless_ko_path();
    if (!script_path) {
        printf("[AutoWi-Fi] Error: Failed to get module script path!\n");
        return NULL;
    }

    if (load_wifi_module(script_path) != 0) {
        printf("[AutoWi-Fi] Error: Failed to load Wi-Fi modules!\n");
        free(script_path);
        return NULL;
    }
    free(script_path);
    update_file_wifi_state(WIFI_STATE_ON);
    printf("[AutoWi-Fi] 2. Checking Wi-Fi enable state...\n");
    if (get_file_wifi_state() != 1) {
        printf("[AutoWi-Fi] Wi-Fi is disabled in config. Skipping init.\n");
        return NULL;
    }

    printf("[AutoWi-Fi] 3. Initializing Wi-Fi...\n");
    if (lv_pro_res_wifi_init() != 0) {
        WIFI_ERR("Wi-Fi init failed!\n");
        return NULL;
    }

    printf("[AutoWi-Fi] 4. Enabling STA mode...\n");
    if (lv_pro_res_wifi_on() != 0) {
        WIFI_ERR("Wi-Fi STA mode enable failed!\n");
        lv_pro_res_wifi_deinit();
        return NULL;
    }

    Ktv_Ctrl_Init();
    pthread_exit((void *)1);
    return NULL;
}

int auto_wifi_start(void)
{
    printf("\n\n\n\n\n\n"
           "****************************************************************************************************\n"
           " Starting Auto Wi-Fi Initialization Thread\n"
           "****************************************************************************************************\n");

    pthread_t tid;
    if (pthread_create(&tid, NULL, _auto_wifi_start_proc, NULL) != 0) {
        printf("[AutoWi-Fi] Failed to create thread!\n");
        return -1;
    }
    pthread_detach(tid);
    return 0;
}



// void *_wifi_proc()
// {
//     while(1){
//     switch () {
//     case WIFI_SRART:
//         /* 初始化 Wi-Fi 模块 */
//         _auto_wifi_start();
//         break;

//     case WIFI_SCAN:
//         /* 启动扫描并刷新列表 */
//         break;

//     case WIFI_CONNECT:
//         /* 根据选中的 AP 进行连接 */
//         break;
// }
//     }
// }