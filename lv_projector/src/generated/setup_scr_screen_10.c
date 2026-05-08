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
#include "order_song_page_demo.h"


void setup_scr_screen_10(lv_ui *ui)
{
    //Write codes screen_10
    ui->screen_10 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_10, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_10, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_img_14
    ui->screen_10_img_14 = lv_img_create(ui->screen_10);
    lv_obj_add_flag(ui->screen_10_img_14, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_10_img_14, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\dsgwsegbsr.png");
#else
    lv_img_set_src(ui->screen_10_img_14, "S:/usr/share/lv_projector/dsgwsegbsr.png");
#endif
    lv_img_set_pivot(ui->screen_10_img_14, 50,50);
    lv_img_set_angle(ui->screen_10_img_14, 0);
    lv_obj_set_pos(ui->screen_10_img_14, 0, 0);
    lv_obj_set_size(ui->screen_10_img_14, 1280, 800);

    //Write style for screen_10_img_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_10_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_10_img_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_10_img_14, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_cont_3
    ui->screen_10_cont_3 = lv_obj_create(ui->screen_10);
    lv_obj_set_pos(ui->screen_10_cont_3, 0, 0);
    lv_obj_set_size(ui->screen_10_cont_3, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_10_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_10_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_10_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_img_2
    ui->screen_10_img_2 = lv_img_create(ui->screen_10_cont_3);
    lv_obj_add_flag(ui->screen_10_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_10_img_2, &_vdsbdf_alpha_223x48);
    lv_img_set_pivot(ui->screen_10_img_2, 50,50);
    lv_img_set_angle(ui->screen_10_img_2, 0);
    lv_obj_set_pos(ui->screen_10_img_2, 28, 25);
    lv_obj_set_size(ui->screen_10_img_2, 223, 48);

    //Write style for screen_10_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_10_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_10_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_10_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_cont_1
    ui->screen_10_cont_1 = lv_obj_create(ui->screen_10_cont_3);
    lv_obj_set_pos(ui->screen_10_cont_1, 240, 225);
    lv_obj_set_size(ui->screen_10_cont_1, 925, 454);
    lv_obj_set_scrollbar_mode(ui->screen_10_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_10_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_10_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_btn_1
    ui->screen_10_btn_1 = lv_btn_create(ui->screen_10_cont_3);
    ui->screen_10_btn_1_label = lv_label_create(ui->screen_10_btn_1);
    lv_label_set_text(ui->screen_10_btn_1_label, "");
    lv_label_set_long_mode(ui->screen_10_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_10_btn_1_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_10_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_10_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_10_btn_1, 6, 5);
    lv_obj_set_size(ui->screen_10_btn_1, 267, 99);

    //Write style for screen_10_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_10_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_10_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_10_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_10_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_10_btn_1, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_10_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_10_btn_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_img_12
    ui->screen_10_img_12 = lv_img_create(ui->screen_10_cont_3);
    lv_obj_add_flag(ui->screen_10_img_12, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_10_img_12, &_gvsdrbhgdh_alpha_147x47);
    lv_img_set_pivot(ui->screen_10_img_12, 50,50);
    lv_img_set_angle(ui->screen_10_img_12, 0);
    lv_obj_set_pos(ui->screen_10_img_12, 1126, 137);
    lv_obj_set_size(ui->screen_10_img_12, 147, 47);

    //Write style for screen_10_img_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_10_img_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_10_img_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_img_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_10_img_12, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_btn_12
    ui->screen_10_btn_12 = lv_btn_create(ui->screen_10_cont_3);
    ui->screen_10_btn_12_label = lv_label_create(ui->screen_10_btn_12);
    lv_label_set_text(ui->screen_10_btn_12_label, "");
    lv_label_set_long_mode(ui->screen_10_btn_12_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_10_btn_12_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_10_btn_12, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_10_btn_12_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_10_btn_12, 1078, 115);
    lv_obj_set_size(ui->screen_10_btn_12, 196, 91);

    //Write style for screen_10_btn_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_10_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_10_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_btn_12, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_10_btn_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_10_btn_12, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_10_btn_12, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_10_btn_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_10_btn_12, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_10_img_13
    ui->screen_10_img_13 = lv_img_create(ui->screen_10_cont_3);
    lv_obj_add_flag(ui->screen_10_img_13, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_10_img_13, &_sgderhbrdsdfgs_alpha_82x82);
    lv_img_set_pivot(ui->screen_10_img_13, 50,50);
    lv_img_set_angle(ui->screen_10_img_13, 0);
    lv_obj_set_pos(ui->screen_10_img_13, 1170, 24);
    lv_obj_set_size(ui->screen_10_img_13, 82, 82);

    //Write style for screen_10_img_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_10_img_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_10_img_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_10_img_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_10_img_13, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_10.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_10);

    //Init events for screen.
    events_init_screen_10(ui);
}
