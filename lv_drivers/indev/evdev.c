/**
 * @file evdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "evdev.h"
#if USE_EVDEV != 0 || USE_BSD_EVDEV

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#if USE_BSD_EVDEV
#include <dev/evdev/input.h>
#else
#include <linux/input.h>
#endif

#if USE_XKB
#include "xkb.h"
#endif /* USE_XKB */

/*********************
 *      DEFINES
 *********************/
#define INPUT_DEV_MAX 4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max);

/**********************
 *  STATIC VARIABLES
 **********************/
int evdev_fd = -1;
int evdev_root_x;
int evdev_root_y;
int evdev_button;

int evdev_key_val;
static bool evdev_key_longpress = 0;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the evdev interface
 */
void evdev_init(void)
{
    if (!evdev_set_file(EVDEV_NAME)) {
        return;
    }

#if USE_XKB
    xkb_init();
#endif
}
/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
bool evdev_set_file(char* dev_name)
{ 
     if(evdev_fd != -1) {
        close(evdev_fd);
     }
#if USE_BSD_EVDEV
     evdev_fd = open(dev_name, O_RDWR | O_NOCTTY);
#else
    // 优先使用传入的设备路径，如果不存在则尝试 touchscreen，最后使用配置的默认值
    if (dev_name != NULL && access(dev_name, F_OK) == 0) {
        evdev_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
    } else if (access("/dev/input/touchscreen", F_OK) == 0) {
        evdev_fd = open("/dev/input/touchscreen", O_RDWR | O_NOCTTY | O_NDELAY);
    } else {
        evdev_fd = open(EVDEV_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
    }
#endif

     if(evdev_fd == -1) {
        perror("unable to open evdev interface:");
        return false;
     }

#if USE_BSD_EVDEV
     fcntl(evdev_fd, F_SETFL, O_NONBLOCK);
#else
     fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);
#endif

     evdev_root_x = 0;
     evdev_root_y = 0;
     evdev_key_val = 0;
     evdev_button = LV_INDEV_STATE_REL;

     return true;
}

static uint8_t s_evdevInit = 0;
static uint8_t input_cnt = 0;
static struct pollfd *pfd = NULL;
uint32_t (*global_key)(uint32_t key) = NULL;

void register_global_key(void *fun)
{
    global_key = fun;
}

#define SUNXI_IR    "sunxi_ir"
#define SUNXI_GPADC "sunxi-gpadc"
int check_sunxi_dev(int id)
{
    int fd = 0;
    int len;
    char value_buf[64] = {0};
    char input_name[32] = {0};

    sprintf(input_name, "/sys/class/input/input%d/name", id);
    fd = open(input_name, O_RDONLY);
    if (fd < 0)
        return -1;

    len = read(fd, value_buf, sizeof(value_buf));
    close(fd);

    if (len == -1)
        return -1;

    //printf("check_dev: value_buf %s\n", value_buf);

    if (strncmp(value_buf, SUNXI_IR, strlen(SUNXI_IR)) == 0) {
        //printf("check_dev: find sunxi_ir\n");
        return 0;
    } else if (strncmp(value_buf, SUNXI_GPADC, strlen(SUNXI_GPADC)) == 0) {
        //printf("check_dev: find sunxi-gpadc\n");
        return 0;
    }

    return -1;
}

int open_evdev_fd(void)
{
    uint8_t input_name[32];
    int key_fds[INPUT_DEV_MAX] = {0};
    int i;

    for (i = 0; i < INPUT_DEV_MAX; i++) {
        if (check_sunxi_dev(i))
            break;

        sprintf(input_name, "/dev/input/event%d",i);
        key_fds[i] = open(input_name,O_RDONLY);
        if(key_fds[i] < 0)
            break;
        input_cnt = input_cnt + 1;
    }
    if (0 == input_cnt){
        printf("%s(), line:%d. No Input device!!!\n", __func__, __LINE__);
        return -1;
    }

    //printf("%s(), input device num: %d.\n", __func__, input_cnt);
    pfd = (struct pollfd*)malloc(sizeof(struct pollfd)*input_cnt);
    memset(pfd, 0, sizeof(struct pollfd)*input_cnt);

    for (i = 0; i < input_cnt; i++) {
        pfd[i].fd = key_fds[i];
        pfd[i].events = POLLIN | POLLRDNORM;
    }
    return 0;
}

