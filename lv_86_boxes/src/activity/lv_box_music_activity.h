/*
 * lv_box_music_activity.h
 *
 *  Created on: 2022��9��9��
 *      Author: anruliu
 */

#ifndef LV_86_BOXES_SRC_ACTIVITY_LV_BOX_MUSIC_ACTIVITY_H_
#define LV_86_BOXES_SRC_ACTIVITY_LV_BOX_MUSIC_ACTIVITY_H_

#include "lvgl/lvgl.h"

extern lv_obj_t *music_activity;
extern bool music_activity_is_open;

void lv_box_music_init(void);
void lv_box_music_timer_en(bool en);

#endif /* LV_86_BOXES_SRC_ACTIVITY_LV_BOX_MUSIC_ACTIVITY_H_ */
