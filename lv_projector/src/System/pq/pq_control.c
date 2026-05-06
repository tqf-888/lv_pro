#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "lv_common.h"
#include "sys_param.h"
#include "pq_control.h"
#include "../Common/setting/picture_setting.h"

pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

int soc_read_pq(char *path)
{
	char value_str[40] = {0};
	int fd = -1, ret = -1;
	unsigned int result = 0;

    if (path == NULL) {
        loge("soc_write_pq path is null\n");
        return -1;
    }

	pthread_mutex_lock(&gMutex);

	fd = open(path, O_RDWR|O_CREAT, 0644);
	if (fd < 0) {
		loge("open failed: %s:%s", path, strerror(errno));
		goto CLOSE;
	}
	memset(value_str, 0 ,40);
	ret = read(fd, value_str, 40);
	if (ret < 0) {
		loge("read fail:%s", strerror(errno));
		goto CLOSE;
	}
	result = atoi(value_str);
CLOSE:
	if (fd >= 0)
		close(fd);
	pthread_mutex_unlock(&gMutex);
	return result;
}

int soc_write_pq(char *path, int value)
{
	char value_str[40] = {0};
	int fd = -1, ret = -1;

    if (path == NULL) {
        loge("soc_write_pq path is null\n");
        return -1;
    }
	pthread_mutex_lock(&gMutex);
	fd = open(path, O_RDWR|O_CREAT, 0644);
	if (fd < 0) {
		loge("open failed: %s:%s", path, strerror(errno));
		goto CLOSE;
	}
    sprintf(value_str, "%d", value);
	ret = write(fd, value_str, strlen(value_str));
	if (ret != strlen(value_str)) {
		loge("Write %s fail:%s", value_str, strerror(errno));
		goto CLOSE;
	}
	ret = 0;
CLOSE:
	if (fd >= 0)
		close(fd);
	pthread_mutex_unlock(&gMutex);
	return ret;
}


void set_all_pq_for_current_picture_mode(void)
{
    int value;
    int color_temp_value = 255;

    value = lv_get_sys_param(P_CONTRAST);
    if (value > 0)
        factory_set_current_pq_para(PQ_CONTRAST_NAME, value, 0);

    value = lv_get_sys_param(P_BRIGHTNESS);
    if (value > 0)
        factory_set_current_pq_para(PQ_BRIGHTNESS_NAME, value, 0);

    value = lv_get_sys_param(P_SHARPNESS);
    if (value > 0)
        factory_set_current_pq_para(PQ_SHARPNESS_NAME, value, 0);

    value = lv_get_sys_param(P_COLOR);
    if (value > 0)
        factory_set_current_pq_para(PQ_COLOR_NAME, value, 0);

    value = lv_get_sys_param(P_COLOR_TEMP);
    if (value == PQ_COLOR_TEMP_STANDARD_ID) {
        color_temp_value = get_pq_para(PQ_COLOR_TEMP_NAME, PQ_COLOR_TEMP_STANDARD_NAME);
    } else if (value == PQ_COLOR_TEMP_COLD_ID) {
        color_temp_value = get_pq_para(PQ_COLOR_TEMP_NAME, PQ_COLOR_TEMP_COLD_NAME);
    } else if (value == PQ_COLOR_TEMP_WARM_ID) {
        color_temp_value = get_pq_para(PQ_COLOR_TEMP_NAME, PQ_COLOR_TEMP_WARM_NAME);
    }
    if (color_temp_value >= PQ_COLOR_TEMP_UI_MIN_VALUE && color_temp_value <= PQ_COLOR_TEMP_UI_MAX_VALUE)
        soc_write_pq(SOC_COLOR_TEMP_PATH, color_temp_value);
}

int lv_init_pq(void)
{
    /* enable pq mode */
    soc_write_pq(SOC_PQ_MODE_PATH, 2);

    //set_all_pq_for_current_picture_mode();
    return 0;
}

#include "system_api.h"

void lv_set_zoom_(void)
{
    float w_ratio = 0;
    float h_ratio = 0;

    uint8_t mode = lv_get_sys_param(P_ASPECT_RATIO);
    uint8_t zoom_ratio = lv_get_sys_param(P_SYS_ZOOM_DIS_MODE);

    if (zoom_ratio > PQ_ZOOM_UI_MAX_VALUE || zoom_ratio < PQ_ZOOM_UI_MIN_VALUE)
        zoom_ratio = PQ_ZOOM_UI_MAX_VALUE;

    if (mode == ASPECT_RATIO_16_9_ID) {
        w_ratio = zoom_ratio;
        h_ratio = zoom_ratio;
    } else if (mode == ASPECT_RATIO_4_3_ID) {
        w_ratio = (float)zoom_ratio * 3 / 4;
        h_ratio = zoom_ratio;
    }

    //printf("set zoom: w_ratio %.2f, h_ratio %.2f\n", w_ratio, h_ratio);
    set_keystone_zoom(w_ratio, h_ratio);
}

// 根据类型和数值设置 PQ 参数
int set_pq_param(sys_param_id type, int value)
{
	loge("\n\n\nset_pq_param: type %d value = %d\n\n", type, value);
    const char *path = NULL;
    int min_val = 0, max_val = 100; // 默认范围

    lv_set_sys_param(type, value);
    
    switch (type) {
        case P_PICTURE_MODE:
            path = SOC_PQ_MODE_PATH;
            min_val = 0;   // 根据实际定义调整
            max_val = 3;   // 假设0:标准,1:动态,2:柔和,3:用户
            break;
        case P_CONTRAST:
            path = SOC_CONTRAST_PATH;
            break;
        case P_BRIGHTNESS:
            path = SOC_BRIGHTNESS_PATH;
            break;
        case P_COLOR:
            path = SOC_COLOR_PATH;
            break;
        case P_SHARPNESS:
            path = SOC_SHARPNESS_PATH;
            break;
        case P_COLOR_TEMP:
            path = SOC_COLOR_TEMP_PATH;
            min_val = 0;   // 0:标准,1:冷,2:暖
            max_val = 2;
            break;

        case P_ASPECT_RATIO:
            lv_set_zoom_();
            break;
        case P_SYS_ZOOM_DIS_MODE:
            set_keystone_zoom(value, value);
            break;
        default:
            loge("set_pq_param: invalid type %d\n", type);
            return -1;
    }

    // 检查数值范围
    if (value < min_val || value > max_val) {
        loge("set_pq_param: value %d out of range [%d, %d]\n", value, min_val, max_val);
        return -1;
    }

    // 调用底层写入函数
    soc_write_pq((char *)path, value);
    update_picture_param();
    return 1;
}