void close_evdev_fd(void)
{
    int i;
    for (i = 0; i < input_cnt; i++) {
        if (pfd[i].fd > 0)
            close(pfd[i].fd);
    }

    if (pfd)
        free(pfd);
}

void evdev_read_ex(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    struct input_event in;
    int i;

    if (!s_evdevInit) {
        if (open_evdev_fd() == 0)
            s_evdevInit = 1;
    }

    while (poll(pfd, input_cnt, 0) > 0) {
        for(i = 0; i < input_cnt; i++) {
            if(pfd[i].revents == 0)
                continue;

            if (read(pfd[i].fd, &in, sizeof(struct input_event)) <= 0)
                continue;

            if(in.type == EV_KEY) {
                if(in.code == BTN_MOUSE || in.code == BTN_TOUCH) {
                    if(in.value == 0)
                        evdev_button = LV_INDEV_STATE_REL;
                    else if(in.value == 1)
                        evdev_button = LV_INDEV_STATE_PR;
                } else if(drv->type == LV_INDEV_TYPE_KEYPAD) {
#if USE_XKB
                    data->key = xkb_process_key(in.code, in.value != 0);
#else
                    /* deal home/back ...., only send press and repeat */
                    if (in.value == LV_INDEV_STATE_PRESSED || in.value == 2) {   //in.value 0：up 1：press 2：long press
                        if (global_key) {
                            if (in.value == 2)
                                evdev_key_longpress = 1;
                            in.code = global_key(in.code);
                        }
                    } else if (in.value == 0) {
                        evdev_key_longpress = 0;
                        if (global_key && in.code == KEY_POWER) // up only report power key
                            in.code = global_key(in.code);
                    }
                    switch(in.code) {
                        case KEY_BACKSPACE:
                            data->key = LV_KEY_BACKSPACE;
                            break;
                        case KEY_OK:
                            data->key = LV_KEY_ENTER;
                            break;
                        case KEY_PREVIOUS:
                            data->key = LV_KEY_PREV;
                            break;
                        case KEY_NEXT:
                            data->key = LV_KEY_NEXT;
                            break;
                        case KEY_UP:
                            data->key = LV_KEY_UP;
                            break;
                        case KEY_LEFT:
                            data->key = LV_KEY_LEFT;
                            break;
                        case KEY_RIGHT:
                            data->key = LV_KEY_RIGHT;
                            break;
                        case KEY_DOWN:
                            data->key = LV_KEY_DOWN;
                            break;
                        case KEY_BACK:
                            data->key = LV_KEY_BACK;
                            break;
                        case KEY_TAB:
                            data->key = LV_KEY_NEXT;
                            break;
                        default:
                            data->key = 0;
                            break;
                    }
#endif /* USE_XKB */
                    if (data->key != 0) {
                        /* Only record button state when actual output is produced to prevent widgets from refreshing */
                        data->state = (in.value) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
                    }
                    evdev_key_val = data->key;
                    evdev_button = data->state;
                    data->continue_reading = false;
                    if(evdev_key_val == LV_KEY_ENTER && evdev_button == 1) {
                        data->continue_reading = true;
                    }
                    return;
                }
            }
        }
    }

    if (!s_evdevInit) {
        close_evdev_fd();
    }
}


