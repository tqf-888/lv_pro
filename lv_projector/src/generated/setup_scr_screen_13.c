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



void setup_scr_screen_13(lv_ui *ui)
{
    //Write codes screen_13
    ui->screen_13 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_13, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_13, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_13, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_13, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_13_img_1
    ui->screen_13_img_1 = lv_img_create(ui->screen_13);
    lv_obj_add_flag(ui->screen_13_img_1, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_13_img_1, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\feswgvwes.png");
#else
    lv_img_set_src(ui->screen_13_img_1, "S:/usr/share/lv_projector/feswgvwes.png");
#endif
    lv_img_set_pivot(ui->screen_13_img_1, 50,50);
    lv_img_set_angle(ui->screen_13_img_1, 0);
    lv_obj_set_pos(ui->screen_13_img_1, 0, 0);
    lv_obj_set_size(ui->screen_13_img_1, 1280, 800);

    //Write style for screen_13_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_13_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_13_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_13_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_13_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_13_btn_1
    ui->screen_13_btn_1 = lv_btn_create(ui->screen_13);
    ui->screen_13_btn_1_label = lv_label_create(ui->screen_13_btn_1);
    lv_label_set_text(ui->screen_13_btn_1_label, "\n");
    lv_label_set_long_mode(ui->screen_13_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_13_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_13_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_13_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_13_btn_1, 4, 2);
    lv_obj_set_size(ui->screen_13_btn_1, 431, 175);

    //Write style for screen_13_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_13_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_13_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_13_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_13_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_13_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_13_btn_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_13_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_13_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_13_img_2
    ui->screen_13_img_2 = lv_img_create(ui->screen_13);
    lv_obj_add_flag(ui->screen_13_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_13_img_2, &_dhbdhnbfd_alpha_409x418);
    lv_img_set_pivot(ui->screen_13_img_2, 50,50);
    lv_img_set_angle(ui->screen_13_img_2, 0);
    lv_obj_set_pos(ui->screen_13_img_2, 751, 203);
    lv_obj_set_size(ui->screen_13_img_2, 409, 418);

    //Write style for screen_13_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_13_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_13_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_13_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_13_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_13.



    //Update current screen layout.
    lv_obj_update_layout(ui->screen_13);

    //Init events for screen.
    events_init_screen_13(ui);
}
