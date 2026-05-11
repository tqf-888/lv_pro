/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_2(lv_ui *ui)
{
    //Write codes screen_2
    ui->screen_2 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_2, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_2, LV_OBJ_FLAG_CLICKABLE);

    //Write style for screen_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_cont_1
    ui->screen_2_cont_1 = lv_obj_create(ui->screen_2);
    lv_obj_set_pos(ui->screen_2_cont_1, 2, 686);
    lv_obj_set_size(ui->screen_2_cont_1, 1388, 107);
    lv_obj_set_scrollbar_mode(ui->screen_2_cont_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_2_cont_1, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_2_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_12
    ui->screen_2_img_12 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_12, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_12, &_123124_1280x103);
    lv_img_set_pivot(ui->screen_2_img_12, 50,50);
    lv_img_set_angle(ui->screen_2_img_12, 0);
    lv_obj_set_pos(ui->screen_2_img_12, 0, 9);
    lv_obj_set_size(ui->screen_2_img_12, 1280, 103);

    //Write style for screen_2_img_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_12, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_10
    ui->screen_2_img_10 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_10, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_10, &_DIV1_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_10, 50,50);
    lv_img_set_angle(ui->screen_2_img_10, 0);
    lv_obj_set_pos(ui->screen_2_img_10, 87, 9);
    lv_obj_set_size(ui->screen_2_img_10, 80, 112);

    //Write style for screen_2_img_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_10, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_9
    ui->screen_2_img_9 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_9, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_9, &_DIV5_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_9, 50,50);
    lv_img_set_angle(ui->screen_2_img_9, 0);
    lv_obj_set_pos(ui->screen_2_img_9, 659, 9);
    lv_obj_set_size(ui->screen_2_img_9, 80, 112);

    //Write style for screen_2_img_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_9, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_8
    ui->screen_2_img_8 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_8, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_8, &_DIV6_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_8, 50,50);
    lv_img_set_angle(ui->screen_2_img_8, 0);
    lv_obj_set_pos(ui->screen_2_img_8, 802, 9);
    lv_obj_set_size(ui->screen_2_img_8, 80, 112);

    //Write style for screen_2_img_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_8, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_7
    ui->screen_2_img_7 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_7, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_7, &_DIV4_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_7, 50,50);
    lv_img_set_angle(ui->screen_2_img_7, 0);
    lv_obj_set_pos(ui->screen_2_img_7, 230, 9);
    lv_obj_set_size(ui->screen_2_img_7, 80, 112);

    //Write style for screen_2_img_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_6
    ui->screen_2_img_6 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_6, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_6, &_DIV3_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_6, 50,50);
    lv_img_set_angle(ui->screen_2_img_6, 0);
    lv_obj_set_pos(ui->screen_2_img_6, 373, 9);
    lv_obj_set_size(ui->screen_2_img_6, 80, 112);

    //Write style for screen_2_img_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_5
    ui->screen_2_img_5 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_5, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_5, &_DIV7_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_5, 50,50);
    lv_img_set_angle(ui->screen_2_img_5, 0);
    lv_obj_set_pos(ui->screen_2_img_5, 945, 9);
    lv_obj_set_size(ui->screen_2_img_5, 80, 112);

    //Write style for screen_2_img_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_4
    ui->screen_2_img_4 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_4, &_DIV2_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_4, 50,50);
    lv_img_set_angle(ui->screen_2_img_4, 0);
    lv_obj_set_pos(ui->screen_2_img_4, 516, 9);
    lv_obj_set_size(ui->screen_2_img_4, 80, 112);

    //Write style for screen_2_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_img_3
    ui->screen_2_img_3 = lv_img_create(ui->screen_2_cont_1);
    lv_obj_add_flag(ui->screen_2_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_2_img_3, &_DIV8_alpha_80x112);
    lv_img_set_pivot(ui->screen_2_img_3, 50,50);
    lv_img_set_angle(ui->screen_2_img_3, 0);
    lv_obj_set_pos(ui->screen_2_img_3, 1088, 9);
    lv_obj_set_size(ui->screen_2_img_3, 80, 112);

    //Write style for screen_2_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_2_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_2_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_2_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_btn_13
    ui->screen_2_btn_13 = lv_btn_create(ui->screen_2_cont_1);
    ui->screen_2_btn_13_label = lv_label_create(ui->screen_2_btn_13);
    lv_label_set_text(ui->screen_2_btn_13_label, "");
    lv_label_set_long_mode(ui->screen_2_btn_13_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_2_btn_13_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_2_btn_13, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_2_btn_13_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_2_btn_13, 495, 10);
    lv_obj_set_size(ui->screen_2_btn_13, 122, 98);

    //Write style for screen_2_btn_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_2_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_btn_13, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_2_btn_13, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_2_btn_13, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_2_btn_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_2_btn_13, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_btn_12
    ui->screen_2_btn_12 = lv_btn_create(ui->screen_2_cont_1);
    ui->screen_2_btn_12_label = lv_label_create(ui->screen_2_btn_12);
    lv_label_set_text(ui->screen_2_btn_12_label, "");
    lv_label_set_long_mode(ui->screen_2_btn_12_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_2_btn_12_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_2_btn_12, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_2_btn_12_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_2_btn_12, 638, 11);
    lv_obj_set_size(ui->screen_2_btn_12, 126, 98);

    //Write style for screen_2_btn_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_2_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_btn_12, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_2_btn_12, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_2_btn_12, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_2_btn_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_2_btn_12, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_btn_10
    ui->screen_2_btn_10 = lv_btn_create(ui->screen_2_cont_1);
    ui->screen_2_btn_10_label = lv_label_create(ui->screen_2_btn_10);
    lv_label_set_text(ui->screen_2_btn_10_label, "");
    lv_label_set_long_mode(ui->screen_2_btn_10_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_2_btn_10_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_2_btn_10, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_2_btn_10_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_2_btn_10, 349, 10);
    lv_obj_set_size(ui->screen_2_btn_10, 122, 98);

    //Write style for screen_2_btn_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2_btn_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_2_btn_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_btn_10, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_btn_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_2_btn_10, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_2_btn_10, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_2_btn_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_2_btn_10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_btn_9
    ui->screen_2_btn_9 = lv_btn_create(ui->screen_2_cont_1);
    ui->screen_2_btn_9_label = lv_label_create(ui->screen_2_btn_9);
    lv_label_set_text(ui->screen_2_btn_9_label, "");
    lv_label_set_long_mode(ui->screen_2_btn_9_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_2_btn_9_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_2_btn_9, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_2_btn_9_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_2_btn_9, 1068, 9);
    lv_obj_set_size(ui->screen_2_btn_9, 126, 98);

    //Write style for screen_2_btn_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2_btn_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_2_btn_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_btn_9, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_btn_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_2_btn_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_2_btn_9, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_2_btn_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_2_btn_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_btn_8
    ui->screen_2_btn_8 = lv_btn_create(ui->screen_2_cont_1);
    ui->screen_2_btn_8_label = lv_label_create(ui->screen_2_btn_8);
    lv_label_set_text(ui->screen_2_btn_8_label, "");
    lv_label_set_long_mode(ui->screen_2_btn_8_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_2_btn_8_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_2_btn_8, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_2_btn_8_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_2_btn_8, 930, 11);
    lv_obj_set_size(ui->screen_2_btn_8, 121, 98);

    //Write style for screen_2_btn_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2_btn_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_2_btn_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_btn_8, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_btn_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_2_btn_8, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_2_btn_8, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_2_btn_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_2_btn_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_btn_14
    ui->screen_2_btn_14 = lv_btn_create(ui->screen_2_cont_1);
    ui->screen_2_btn_14_label = lv_label_create(ui->screen_2_btn_14);
    lv_label_set_text(ui->screen_2_btn_14_label, "");
    lv_label_set_long_mode(ui->screen_2_btn_14_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_2_btn_14_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_2_btn_14, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_2_btn_14_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_2_btn_14, 67, 17);
    lv_obj_set_size(ui->screen_2_btn_14, 122, 98);

    //Write style for screen_2_btn_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_2_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_btn_14, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_2_btn_14, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_2_btn_14, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_2_btn_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_2_btn_14, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_2_btn_15
    ui->screen_2_btn_15 = lv_btn_create(ui->screen_2);
    ui->screen_2_btn_15_label = lv_label_create(ui->screen_2_btn_15);
    lv_label_set_text(ui->screen_2_btn_15_label, "");
    lv_label_set_long_mode(ui->screen_2_btn_15_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_2_btn_15_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_2_btn_15, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_2_btn_15_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_2_btn_15, 2, 0);
    lv_obj_set_size(ui->screen_2_btn_15, 1275, 689);

    //Write style for screen_2_btn_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_2_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_2_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_2_btn_15, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_2_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_2_btn_15, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_2_btn_15, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_2_btn_15, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_2_btn_15, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_2.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_2);

    //Init events for screen.
    events_init_screen_2(ui);
}
