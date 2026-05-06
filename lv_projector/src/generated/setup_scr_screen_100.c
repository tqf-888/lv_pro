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



void setup_scr_screen_100(lv_ui *ui)
{
    //Write codes screen_100
    ui->screen_100 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_100, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_100, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_100, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_100, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_100, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_100, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_100_btn_1
    ui->screen_100_btn_1 = lv_btn_create(ui->screen_100);
    ui->screen_100_btn_1_label = lv_label_create(ui->screen_100_btn_1);
    lv_label_set_text(ui->screen_100_btn_1_label, "Button");
    lv_label_set_long_mode(ui->screen_100_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_100_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_100_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_100_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_100_btn_1, 51, 60);
    lv_obj_set_size(ui->screen_100_btn_1, 575, 222);

    //Write style for screen_100_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_100_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_100_btn_1, lv_color_hex(0xa600ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_100_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_100_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_100_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_100_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_100_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_100_btn_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_100_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_100_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_100_btn_2
    ui->screen_100_btn_2 = lv_btn_create(ui->screen_100);
    ui->screen_100_btn_2_label = lv_label_create(ui->screen_100_btn_2);
    lv_label_set_text(ui->screen_100_btn_2_label, "Button");
    lv_label_set_long_mode(ui->screen_100_btn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_100_btn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_100_btn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_100_btn_2_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_100_btn_2, 76, 320);
    lv_obj_set_size(ui->screen_100_btn_2, 573, 214);

    //Write style for screen_100_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_100_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_100_btn_2, lv_color_hex(0xff6500), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_100_btn_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_100_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_100_btn_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_100_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_100_btn_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_100_btn_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_100_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_100_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_100_label_1
    ui->screen_100_label_1 = lv_label_create(ui->screen_100);
    lv_label_set_text(ui->screen_100_label_1, "0    1         2    3    4\n\n5   6     7       8   9\n\n8\n");
    lv_label_set_long_mode(ui->screen_100_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_100_label_1, 708, 132);
    lv_obj_set_size(ui->screen_100_label_1, 368, 495);

    //Write style for screen_100_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_100_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_100_label_1, &lv_font_lv_font_ktv_30_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_100_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_100_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_100_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_100_label_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_100_label_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_100_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_100_img_1
    ui->screen_100_img_1 = lv_img_create(ui->screen_100);
    lv_obj_add_flag(ui->screen_100_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_100_img_1, &_ererhgredh1_alpha_44x35);
    lv_img_set_pivot(ui->screen_100_img_1, 50,50);
    lv_img_set_angle(ui->screen_100_img_1, 0);
    lv_obj_set_pos(ui->screen_100_img_1, 283, 645);
    lv_obj_set_size(ui->screen_100_img_1, 44, 35);

    //Write style for screen_100_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_100_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_100_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_100_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_100_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_100_img_2
    ui->screen_100_img_2 = lv_img_create(ui->screen_100);
    lv_obj_add_flag(ui->screen_100_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_100_img_2, &_ererhgredh4_alpha_44x35);
    lv_img_set_pivot(ui->screen_100_img_2, 50,50);
    lv_img_set_angle(ui->screen_100_img_2, 0);
    lv_obj_set_pos(ui->screen_100_img_2, 391, 649);
    lv_obj_set_size(ui->screen_100_img_2, 44, 35);

    //Write style for screen_100_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_100_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_100_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_100_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_100_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_100_img_3
    ui->screen_100_img_3 = lv_img_create(ui->screen_100);
    lv_obj_add_flag(ui->screen_100_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_100_img_3, &_ererhgredh3_alpha_44x35);
    lv_img_set_pivot(ui->screen_100_img_3, 50,50);
    lv_img_set_angle(ui->screen_100_img_3, 0);
    lv_obj_set_pos(ui->screen_100_img_3, 457, 658);
    lv_obj_set_size(ui->screen_100_img_3, 44, 35);

    //Write style for screen_100_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_100_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_100_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_100_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_100_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_100_img_4
    ui->screen_100_img_4 = lv_img_create(ui->screen_100);
    lv_obj_add_flag(ui->screen_100_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_100_img_4, &_ererhgredh2_alpha_44x35);
    lv_img_set_pivot(ui->screen_100_img_4, 50,50);
    lv_img_set_angle(ui->screen_100_img_4, 0);
    lv_obj_set_pos(ui->screen_100_img_4, 527, 658);
    lv_obj_set_size(ui->screen_100_img_4, 44, 35);

    //Write style for screen_100_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_100_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_100_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_100_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_100_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_100.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_100);

    //Init events for screen.
    events_init_screen_100(ui);
}
