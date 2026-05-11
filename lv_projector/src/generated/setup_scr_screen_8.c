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



void setup_scr_screen_8(lv_ui *ui)
{
    //Write codes screen_8
    ui->screen_8 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_8, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_8, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_13
    ui->screen_8_cont_13 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_13, 0, 0);
    lv_obj_set_size(ui->screen_8_cont_13, 352, 701);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_13, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_cont_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_cont_13, lv_color_hex(0x111520), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_cont_13, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_14
    ui->screen_8_cont_14 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_14, 932, 0);
    lv_obj_set_size(ui->screen_8_cont_14, 352, 701);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_14, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_cont_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_cont_14, lv_color_hex(0x111520), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_cont_14, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_16
    ui->screen_8_cont_16 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_16, 346, 471);
    lv_obj_set_size(ui->screen_8_cont_16, 588, 228);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_16, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_cont_16, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_16, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_cont_16, lv_color_hex(0x111520), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_cont_16, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_15
    ui->screen_8_cont_15 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_15, 183, 0);
    lv_obj_set_size(ui->screen_8_cont_15, 909, 150);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_15, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_cont_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_15, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_cont_15, lv_color_hex(0x111520), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_cont_15, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_6
    ui->screen_8_cont_6 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_6, 30, 135);
    lv_obj_set_size(ui->screen_8_cont_6, 1301, 528);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_6, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_cont_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_6, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_8_cont_6, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_8_cont_6, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_100
    ui->screen_8_img_100 = lv_img_create(ui->screen_8_cont_6);
    lv_obj_add_flag(ui->screen_8_img_100, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_8_img_100, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\foiwdshboa1.png");
#else
    lv_img_set_src(ui->screen_8_img_100, "S:/usr/share/lv_projector/foiwdshboa1.png");
#endif
    lv_img_set_pivot(ui->screen_8_img_100, 50,50);
    lv_img_set_angle(ui->screen_8_img_100, 0);
    lv_obj_set_pos(ui->screen_8_img_100, 18, 7);
    lv_obj_set_size(ui->screen_8_img_100, 1183, 554);

    //Write style for screen_8_img_100, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_100, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_100, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_100, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_100, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_18
    ui->screen_8_btn_18 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_18_label = lv_label_create(ui->screen_8_btn_18);
    lv_label_set_text(ui->screen_8_btn_18_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_18_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_18_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_18, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_18_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_18, 922, 191);
    lv_obj_set_size(ui->screen_8_btn_18, 273, 147);

    //Write style for screen_8_btn_18, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_18, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_18, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_18, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_18, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_18, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_17
    ui->screen_8_btn_17 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_17_label = lv_label_create(ui->screen_8_btn_17);
    lv_label_set_text(ui->screen_8_btn_17_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_17_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_17_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_17, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_17_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_17, 917, 376);
    lv_obj_set_size(ui->screen_8_btn_17, 273, 147);

    //Write style for screen_8_btn_17, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_17, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_17, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_17, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_17, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_17, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_16
    ui->screen_8_btn_16 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_16_label = lv_label_create(ui->screen_8_btn_16);
    lv_label_set_text(ui->screen_8_btn_16_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_16_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_16_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_16, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_16_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_16, 621, 375);
    lv_obj_set_size(ui->screen_8_btn_16, 273, 147);

    //Write style for screen_8_btn_16, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_16, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_16, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_16, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_16, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_16, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_15
    ui->screen_8_btn_15 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_15_label = lv_label_create(ui->screen_8_btn_15);
    lv_label_set_text(ui->screen_8_btn_15_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_15_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_15_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_15, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_15_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_15, 323, 377);
    lv_obj_set_size(ui->screen_8_btn_15, 273, 147);

    //Write style for screen_8_btn_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_15, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_15, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_15, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_15, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_15, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_14
    ui->screen_8_btn_14 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_14_label = lv_label_create(ui->screen_8_btn_14);
    lv_label_set_text(ui->screen_8_btn_14_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_14_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_14_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_14, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_14_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_14, 28, 368);
    lv_obj_set_size(ui->screen_8_btn_14, 273, 147);

    //Write style for screen_8_btn_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_14, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_14, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_14, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_14, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_13
    ui->screen_8_btn_13 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_13_label = lv_label_create(ui->screen_8_btn_13);
    lv_label_set_text(ui->screen_8_btn_13_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_13_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_13_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_13, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_13_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_13, 19, 193);
    lv_obj_set_size(ui->screen_8_btn_13, 273, 147);

    //Write style for screen_8_btn_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_13, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_13, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_13, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_13, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_12
    ui->screen_8_btn_12 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_12_label = lv_label_create(ui->screen_8_btn_12);
    lv_label_set_text(ui->screen_8_btn_12_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_12_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_12_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_12, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_12_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_12, 19, 10);
    lv_obj_set_size(ui->screen_8_btn_12, 273, 147);

    //Write style for screen_8_btn_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_12, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_12, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_12, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_12, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_30
    ui->screen_8_btn_30 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_30_label = lv_label_create(ui->screen_8_btn_30);
    lv_label_set_text(ui->screen_8_btn_30_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_30_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_30_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_30, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_30_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_30, 309, 16);
    lv_obj_set_size(ui->screen_8_btn_30, 599, 316);

    //Write style for screen_8_btn_30, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_30, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_30, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_30, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_30, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_30, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_19
    ui->screen_8_btn_19 = lv_btn_create(ui->screen_8_cont_6);
    ui->screen_8_btn_19_label = lv_label_create(ui->screen_8_btn_19);
    lv_label_set_text(ui->screen_8_btn_19_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_19_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_19_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_19, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_19_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_19, 916, 11);
    lv_obj_set_size(ui->screen_8_btn_19, 273, 147);

    //Write style for screen_8_btn_19, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_19, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_19, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_19, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_19, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_19, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_10
    ui->screen_8_cont_10 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_10, 1, 687);
    lv_obj_set_size(ui->screen_8_cont_10, 1388, 107);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_10, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_cont_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_85
    ui->screen_8_img_85 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_85, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_85, &_123124_1280x103);
    lv_img_set_pivot(ui->screen_8_img_85, 50,50);
    lv_img_set_angle(ui->screen_8_img_85, 0);
    lv_obj_set_pos(ui->screen_8_img_85, 0, 9);
    lv_obj_set_size(ui->screen_8_img_85, 1280, 103);

    //Write style for screen_8_img_85, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_85, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_85, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_85, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_85, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_86
    ui->screen_8_img_86 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_86, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_86, &_DIV_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_86, 50,50);
    lv_img_set_angle(ui->screen_8_img_86, 0);
    lv_obj_set_pos(ui->screen_8_img_86, 71, 7);
    lv_obj_set_size(ui->screen_8_img_86, 80, 112);

    //Write style for screen_8_img_86, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_86, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_86, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_86, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_86, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_87
    ui->screen_8_img_87 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_87, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_87, &_DIV1_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_87, 50,50);
    lv_img_set_angle(ui->screen_8_img_87, 0);
    lv_obj_set_pos(ui->screen_8_img_87, 202, 7);
    lv_obj_set_size(ui->screen_8_img_87, 80, 112);

    //Write style for screen_8_img_87, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_87, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_87, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_87, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_87, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_88
    ui->screen_8_img_88 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_88, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_88, &_DIV5_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_88, 50,50);
    lv_img_set_angle(ui->screen_8_img_88, 0);
    lv_obj_set_pos(ui->screen_8_img_88, 726, 7);
    lv_obj_set_size(ui->screen_8_img_88, 80, 112);

    //Write style for screen_8_img_88, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_88, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_88, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_88, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_88, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_91
    ui->screen_8_img_91 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_91, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_91, &_DIV6_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_91, 50,50);
    lv_img_set_angle(ui->screen_8_img_91, 0);
    lv_obj_set_pos(ui->screen_8_img_91, 857, 7);
    lv_obj_set_size(ui->screen_8_img_91, 80, 112);

    //Write style for screen_8_img_91, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_91, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_91, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_91, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_91, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_90
    ui->screen_8_img_90 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_90, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_90, &_DIV4_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_90, 50,50);
    lv_img_set_angle(ui->screen_8_img_90, 0);
    lv_obj_set_pos(ui->screen_8_img_90, 333, 7);
    lv_obj_set_size(ui->screen_8_img_90, 80, 112);

    //Write style for screen_8_img_90, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_90, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_90, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_90, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_90, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_89
    ui->screen_8_img_89 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_89, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_89, &_DIV3_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_89, 50,50);
    lv_img_set_angle(ui->screen_8_img_89, 0);
    lv_obj_set_pos(ui->screen_8_img_89, 464, 7);
    lv_obj_set_size(ui->screen_8_img_89, 80, 112);

    //Write style for screen_8_img_89, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_89, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_89, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_89, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_89, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_94
    ui->screen_8_img_94 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_94, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_94, &_DIV7_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_94, 50,50);
    lv_img_set_angle(ui->screen_8_img_94, 0);
    lv_obj_set_pos(ui->screen_8_img_94, 988, 7);
    lv_obj_set_size(ui->screen_8_img_94, 80, 112);

    //Write style for screen_8_img_94, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_94, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_94, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_94, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_94, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_93
    ui->screen_8_img_93 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_93, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_93, &_DIV2_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_93, 50,50);
    lv_img_set_angle(ui->screen_8_img_93, 0);
    lv_obj_set_pos(ui->screen_8_img_93, 595, 7);
    lv_obj_set_size(ui->screen_8_img_93, 80, 112);

    //Write style for screen_8_img_93, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_93, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_93, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_93, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_93, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_92
    ui->screen_8_img_92 = lv_img_create(ui->screen_8_cont_10);
    lv_obj_add_flag(ui->screen_8_img_92, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_92, &_DIV8_alpha_80x112);
    lv_img_set_pivot(ui->screen_8_img_92, 50,50);
    lv_img_set_angle(ui->screen_8_img_92, 0);
    lv_obj_set_pos(ui->screen_8_img_92, 1119, 7);
    lv_obj_set_size(ui->screen_8_img_92, 80, 112);

    //Write style for screen_8_img_92, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_92, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_92, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_92, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_92, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_20
    ui->screen_8_btn_20 = lv_btn_create(ui->screen_8_cont_10);
    ui->screen_8_btn_20_label = lv_label_create(ui->screen_8_btn_20);
    lv_label_set_text(ui->screen_8_btn_20_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_20_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_20_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_20, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_20_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_20, 577, 10);
    lv_obj_set_size(ui->screen_8_btn_20, 122, 98);

    //Write style for screen_8_btn_20, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_20, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_20, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_20, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_20, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_20, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_21
    ui->screen_8_btn_21 = lv_btn_create(ui->screen_8_cont_10);
    ui->screen_8_btn_21_label = lv_label_create(ui->screen_8_btn_21);
    lv_label_set_text(ui->screen_8_btn_21_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_21_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_21_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_21, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_21_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_21, 706, 10);
    lv_obj_set_size(ui->screen_8_btn_21, 126, 98);

    //Write style for screen_8_btn_21, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_21, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_21, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_21, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_21, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_21, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_22
    ui->screen_8_btn_22 = lv_btn_create(ui->screen_8_cont_10);
    ui->screen_8_btn_22_label = lv_label_create(ui->screen_8_btn_22);
    lv_label_set_text(ui->screen_8_btn_22_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_22_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_22_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_22, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_22_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_22, 46, 12);
    lv_obj_set_size(ui->screen_8_btn_22, 124, 98);

    //Write style for screen_8_btn_22, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_22, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_22, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_22, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_22, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_22, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_25
    ui->screen_8_btn_25 = lv_btn_create(ui->screen_8_cont_10);
    ui->screen_8_btn_25_label = lv_label_create(ui->screen_8_btn_25);
    lv_label_set_text(ui->screen_8_btn_25_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_25_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_25_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_25, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_25_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_25, 442, 13);
    lv_obj_set_size(ui->screen_8_btn_25, 122, 98);

    //Write style for screen_8_btn_25, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_25, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_25, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_25, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_25, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_25, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_26
    ui->screen_8_btn_26 = lv_btn_create(ui->screen_8_cont_10);
    ui->screen_8_btn_26_label = lv_label_create(ui->screen_8_btn_26);
    lv_label_set_text(ui->screen_8_btn_26_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_26_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_26_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_26, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_26_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_26, 1096, 14);
    lv_obj_set_size(ui->screen_8_btn_26, 126, 98);

    //Write style for screen_8_btn_26, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_26, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_26, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_26, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_26, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_26, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_27
    ui->screen_8_btn_27 = lv_btn_create(ui->screen_8_cont_10);
    ui->screen_8_btn_27_label = lv_label_create(ui->screen_8_btn_27);
    lv_label_set_text(ui->screen_8_btn_27_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_27_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_27_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_27, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_27_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_27, 969, 14);
    lv_obj_set_size(ui->screen_8_btn_27, 121, 98);

    //Write style for screen_8_btn_27, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_27, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_27, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_27, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_27, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_27, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_29
    ui->screen_8_btn_29 = lv_btn_create(ui->screen_8_cont_10);
    ui->screen_8_btn_29_label = lv_label_create(ui->screen_8_btn_29);
    lv_label_set_text(ui->screen_8_btn_29_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_29_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_29_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_29, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_29_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_29, 179, 9);
    lv_obj_set_size(ui->screen_8_btn_29, 122, 98);

    //Write style for screen_8_btn_29, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_29, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_29, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_29, &lv_font_montserratMedium_34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_29, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_29, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_11
    ui->screen_8_cont_11 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_11, 995, 321);
    lv_obj_set_size(ui->screen_8_cont_11, 72, 366);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_11, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_8_cont_11, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_8_cont_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_11, 70, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_cont_11, lv_color_hex(0x1C1D1E), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_cont_11, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_slider_1
    ui->screen_8_slider_1 = lv_slider_create(ui->screen_8_cont_11);
    lv_slider_set_range(ui->screen_8_slider_1, 0, 100);
    lv_slider_set_mode(ui->screen_8_slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_8_slider_1, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_8_slider_1, 26, 36);
    lv_obj_set_size(ui->screen_8_slider_1, 17, 295);

    //Write style for screen_8_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_slider_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_slider_1, lv_color_hex(0x626262), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_slider_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_8_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_8_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_slider_1, lv_color_hex(0xfff700), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_slider_1, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_8_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_slider_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_slider_1, lv_color_hex(0xffe200), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_slider_1, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_8_cont_12
    ui->screen_8_cont_12 = lv_obj_create(ui->screen_8);
    lv_obj_set_pos(ui->screen_8_cont_12, 3, 15);
    lv_obj_set_size(ui->screen_8_cont_12, 1280, 103);
    lv_obj_set_scrollbar_mode(ui->screen_8_cont_12, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_cont_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_98
    ui->screen_8_img_98 = lv_img_create(ui->screen_8_cont_12);
    lv_obj_add_flag(ui->screen_8_img_98, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_98, &_sgderhbrdsdfgs_alpha_100x100);
    lv_img_set_pivot(ui->screen_8_img_98, 50,50);
    lv_img_set_angle(ui->screen_8_img_98, 0);
    lv_obj_set_pos(ui->screen_8_img_98, 44, 0);
    lv_obj_set_size(ui->screen_8_img_98, 100, 100);

    //Write style for screen_8_img_98, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_98, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_98, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_98, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_98, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_img_99
    ui->screen_8_img_99 = lv_img_create(ui->screen_8_cont_12);
    lv_obj_add_flag(ui->screen_8_img_99, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_img_99, &_rhbstrhrtfd_alpha_214x49);
    lv_img_set_pivot(ui->screen_8_img_99, 50,50);
    lv_img_set_angle(ui->screen_8_img_99, 0);
    lv_obj_set_pos(ui->screen_8_img_99, 1018, 28);
    lv_obj_set_size(ui->screen_8_img_99, 214, 49);

    //Write style for screen_8_img_99, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_img_99, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_img_99, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_img_99, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_img_99, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_label_2
    ui->screen_8_label_2 = lv_label_create(ui->screen_8_cont_12);
    lv_label_set_text(ui->screen_8_label_2, "12:00");
    lv_label_set_long_mode(ui->screen_8_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_8_label_2, 926, 44);
    lv_obj_set_size(ui->screen_8_label_2, 107, 50);

    //Write style for screen_8_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_label_2, lv_color_hex(0xa2a2a2), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_label_2, &lv_font_Regular_28, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_label_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_btn_31
    ui->screen_8_btn_31 = lv_btn_create(ui->screen_8_cont_12);
    ui->screen_8_btn_31_label = lv_label_create(ui->screen_8_btn_31);
    lv_label_set_text(ui->screen_8_btn_31_label, "");
    lv_label_set_long_mode(ui->screen_8_btn_31_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_btn_31_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_btn_31, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_btn_31_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_btn_31, 1149, -11);
    lv_obj_set_size(ui->screen_8_btn_31, 123, 110);

    //Write style for screen_8_btn_31, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_btn_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_btn_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_btn_31, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_btn_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_btn_31, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_btn_31, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_btn_31, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_btn_31, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_8.





    //Update current screen layout.
    lv_obj_update_layout(ui->screen_8);

    //Init events for screen.
    events_init_screen_8(ui);
}
