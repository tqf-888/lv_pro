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



void setup_scr_screen_7(lv_ui *ui)
{
    //Write codes screen_7
    ui->screen_7 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_7, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_7, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_7, lv_color_hex(0x111520), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_7, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_img_4
    ui->screen_7_img_4 = lv_img_create(ui->screen_7);
    lv_obj_add_flag(ui->screen_7_img_4, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_7_img_4, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\bdfxbhdf.png");
#else
    lv_img_set_src(ui->screen_7_img_4, "S:/usr/share/lv_projector/bdfxbhdf.png");
#endif
    lv_img_set_pivot(ui->screen_7_img_4, 0,0);
    lv_img_set_angle(ui->screen_7_img_4, 0);
    lv_obj_set_pos(ui->screen_7_img_4, 3, 105);
    lv_obj_set_size(ui->screen_7_img_4, 1248, 647);

    //Write style for screen_7_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_7_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_7_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_7_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_12
    ui->screen_7_btn_12 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_12_label = lv_label_create(ui->screen_7_btn_12);
    lv_label_set_text(ui->screen_7_btn_12_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_12_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_12_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_12, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_12_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_12, 30, 156);
    lv_obj_set_size(ui->screen_7_btn_12, 603, 388);

    //Write style for screen_7_btn_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_12, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_12, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_12, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_12, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_13
    ui->screen_7_btn_13 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_13_label = lv_label_create(ui->screen_7_btn_13);
    lv_label_set_text(ui->screen_7_btn_13_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_13_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_13_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_13, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_13_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_13, 659, 143);
    lv_obj_set_size(ui->screen_7_btn_13, 284, 174);

    //Write style for screen_7_btn_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_13, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_13, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_13, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_13, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_14
    ui->screen_7_btn_14 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_14_label = lv_label_create(ui->screen_7_btn_14);
    lv_label_set_text(ui->screen_7_btn_14_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_14_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_14_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_14, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_14_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_14, 966, 143);
    lv_obj_set_size(ui->screen_7_btn_14, 284, 174);

    //Write style for screen_7_btn_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_14, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_14, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_14, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_14, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_15
    ui->screen_7_btn_15 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_15_label = lv_label_create(ui->screen_7_btn_15);
    lv_label_set_text(ui->screen_7_btn_15_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_15_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_15_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_15, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_15_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_15, 966, 356);
    lv_obj_set_size(ui->screen_7_btn_15, 284, 174);

    //Write style for screen_7_btn_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_15, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_15, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_15, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_15, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_15, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_16
    ui->screen_7_btn_16 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_16_label = lv_label_create(ui->screen_7_btn_16);
    lv_label_set_text(ui->screen_7_btn_16_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_16_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_16_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_16, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_16_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_16, 966, 576);
    lv_obj_set_size(ui->screen_7_btn_16, 284, 168);

    //Write style for screen_7_btn_16, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_16, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_16, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_16, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_16, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_16, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_17
    ui->screen_7_btn_17 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_17_label = lv_label_create(ui->screen_7_btn_17);
    lv_label_set_text(ui->screen_7_btn_17_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_17_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_17_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_17, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_17_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_17, 659, 359);
    lv_obj_set_size(ui->screen_7_btn_17, 284, 171);

    //Write style for screen_7_btn_17, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_17, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_17, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_17, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_17, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_17, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_18
    ui->screen_7_btn_18 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_18_label = lv_label_create(ui->screen_7_btn_18);
    lv_label_set_text(ui->screen_7_btn_18_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_18_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_18_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_18, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_18_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_18, 659, 583);
    lv_obj_set_size(ui->screen_7_btn_18, 284, 166);

    //Write style for screen_7_btn_18, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_18, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_18, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_18, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_18, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_18, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_19
    ui->screen_7_btn_19 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_19_label = lv_label_create(ui->screen_7_btn_19);
    lv_label_set_text(ui->screen_7_btn_19_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_19_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_19_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_19, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_19_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_19, 347, 576);
    lv_obj_set_size(ui->screen_7_btn_19, 284, 174);

    //Write style for screen_7_btn_19, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_19, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_19, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_19, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_19, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_19, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_20
    ui->screen_7_btn_20 = lv_btn_create(ui->screen_7);
    ui->screen_7_btn_20_label = lv_label_create(ui->screen_7_btn_20);
    lv_label_set_text(ui->screen_7_btn_20_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_20_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_20_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_20, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_20_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_20, 39, 572);
    lv_obj_set_size(ui->screen_7_btn_20, 284, 174);

    //Write style for screen_7_btn_20, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_20, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_20, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_20, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_20, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_20, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_cont_1
    ui->screen_7_cont_1 = lv_obj_create(ui->screen_7);
    lv_obj_set_pos(ui->screen_7_cont_1, 3, 15);
    lv_obj_set_size(ui->screen_7_cont_1, 1280, 109);
    lv_obj_set_scrollbar_mode(ui->screen_7_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_7_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_img_2
    ui->screen_7_img_2 = lv_img_create(ui->screen_7_cont_1);
    lv_obj_add_flag(ui->screen_7_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_7_img_2, &_sgderhbrd_alpha_324x122);
    lv_img_set_pivot(ui->screen_7_img_2, 50,50);
    lv_img_set_angle(ui->screen_7_img_2, 0);
    lv_obj_set_pos(ui->screen_7_img_2, 36, -3);
    lv_obj_set_size(ui->screen_7_img_2, 324, 122);

    //Write style for screen_7_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_7_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_7_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_7_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_label_1
    ui->screen_7_label_1 = lv_label_create(ui->screen_7_cont_1);
    lv_label_set_text(ui->screen_7_label_1, "12:00");
    lv_label_set_long_mode(ui->screen_7_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_7_label_1, 926, 44);
    lv_obj_set_size(ui->screen_7_label_1, 107, 50);

    //Write style for screen_7_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_label_1, lv_color_hex(0xa2a2a2), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_label_1, &lv_font_Regular_28, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_label_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_img_3
    ui->screen_7_img_3 = lv_img_create(ui->screen_7_cont_1);
    lv_obj_add_flag(ui->screen_7_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_7_img_3, &_rhbstrhrtfd_alpha_214x49);
    lv_img_set_pivot(ui->screen_7_img_3, 50,50);
    lv_img_set_angle(ui->screen_7_img_3, 0);
    lv_obj_set_pos(ui->screen_7_img_3, 1018, 28);
    lv_obj_set_size(ui->screen_7_img_3, 214, 49);

    //Write style for screen_7_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_7_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_7_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_7_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_7_btn_11
    ui->screen_7_btn_11 = lv_btn_create(ui->screen_7_cont_1);
    ui->screen_7_btn_11_label = lv_label_create(ui->screen_7_btn_11);
    lv_label_set_text(ui->screen_7_btn_11_label, "");
    lv_label_set_long_mode(ui->screen_7_btn_11_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_7_btn_11_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_7_btn_11, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_7_btn_11_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_7_btn_11, 1130, -9);
    lv_obj_set_size(ui->screen_7_btn_11, 142, 111);

    //Write style for screen_7_btn_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_7_btn_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_7_btn_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_7_btn_11, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_7_btn_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_7_btn_11, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_7_btn_11, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_7_btn_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_7_btn_11, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_7.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_7);

    //Init events for screen.
    events_init_screen_7(ui);
}
