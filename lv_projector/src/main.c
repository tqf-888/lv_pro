
#include "lv_drivers/display/sunxifb.h"
#include "lv_drivers/indev/evdev.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lv_pro_launcher.h"
#include "widget/lv_pro_res.h"
#include "System/system_api.h"
#include "sys_param.h"
#include "lv_common.h"
#include "awcast.h"
#include <signal.h>
#include "page.h"
#include <ucontext.h>
#include <dlfcn.h>
#include "gui_guider.h"
#include "events_init.h"
#include "NetWork_WIFI_Function.h"
#include "network_http_download.h"
#include "my_lv_pro_res_media_player_int.h"
#include "ktv.h"
#include "auto_wifi.h"
/* 包含我们的消息模块 */

#include "page_ui.h"

#include "ktv_player_ui.h"
#include "lvgl_page_navigation.h"

#include "db_list_pro_worker.h"
#include "db_list_pro_lvgl.h"
#include "mqtt_layer.h"


extern lv_obj_t *launcher_activity;
lv_indev_t *evdev_indev;
lv_indev_drv_t indev_drv;
lv_ui guider_ui;
static void disable_bootlogo(void)
{
    system("./usr/bin/kill_yuview.sh");
}

static void keypad_int()
{
    evdev_init();
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*See below.*/
    indev_drv.read_cb = evdev_read;         /*See below.*/
    /*Register the driver in LVGL and save the created input device object*/
    evdev_indev = lv_indev_drv_register(&indev_drv);

    register_global_key(do_global_event);
}

void print_stack(ucontext_t *uc)
{
    if (!uc)
    {
        printf("No valid context provided.\n");
        return;
    }

    // Extract program counter (PC) and stack pointer (SP) from context
    void *pc = (void *)uc->uc_mcontext.__gregs[REG_PC];
    void *sp = (void *)uc->uc_mcontext.__gregs[REG_SP];

    printf("Program counter: %p\n", pc);
    printf("Stack pointer: %p\n", sp);

    // Traverse the stack and print addresses
    void **stack = (void **)sp;
    for (int i = 0; i < 10 && stack; i++)
    {
        Dl_info info;
        void *addr = stack[i];

        // Resolve symbol and library info
        if (dladdr(addr, &info))
        {
            printf("Frame[%d]: Address: %p, Symbol: %s, Library: %s, Offset: %p\n",
                   i,
                   addr,
                   info.dli_sname ? info.dli_sname : "Unknown",
                   info.dli_fname ? info.dli_fname : "Unknown",
                   (void *)((uintptr_t)addr - (uintptr_t)info.dli_fbase));
        }
        else
        {
            printf("Frame[%d]: Address: %p, Unknown symbol\n", i, addr);
        }
    }
}

void signal_handler(int sig, siginfo_t *si, void *arg)
{
    ucontext_t *uc = (ucontext_t *)arg;
    printf("Caught signal %d\n", sig);
    print_stack(uc);
    exit(1);
}

static void install_sig_handler(void)
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);

    int signals[] = {
        SIGBUS,
        SIGFPE,
        SIGHUP,
        SIGILL,
        SIGINT,
        SIGIOT,
        SIGPIPE,
        SIGQUIT,
        SIGSEGV,
        SIGSYS,
        SIGTERM,
        SIGTRAP,
        SIGUSR1,
        SIGUSR2};
    size_t num_signals = sizeof(signals) / sizeof(signals[0]);

    for (size_t i = 0; i < num_signals; i++)
    {
        if (sigaction(signals[i], &sa, NULL) == -1)
        {
            printf("Failed to register handler for signal %d\n", signals[i]);
            exit(EXIT_FAILURE);
        }
    }
}


static void my_lv_log_print_cb(const char *buf)
{
    dbg_print("[lvgl-lvgl]%s", buf);
}

void static ui_init(void)
{
   static lv_disp_drv_t disp_drv;
   static lv_disp_draw_buf_t disp_buf;
    uint32_t rotated = LV_DISP_ROT_NONE;

    install_sig_handler();

    lv_disp_drv_init(&disp_drv);

    rotated = LV_DISP_ROT_270;
#ifndef USE_SUNXIFB_G2D_ROTATE
    disp_drv.sw_rotate = 1;
#endif

#if ENABLE_FASTBOOT
    disable_bootlogo();
#endif

    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    sunxifb_init(rotated);

    /*A buffer for LittlevGL to draw the screen's content*/
    static uint32_t width, height;
    sunxifb_get_sizes(&width, &height);

    static lv_color_t *buf;
#ifdef USE_SUNXIFB_DIRECT_MODE
    buf = (lv_color_t *)sunxifb_get_buf();
#else
    buf = (lv_color_t *)sunxifb_alloc(width * height * sizeof(lv_color_t),
                                      "lv_projector");
#endif

    if (buf == NULL)
    {
        sunxifb_exit();
        printf("malloc draw buffer fail\n");
        return 0;
    }

    /*Initialize a descriptor for the buffer*/
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, width * height);

    /*Initialize and register a display driver*/
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = sunxifb_flush;
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    disp_drv.rotated = rotated;
    disp_drv.screen_transp = 0;
    disp_drv.full_refresh = 0;
#ifdef USE_SUNXIFB_DIRECT_MODE
    disp_drv.direct_mode = 1;
    disp_drv.full_refresh = 1;
#endif
    lv_disp_drv_register(&disp_drv);
    keypad_int();


}

    #include "ktv_token_management.h"
#include "page_nav_stack.h"
int main(int argc, char *argv[])
{

    ui_init();
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    // lv_split_jpeg_init();
    // show_1111_jpg_demo();

    lv_page_list_btn_style_create();

    /*消息队列-woker线程*/
    db_list_pro_thread_init(32, 32);
    db_list_pro_worker_register();

    ktv_get_token();

    extern int auto_wifi_start(void);
    auto_wifi_start();


    /*视频相关*/
    ktv_player_ui_init();
    /*字幕*/
    karaoke_demo_open(lv_layer_top());


    mqtt_run();


    ktv_time_thread_start();

    lv_keyboard_set_layout(guider_ui.g_kb_top_layer, LV_KEYBOARD_LAYOUT_AZ);
    page_nav_register_home("home", &guider_ui.screen_7, &guider_ui.screen_7_del, setup_scr_screen_7);

    
    // system_init_early();
    // system_init_late();
    // lv_init_sys_param_late();

    while (1)
    {
        lv_task_handler();
        usleep(8000); 
    }
    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void) {
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = ((uint64_t) tv_start.tv_sec * 1000000
                + (uint64_t) tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = ((uint64_t) tv_now.tv_sec * 1000000 + (uint64_t) tv_now.tv_usec)
            / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

