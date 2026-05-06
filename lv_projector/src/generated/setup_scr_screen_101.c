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



void setup_scr_screen_101(lv_ui *ui)
{
    //Write codes screen_101
    ui->screen_101 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_101, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_101, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_101, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_101, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_101_cont_1
    ui->screen_101_cont_1 = lv_obj_create(ui->screen_101);
    lv_obj_set_pos(ui->screen_101_cont_1, 1, 2);
    lv_obj_set_size(ui->screen_101_cont_1, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_101_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_101_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_101_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_101_btn_1
    ui->screen_101_btn_1 = lv_btn_create(ui->screen_101);
    ui->screen_101_btn_1_label = lv_label_create(ui->screen_101_btn_1);
    lv_label_set_text(ui->screen_101_btn_1_label, "");
    lv_label_set_long_mode(ui->screen_101_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_101_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_101_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_101_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_101_btn_1, 7, 7);
    lv_obj_set_size(ui->screen_101_btn_1, 200, 109);

    //Write style for screen_101_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_101_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_101_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_101_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_101_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_101_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_101_btn_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_101_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_101_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_101.
    test_open_folder_browser(guider_ui.screen_101_cont_1);



    //Update current screen layout.
    lv_obj_update_layout(ui->screen_101);

    //Init events for screen.
    events_init_screen_101(ui);
}