/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 */
void evdev_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    struct input_event in;

    while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
        if(in.type == EV_REL) {
            if(in.code == REL_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y += in.value;
				#else
					evdev_root_x += in.value;
				#endif
            else if(in.code == REL_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x += in.value;
				#else
					evdev_root_y += in.value;
				#endif
        } else if(in.type == EV_ABS) {
            if(in.code == ABS_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y = in.value;
				#else
					evdev_root_x = in.value;
				#endif
            else if(in.code == ABS_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x = in.value;
				#else
					evdev_root_y = in.value;
				#endif
            else if(in.code == ABS_MT_POSITION_X)
                                #if EVDEV_SWAP_AXES
                                        evdev_root_y = in.value;
                                #else
                                        evdev_root_x = in.value;
                                #endif
            else if(in.code == ABS_MT_POSITION_Y)
                                #if EVDEV_SWAP_AXES
                                        evdev_root_x = in.value;
                                #else
                                        evdev_root_y = in.value;
                                #endif
            else if(in.code == ABS_MT_TRACKING_ID) {
                                if(in.value == -1)
                                    evdev_button = LV_INDEV_STATE_REL;
                                else if(in.value == 0)
                                    evdev_button = LV_INDEV_STATE_PR;
            } else if(in.code == ABS_PRESSURE) {
                                if(in.value == 0)
                                    evdev_button = LV_INDEV_STATE_REL;
                                else if(in.value > 0)
                                    evdev_button = LV_INDEV_STATE_PR;
            }
        } else if(in.type == EV_KEY) {
            if(in.code == BTN_MOUSE || in.code == BTN_TOUCH) {
                if(in.value == 0)
                    evdev_button = LV_INDEV_STATE_REL;
                else if(in.value == 1)
                    evdev_button = LV_INDEV_STATE_PR;
            } else if(drv->type == LV_INDEV_TYPE_KEYPAD) {
#if USE_XKB
                data->key = xkb_process_key(in.code, in.value != 0);
#else
                switch(in.code) {
                    case KEY_BACKSPACE:
                        data->key = LV_KEY_BACKSPACE;
                        break;
                    case KEY_OK:
                        data->key = LV_KEY_ENTER;
                        break;
                    case KEY_PREVIOUS:
                        data->key = LV_KEY_PREV;
                        break;
                    case KEY_NEXT:
                        data->key = LV_KEY_NEXT;
                        break;
                    case KEY_UP:
                        data->key = LV_KEY_UP;
                        break;
                    case KEY_LEFT:
                        data->key = LV_KEY_PREV;
                        break;
                    case KEY_RIGHT:
                        data->key = LV_KEY_NEXT;
                        break;
                    case KEY_DOWN:
                        data->key = LV_KEY_DOWN;
                        break;
                    case KEY_TAB:
                        data->key = LV_KEY_NEXT;
                        break;
                    default:
                        data->key = 0;
                        break;
                }
#endif /* USE_XKB */
                if (data->key != 0) {
                    /* Only record button state when actual output is produced to prevent widgets from refreshing */
                    data->state = (in.value) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
                }
                evdev_key_val = data->key;
                evdev_button = data->state;
                return;
            }
        }
    }

    if(drv->type == LV_INDEV_TYPE_KEYPAD) {
        /* No data retrieved */
        data->key = evdev_key_val;
        data->state = evdev_button;
        return;
    }
    if(drv->type != LV_INDEV_TYPE_POINTER)
        return ;
    /*Store the collected data*/

#if EVDEV_CALIBRATE
    data->point.x = map(evdev_root_x, EVDEV_HOR_MIN, EVDEV_HOR_MAX, 0, drv->disp->driver->hor_res);
    data->point.y = map(evdev_root_y, EVDEV_VER_MIN, EVDEV_VER_MAX, 0, drv->disp->driver->ver_res);
#else
    data->point.x = evdev_root_x;
    data->point.y = evdev_root_y;
#endif

    data->state = evdev_button;

    if(data->point.x < 0)
      data->point.x = 0;
    if(data->point.y < 0)
      data->point.y = 0;
    if(data->point.x >= drv->disp->driver->hor_res)
      data->point.x = drv->disp->driver->hor_res - 1;
    if(data->point.y >= drv->disp->driver->ver_res)
      data->point.y = drv->disp->driver->ver_res - 1;

    return ;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**********************
 *   long press flag
 **********************/
int get_longpress_flag(void)
{
    return evdev_key_longpress;
}
#endif
