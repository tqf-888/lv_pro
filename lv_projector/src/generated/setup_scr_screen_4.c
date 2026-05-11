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
#include "lv_wifi_list_demo.h"


lv_calendar_date_t screen_4_calendar_1_today;
lv_calendar_date_t screen_4_calendar_1_highlihted_days[1];
void setup_scr_screen_4(lv_ui *ui)
{
    //Write codes screen_4
    ui->screen_4 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_4, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_4, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_9
    ui->screen_4_cont_9 = lv_obj_create(ui->screen_4);
    lv_obj_set_pos(ui->screen_4_cont_9, 364, 14);
    lv_obj_set_size(ui->screen_4_cont_9, 868, 776);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_9, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_4_cont_9, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_4_cont_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_9, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_9, 45, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_9, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_17
    ui->screen_4_cont_17 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_17, 34, 672);
    lv_obj_set_size(ui->screen_4_cont_17, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_17, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_17, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_17, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_17, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_17, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_17, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_8
    ui->screen_4_label_8 = lv_label_create(ui->screen_4_cont_17);
    lv_label_set_text(ui->screen_4_label_8, "缩放");
    lv_label_set_long_mode(ui->screen_4_label_8, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_8, 15, 16);
    lv_obj_set_size(ui->screen_4_label_8, 204, 36);

    //Write style for screen_4_label_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_8, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_8, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_8, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_slider_7
    ui->screen_4_slider_7 = lv_slider_create(ui->screen_4_cont_17);
    lv_slider_set_range(ui->screen_4_slider_7, 80, 100);
    lv_slider_set_mode(ui->screen_4_slider_7, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_4_slider_7, 90, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_4_slider_7, 272, 30);
    lv_obj_set_size(ui->screen_4_slider_7, 388, 12);

    //Write style for screen_4_slider_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_7, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_7, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_7, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_7, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_4_slider_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_slider_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_7, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_7, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_7, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_7, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_7, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_7, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_7, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_7, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_7, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_7, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_16
    ui->screen_4_cont_16 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_16, 34, 580);
    lv_obj_set_size(ui->screen_4_cont_16, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_16, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_16, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_16, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_16, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_16, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_16, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_7
    ui->screen_4_label_7 = lv_label_create(ui->screen_4_cont_16);
    lv_label_set_text(ui->screen_4_label_7, "画面比例");
    lv_label_set_long_mode(ui->screen_4_label_7, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_7, 15, 16);
    lv_obj_set_size(ui->screen_4_label_7, 204, 36);

    //Write style for screen_4_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_7, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_7, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_7, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_5
    ui->screen_4_ddlist_5 = lv_dropdown_create(ui->screen_4_cont_16);
    lv_dropdown_set_options(ui->screen_4_ddlist_5, "16:9\n4:3");
    lv_obj_set_pos(ui->screen_4_ddlist_5, 602, 15);
    lv_obj_set_size(ui->screen_4_ddlist_5, 174, 39);

    //Write style for screen_4_ddlist_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_5, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_5, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_5, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_5, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_5, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_5, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_5_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_5_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_5_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_5_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_5_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_5_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_5_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_5_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_5), &style_screen_4_ddlist_5_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_5_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_5_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_5_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_5_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_5_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_5_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_5_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_5_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_5_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_5_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_5_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_5_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_5), &style_screen_4_ddlist_5_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_15
    ui->screen_4_cont_15 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_15, 34, 489);
    lv_obj_set_size(ui->screen_4_cont_15, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_15, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_15, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_15, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_15, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_15, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_6
    ui->screen_4_label_6 = lv_label_create(ui->screen_4_cont_15);
    lv_label_set_text(ui->screen_4_label_6, "色温");
    lv_label_set_long_mode(ui->screen_4_label_6, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_6, 15, 16);
    lv_obj_set_size(ui->screen_4_label_6, 204, 36);

    //Write style for screen_4_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_6, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_6, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_4
    ui->screen_4_ddlist_4 = lv_dropdown_create(ui->screen_4_cont_15);
    lv_dropdown_set_options(ui->screen_4_ddlist_4, "标准\n冷色温\n暖色温");
    lv_obj_set_pos(ui->screen_4_ddlist_4, 601, 15);
    lv_obj_set_size(ui->screen_4_ddlist_4, 174, 39);

    //Write style for screen_4_ddlist_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_4, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_4, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_4, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_4, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_4, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_4, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_4_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_4_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_4_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_4_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_4_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_4_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_4_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_4_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_4), &style_screen_4_ddlist_4_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_4_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_4_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_4_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_4_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_4_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_4_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_4_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_4_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_4_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_4_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_4_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_4_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_4), &style_screen_4_ddlist_4_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_14
    ui->screen_4_cont_14 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_14, 34, 398);
    lv_obj_set_size(ui->screen_4_cont_14, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_14, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_14, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_14, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_14, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_14, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_5
    ui->screen_4_label_5 = lv_label_create(ui->screen_4_cont_14);
    lv_label_set_text(ui->screen_4_label_5, "清晰度");
    lv_label_set_long_mode(ui->screen_4_label_5, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_5, 14, 17);
    lv_obj_set_size(ui->screen_4_label_5, 204, 36);

    //Write style for screen_4_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_5, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_5, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_slider_4
    ui->screen_4_slider_4 = lv_slider_create(ui->screen_4_cont_14);
    lv_slider_set_range(ui->screen_4_slider_4, 0, 10);
    lv_slider_set_mode(ui->screen_4_slider_4, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_4_slider_4, 5, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_4_slider_4, 277, 29);
    lv_obj_set_size(ui->screen_4_slider_4, 388, 12);

    //Write style for screen_4_slider_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_4, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_4, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_4, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_4_slider_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_slider_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_4, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_4, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_4, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_4, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_4, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_4, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_4, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_4, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_4, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_4, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_13
    ui->screen_4_cont_13 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_13, 34, 307);
    lv_obj_set_size(ui->screen_4_cont_13, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_13, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_13, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_13, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_13, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_13, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_4
    ui->screen_4_label_4 = lv_label_create(ui->screen_4_cont_13);
    lv_label_set_text(ui->screen_4_label_4, "颜色");
    lv_label_set_long_mode(ui->screen_4_label_4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_4, 14, 17);
    lv_obj_set_size(ui->screen_4_label_4, 204, 36);

    //Write style for screen_4_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_4, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_slider_3
    ui->screen_4_slider_3 = lv_slider_create(ui->screen_4_cont_13);
    lv_slider_set_range(ui->screen_4_slider_3, 0, 100);
    lv_slider_set_mode(ui->screen_4_slider_3, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_4_slider_3, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_4_slider_3, 273, 29);
    lv_obj_set_size(ui->screen_4_slider_3, 388, 12);

    //Write style for screen_4_slider_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_3, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_3, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_4_slider_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_slider_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_3, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_3, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_3, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_3, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_3, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_3, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_3, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_3, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_3, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_3, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_12
    ui->screen_4_cont_12 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_12, 34, 216);
    lv_obj_set_size(ui->screen_4_cont_12, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_12, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_12, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_12, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_12, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_12, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_3
    ui->screen_4_label_3 = lv_label_create(ui->screen_4_cont_12);
    lv_label_set_text(ui->screen_4_label_3, "亮度");
    lv_label_set_long_mode(ui->screen_4_label_3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_3, 15, 16);
    lv_obj_set_size(ui->screen_4_label_3, 204, 36);

    //Write style for screen_4_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_3, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_slider_2
    ui->screen_4_slider_2 = lv_slider_create(ui->screen_4_cont_12);
    lv_slider_set_range(ui->screen_4_slider_2, 0, 100);
    lv_slider_set_mode(ui->screen_4_slider_2, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_4_slider_2, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_4_slider_2, 276, 29);
    lv_obj_set_size(ui->screen_4_slider_2, 388, 12);

    //Write style for screen_4_slider_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_2, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_2, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_4_slider_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_slider_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_2, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_2, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_2, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_2, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_2, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_2, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_2, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_2, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_2, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_11
    ui->screen_4_cont_11 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_11, 34, 127);
    lv_obj_set_size(ui->screen_4_cont_11, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_11, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_11, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_11, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_11, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_11, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_2
    ui->screen_4_label_2 = lv_label_create(ui->screen_4_cont_11);
    lv_label_set_text(ui->screen_4_label_2, "对比度");
    lv_label_set_long_mode(ui->screen_4_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_2, 15, 16);
    lv_obj_set_size(ui->screen_4_label_2, 204, 36);

    //Write style for screen_4_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_2, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_slider_1
    ui->screen_4_slider_1 = lv_slider_create(ui->screen_4_cont_11);
    lv_slider_set_range(ui->screen_4_slider_1, 0, 100);
    lv_slider_set_mode(ui->screen_4_slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_4_slider_1, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_4_slider_1, 266, 28);
    lv_obj_set_size(ui->screen_4_slider_1, 388, 12);

    //Write style for screen_4_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_1, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_4_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_1, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_1, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_1, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_10
    ui->screen_4_cont_10 = lv_obj_create(ui->screen_4_cont_9);
    lv_obj_set_pos(ui->screen_4_cont_10, 34, 34);
    lv_obj_set_size(ui->screen_4_cont_10, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_10, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_10, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_10, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_10, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_10, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_1
    ui->screen_4_label_1 = lv_label_create(ui->screen_4_cont_10);
    lv_label_set_text(ui->screen_4_label_1, "图像模式");
    lv_label_set_long_mode(ui->screen_4_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_1, 15, 16);
    lv_obj_set_size(ui->screen_4_label_1, 204, 36);

    //Write style for screen_4_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_1, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_1
    ui->screen_4_ddlist_1 = lv_dropdown_create(ui->screen_4_cont_10);
    lv_dropdown_set_options(ui->screen_4_ddlist_1, "标准\n动态\n温和\n用户");
    lv_obj_set_pos(ui->screen_4_ddlist_1, 603, 12);
    lv_obj_set_size(ui->screen_4_ddlist_1, 174, 39);

    //Write style for screen_4_ddlist_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_1, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_1, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_1_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_1_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_1_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_1_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_1_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_1_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_1_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_1_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_1), &style_screen_4_ddlist_1_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_1_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_1_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_1_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_1_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_1_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_1_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_1_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_1_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_1_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_1_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_1_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_1_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_1), &style_screen_4_ddlist_1_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_18
    ui->screen_4_cont_18 = lv_obj_create(ui->screen_4);
    lv_obj_set_pos(ui->screen_4_cont_18, 364, 71);
    lv_obj_set_size(ui->screen_4_cont_18, 868, 412);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_18, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_4_cont_18, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_4_cont_18, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_18, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_18, 45, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_18, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_18, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_18, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_24
    ui->screen_4_cont_24 = lv_obj_create(ui->screen_4_cont_18);
    lv_obj_set_pos(ui->screen_4_cont_24, 36, 307);
    lv_obj_set_size(ui->screen_4_cont_24, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_24, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_24, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_24, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_24, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_24, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_24, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_15
    ui->screen_4_label_15 = lv_label_create(ui->screen_4_cont_24);
    lv_label_set_text(ui->screen_4_label_15, "声音输出");
    lv_label_set_long_mode(ui->screen_4_label_15, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_15, 15, 16);
    lv_obj_set_size(ui->screen_4_label_15, 204, 36);

    //Write style for screen_4_label_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_15, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_15, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_15, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_15, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_7
    ui->screen_4_ddlist_7 = lv_dropdown_create(ui->screen_4_cont_24);
    lv_dropdown_set_options(ui->screen_4_ddlist_7, "标准\n喇叭\nHDMI ARC\n蓝牙\n耳机\nOWA");
    lv_obj_set_pos(ui->screen_4_ddlist_7, 601, 15);
    lv_obj_set_size(ui->screen_4_ddlist_7, 174, 39);

    //Write style for screen_4_ddlist_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_7, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_7, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_7, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_7, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_7, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_7, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_7, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_7, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_7_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_7_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_7_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_7_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_7_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_7_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_7_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_7_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_7), &style_screen_4_ddlist_7_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_7_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_7_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_7_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_7_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_7_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_7_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_7_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_7_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_7_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_7_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_7_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_7_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_7), &style_screen_4_ddlist_7_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_21
    ui->screen_4_cont_21 = lv_obj_create(ui->screen_4_cont_18);
    lv_obj_set_pos(ui->screen_4_cont_21, 34, 216);
    lv_obj_set_size(ui->screen_4_cont_21, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_21, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_21, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_21, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_21, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_21, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_21, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_12
    ui->screen_4_label_12 = lv_label_create(ui->screen_4_cont_21);
    lv_label_set_text(ui->screen_4_label_12, "高音");
    lv_label_set_long_mode(ui->screen_4_label_12, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_12, 15, 16);
    lv_obj_set_size(ui->screen_4_label_12, 204, 36);

    //Write style for screen_4_label_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_12, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_12, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_12, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_slider_6
    ui->screen_4_slider_6 = lv_slider_create(ui->screen_4_cont_21);
    lv_slider_set_range(ui->screen_4_slider_6, -10, 10);
    lv_slider_set_mode(ui->screen_4_slider_6, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_4_slider_6, 0, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_4_slider_6, 276, 30);
    lv_obj_set_size(ui->screen_4_slider_6, 388, 12);

    //Write style for screen_4_slider_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_6, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_6, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_6, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_6, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_4_slider_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_slider_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_6, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_6, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_6, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_6, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_6, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_6, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_6, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_6, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_6, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_6, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_20
    ui->screen_4_cont_20 = lv_obj_create(ui->screen_4_cont_18);
    lv_obj_set_pos(ui->screen_4_cont_20, 34, 127);
    lv_obj_set_size(ui->screen_4_cont_20, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_20, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_20, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_20, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_20, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_20, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_20, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_11
    ui->screen_4_label_11 = lv_label_create(ui->screen_4_cont_20);
    lv_label_set_text(ui->screen_4_label_11, "低音");
    lv_label_set_long_mode(ui->screen_4_label_11, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_11, 15, 16);
    lv_obj_set_size(ui->screen_4_label_11, 204, 36);

    //Write style for screen_4_label_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_11, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_11, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_11, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_slider_5
    ui->screen_4_slider_5 = lv_slider_create(ui->screen_4_cont_20);
    lv_slider_set_range(ui->screen_4_slider_5, -10, 10);
    lv_slider_set_mode(ui->screen_4_slider_5, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_4_slider_5, 0, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_4_slider_5, 266, 28);
    lv_obj_set_size(ui->screen_4_slider_5, 388, 12);

    //Write style for screen_4_slider_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_5, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_5, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_5, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_4_slider_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_slider_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_5, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_5, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_5, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_5, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_5, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_4_slider_5, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_slider_5, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_slider_5, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_slider_5, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_slider_5, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_19
    ui->screen_4_cont_19 = lv_obj_create(ui->screen_4_cont_18);
    lv_obj_set_pos(ui->screen_4_cont_19, 34, 34);
    lv_obj_set_size(ui->screen_4_cont_19, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_19, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_19, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_19, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_19, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_19, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_19, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_10
    ui->screen_4_label_10 = lv_label_create(ui->screen_4_cont_19);
    lv_label_set_text(ui->screen_4_label_10, "声音模式");
    lv_label_set_long_mode(ui->screen_4_label_10, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_10, 15, 15);
    lv_obj_set_size(ui->screen_4_label_10, 204, 36);

    //Write style for screen_4_label_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_10, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_10, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_10, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_6
    ui->screen_4_ddlist_6 = lv_dropdown_create(ui->screen_4_cont_19);
    lv_dropdown_set_options(ui->screen_4_ddlist_6, "标准\n音乐\n电影\n运动");
    lv_obj_set_pos(ui->screen_4_ddlist_6, 602, 11);
    lv_obj_set_size(ui->screen_4_ddlist_6, 174, 39);

    //Write style for screen_4_ddlist_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_6, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_6, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_6, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_6, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_6, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_6, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_6, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_6_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_6_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_6_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_6_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_6_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_6_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_6_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_6_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_6), &style_screen_4_ddlist_6_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_6_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_6_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_6_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_6_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_6_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_6_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_6_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_6_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_6_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_6_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_6_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_6_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_6), &style_screen_4_ddlist_6_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_25
    ui->screen_4_cont_25 = lv_obj_create(ui->screen_4);
    lv_obj_set_pos(ui->screen_4_cont_25, 369, 112);
    lv_obj_set_size(ui->screen_4_cont_25, 868, 502);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_25, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_4_cont_25, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_4_cont_25, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_25, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_25, 45, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_25, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_25, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_31
    ui->screen_4_cont_31 = lv_obj_create(ui->screen_4_cont_25);
    lv_obj_set_pos(ui->screen_4_cont_31, 34, 125);
    lv_obj_set_size(ui->screen_4_cont_31, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_31, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_31, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_31, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_31, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_31, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_31, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_21
    ui->screen_4_label_21 = lv_label_create(ui->screen_4_cont_31);
    lv_label_set_text(ui->screen_4_label_21, "投屏模式");
    lv_label_set_long_mode(ui->screen_4_label_21, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_21, 15, 16);
    lv_obj_set_size(ui->screen_4_label_21, 204, 36);

    //Write style for screen_4_label_21, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_21, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_21, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_21, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_21, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_9
    ui->screen_4_ddlist_9 = lv_dropdown_create(ui->screen_4_cont_31);
    lv_dropdown_set_options(ui->screen_4_ddlist_9, "桌上正投\n吊装正投\n桌上背投\n吊装背投");
    lv_obj_set_pos(ui->screen_4_ddlist_9, 601, 15);
    lv_obj_set_size(ui->screen_4_ddlist_9, 174, 39);

    //Write style for screen_4_ddlist_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_9, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_9, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_9, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_9, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_9, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_9, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_9, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_9_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_9_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_9_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_9_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_9_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_9_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_9_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_9_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_9), &style_screen_4_ddlist_9_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_9_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_9_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_9_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_9_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_9_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_9_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_9_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_9_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_9_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_9_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_9_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_9_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_9), &style_screen_4_ddlist_9_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_26
    ui->screen_4_cont_26 = lv_obj_create(ui->screen_4_cont_25);
    lv_obj_set_pos(ui->screen_4_cont_26, 34, 34);
    lv_obj_set_size(ui->screen_4_cont_26, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_26, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_26, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_26, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_26, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_26, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_26, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_16
    ui->screen_4_label_16 = lv_label_create(ui->screen_4_cont_26);
    lv_label_set_text(ui->screen_4_label_16, "语言设置");
    lv_label_set_long_mode(ui->screen_4_label_16, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_16, 15, 16);
    lv_obj_set_size(ui->screen_4_label_16, 204, 36);

    //Write style for screen_4_label_16, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_16, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_16, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_16, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_16, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_8
    ui->screen_4_ddlist_8 = lv_dropdown_create(ui->screen_4_cont_26);
    lv_dropdown_set_options(ui->screen_4_ddlist_8, "中文");
    lv_obj_set_pos(ui->screen_4_ddlist_8, 603, 12);
    lv_obj_set_size(ui->screen_4_ddlist_8, 174, 39);

    //Write style for screen_4_ddlist_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_8, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_8, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_8, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_8, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_8, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_8, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_8, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_8, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_8_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_8_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_8_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_8_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_8_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_8_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_8_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_8_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_8), &style_screen_4_ddlist_8_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_8_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_8_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_8_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_8_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_8_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_8_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_8_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_8_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_8_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_8_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_8_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_8_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_8), &style_screen_4_ddlist_8_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_35
    ui->screen_4_cont_35 = lv_obj_create(ui->screen_4_cont_25);
    lv_obj_set_pos(ui->screen_4_cont_35, 34, 216);
    lv_obj_set_size(ui->screen_4_cont_35, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_35, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_35, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_35, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_35, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_35, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_35, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_35, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_35, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_35, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_35, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_35, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_35, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_25
    ui->screen_4_label_25 = lv_label_create(ui->screen_4_cont_35);
    lv_label_set_text(ui->screen_4_label_25, "恢复出厂设置");
    lv_label_set_long_mode(ui->screen_4_label_25, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_25, 15, 16);
    lv_obj_set_size(ui->screen_4_label_25, 204, 36);

    //Write style for screen_4_label_25, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_25, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_25, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_25, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_25, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_34
    ui->screen_4_cont_34 = lv_obj_create(ui->screen_4_cont_25);
    lv_obj_set_pos(ui->screen_4_cont_34, 34, 307);
    lv_obj_set_size(ui->screen_4_cont_34, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_34, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_34, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_34, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_34, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_34, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_34, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_34, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_34, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_34, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_34, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_34, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_34, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_24
    ui->screen_4_label_24 = lv_label_create(ui->screen_4_cont_34);
    lv_label_set_text(ui->screen_4_label_24, "软件升级");
    lv_label_set_long_mode(ui->screen_4_label_24, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_24, 15, 16);
    lv_obj_set_size(ui->screen_4_label_24, 204, 36);

    //Write style for screen_4_label_24, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_24, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_24, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_24, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_24, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_33
    ui->screen_4_cont_33 = lv_obj_create(ui->screen_4_cont_25);
    lv_obj_set_pos(ui->screen_4_cont_33, 34, 398);
    lv_obj_set_size(ui->screen_4_cont_33, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_33, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_33, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_33, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_33, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_33, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_33, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_23
    ui->screen_4_label_23 = lv_label_create(ui->screen_4_cont_33);
    lv_label_set_text(ui->screen_4_label_23, "自动休眠");
    lv_label_set_long_mode(ui->screen_4_label_23, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_23, 15, 16);
    lv_obj_set_size(ui->screen_4_label_23, 204, 36);

    //Write style for screen_4_label_23, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_23, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_23, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_23, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_23, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_ddlist_11
    ui->screen_4_ddlist_11 = lv_dropdown_create(ui->screen_4_cont_33);
    lv_dropdown_set_options(ui->screen_4_ddlist_11, "10分钟\n20分钟\n30分钟");
    lv_obj_set_pos(ui->screen_4_ddlist_11, 602, 15);
    lv_obj_set_size(ui->screen_4_ddlist_11, 174, 39);

    //Write style for screen_4_ddlist_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_4_ddlist_11, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_ddlist_11, &lv_font_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_ddlist_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_ddlist_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_ddlist_11, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_ddlist_11, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_ddlist_11, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_ddlist_11, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_ddlist_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_ddlist_11, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_ddlist_11, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_ddlist_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_4_ddlist_11_extra_list_selected_checked
    static lv_style_t style_screen_4_ddlist_11_extra_list_selected_checked;
    ui_init_style(&style_screen_4_ddlist_11_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_4_ddlist_11_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_screen_4_ddlist_11_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_11_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_11_extra_list_selected_checked, lv_color_hex(0x999999));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_11_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_11), &style_screen_4_ddlist_11_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_ddlist_11_extra_list_main_default
    static lv_style_t style_screen_4_ddlist_11_extra_list_main_default;
    ui_init_style(&style_screen_4_ddlist_11_extra_list_main_default);

    lv_style_set_max_height(&style_screen_4_ddlist_11_extra_list_main_default, 160);
    lv_style_set_text_color(&style_screen_4_ddlist_11_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_4_ddlist_11_extra_list_main_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_screen_4_ddlist_11_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_4_ddlist_11_extra_list_main_default, 0);
    lv_style_set_radius(&style_screen_4_ddlist_11_extra_list_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_ddlist_11_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_ddlist_11_extra_list_main_default, lv_color_hex(0xefefef));
    lv_style_set_bg_grad_dir(&style_screen_4_ddlist_11_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_4_ddlist_11), &style_screen_4_ddlist_11_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_36
    ui->screen_4_cont_36 = lv_obj_create(ui->screen_4);
    lv_obj_set_pos(ui->screen_4_cont_36, 369, 112);
    lv_obj_set_size(ui->screen_4_cont_36, 868, 502);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_36, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_4_cont_36, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_4_cont_36, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_36, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_36, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_36, 45, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_36, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_36, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_36, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_36, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_36, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_36, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_36, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_41
    ui->screen_4_cont_41 = lv_obj_create(ui->screen_4_cont_36);
    lv_obj_set_pos(ui->screen_4_cont_41, 34, 125);
    lv_obj_set_size(ui->screen_4_cont_41, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_41, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_41, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_41, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_41, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_41, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_41, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_30
    ui->screen_4_label_30 = lv_label_create(ui->screen_4_cont_41);
    lv_label_set_text(ui->screen_4_label_30, "手动梯形校正");
    lv_label_set_long_mode(ui->screen_4_label_30, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_30, 15, 16);
    lv_obj_set_size(ui->screen_4_label_30, 204, 36);

    //Write style for screen_4_label_30, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_30, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_30, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_30, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_30, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_40
    ui->screen_4_cont_40 = lv_obj_create(ui->screen_4_cont_36);
    lv_obj_set_pos(ui->screen_4_cont_40, 34, 34);
    lv_obj_set_size(ui->screen_4_cont_40, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_40, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_40, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_40, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_40, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_40, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_40, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_29
    ui->screen_4_label_29 = lv_label_create(ui->screen_4_cont_40);
    lv_label_set_text(ui->screen_4_label_29, "自动梯形校正");
    lv_label_set_long_mode(ui->screen_4_label_29, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_29, 15, 16);
    lv_obj_set_size(ui->screen_4_label_29, 204, 36);

    //Write style for screen_4_label_29, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_29, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_29, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_29, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_29, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_sw_1
    ui->screen_4_sw_1 = lv_switch_create(ui->screen_4_cont_40);
    lv_obj_set_pos(ui->screen_4_sw_1, 654, 21);
    lv_obj_set_size(ui->screen_4_sw_1, 77, 28);

    //Write style for screen_4_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_sw_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_sw_1, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_sw_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_sw_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_4_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_4_sw_1, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_4_sw_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_sw_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_4_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_4_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_sw_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_sw_1, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_sw_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_sw_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_sw_1, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_39
    ui->screen_4_cont_39 = lv_obj_create(ui->screen_4_cont_36);
    lv_obj_set_pos(ui->screen_4_cont_39, 35, 216);
    lv_obj_set_size(ui->screen_4_cont_39, 792, 64);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_39, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_39, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_39, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_39, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_39, 115, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_39, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_39, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_39, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_39, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_39, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_39, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_39, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_28
    ui->screen_4_label_28 = lv_label_create(ui->screen_4_cont_39);
    lv_label_set_text(ui->screen_4_label_28, "手动梯形校正");
    lv_label_set_long_mode(ui->screen_4_label_28, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_28, 15, 16);
    lv_obj_set_size(ui->screen_4_label_28, 204, 36);

    //Write style for screen_4_label_28, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_28, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_28, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_28, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_28, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_label_31
    ui->screen_4_label_31 = lv_label_create(ui->screen_4_cont_39);
    lv_label_set_text(ui->screen_4_label_31, "重置");
    lv_label_set_long_mode(ui->screen_4_label_31, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_31, 656, 15);
    lv_obj_set_size(ui->screen_4_label_31, 100, 36);

    //Write style for screen_4_label_31, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_31, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_31, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_31, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_31, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_42
    ui->screen_4_cont_42 = lv_obj_create(ui->screen_4);
    lv_obj_set_pos(ui->screen_4_cont_42, 369, 112);
    lv_obj_set_size(ui->screen_4_cont_42, 868, 609);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_42, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_4_cont_42, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_4_cont_42, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_42, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_42, 45, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_4_cont_42, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_4_cont_42, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_calendar_1
    ui->screen_4_calendar_1 = lv_calendar_create(ui->screen_4_cont_42);
    screen_4_calendar_1_today.year = 2026;
    screen_4_calendar_1_today.month = 5;
    screen_4_calendar_1_today.day = 10;
    lv_calendar_set_today_date(ui->screen_4_calendar_1, screen_4_calendar_1_today.year, screen_4_calendar_1_today.month, screen_4_calendar_1_today.day);
    lv_calendar_set_showed_date(ui->screen_4_calendar_1, screen_4_calendar_1_today.year, screen_4_calendar_1_today.month);
    screen_4_calendar_1_highlihted_days[0].year = 2026;
    screen_4_calendar_1_highlihted_days[0].month = 5;
    screen_4_calendar_1_highlihted_days[0].day = 11;
    lv_calendar_set_highlighted_dates(ui->screen_4_calendar_1, screen_4_calendar_1_highlihted_days, 1);
    lv_obj_t *screen_4_calendar_1_header = lv_calendar_header_arrow_create(ui->screen_4_calendar_1);
    lv_calendar_t *screen_4_calendar_1 = (lv_calendar_t *)ui->screen_4_calendar_1;
    lv_obj_add_event_cb(screen_4_calendar_1->btnm, screen_4_calendar_1_draw_part_begin_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_add_event_cb(ui->screen_4_calendar_1, screen_4_calendar_1_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(ui->screen_4_calendar_1, 22, 10);
    lv_obj_set_size(ui->screen_4_calendar_1, 832, 581);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_calendar_1_main_main_default
    static lv_style_t style_screen_4_calendar_1_main_main_default;
    ui_init_style(&style_screen_4_calendar_1_main_main_default);

    lv_style_set_border_width(&style_screen_4_calendar_1_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_4_calendar_1_main_main_default, 255);
    lv_style_set_border_color(&style_screen_4_calendar_1_main_main_default, lv_color_hex(0xc0c0c0));
    lv_style_set_border_side(&style_screen_4_calendar_1_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_bg_opa(&style_screen_4_calendar_1_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_calendar_1_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_4_calendar_1_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_shadow_width(&style_screen_4_calendar_1_main_main_default, 0);
    lv_style_set_radius(&style_screen_4_calendar_1_main_main_default, 0);
    lv_obj_add_style(ui->screen_4_calendar_1, &style_screen_4_calendar_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_calendar_1_extra_header_main_default
    static lv_style_t style_screen_4_calendar_1_extra_header_main_default;
    ui_init_style(&style_screen_4_calendar_1_extra_header_main_default);

    lv_style_set_text_color(&style_screen_4_calendar_1_extra_header_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_screen_4_calendar_1_extra_header_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_4_calendar_1_extra_header_main_default, 255);
    lv_style_set_bg_opa(&style_screen_4_calendar_1_extra_header_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_calendar_1_extra_header_main_default, lv_color_hex(0x2195f6));
    lv_style_set_bg_grad_dir(&style_screen_4_calendar_1_extra_header_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(screen_4_calendar_1_header, &style_screen_4_calendar_1_extra_header_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_calendar_1_main_items_default
    static lv_style_t style_screen_4_calendar_1_main_items_default;
    ui_init_style(&style_screen_4_calendar_1_main_items_default);

    lv_style_set_bg_opa(&style_screen_4_calendar_1_main_items_default, 0);
    lv_style_set_border_width(&style_screen_4_calendar_1_main_items_default, 1);
    lv_style_set_border_opa(&style_screen_4_calendar_1_main_items_default, 255);
    lv_style_set_border_color(&style_screen_4_calendar_1_main_items_default, lv_color_hex(0xc0c0c0));
    lv_style_set_border_side(&style_screen_4_calendar_1_main_items_default, LV_BORDER_SIDE_FULL);
    lv_style_set_text_color(&style_screen_4_calendar_1_main_items_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_4_calendar_1_main_items_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_4_calendar_1_main_items_default, 255);
    lv_obj_add_style(lv_calendar_get_btnmatrix(ui->screen_4_calendar_1), &style_screen_4_calendar_1_main_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes screen_4_cont_43
    ui->screen_4_cont_43 = lv_obj_create(ui->screen_4);
    lv_obj_set_pos(ui->screen_4_cont_43, 359, 83);
    lv_obj_set_size(ui->screen_4_cont_43, 906, 707);
    lv_obj_set_scrollbar_mode(ui->screen_4_cont_43, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_4_cont_43, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_cont_43, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_cont_43, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_cont_43, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_cont_43, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_cont_43, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_cont_43, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_cont_43, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_cont_43, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_list_1
    ui->screen_4_list_1 = lv_list_create(ui->screen_4);
    ui->screen_4_list_1_item0 = lv_list_add_btn(ui->screen_4_list_1, LV_SYMBOL_WIFI, "网络");
    ui->screen_4_list_1_item1 = lv_list_add_btn(ui->screen_4_list_1, LV_SYMBOL_AUDIO, "声音");
    ui->screen_4_list_1_item2 = lv_list_add_btn(ui->screen_4_list_1, LV_SYMBOL_SETTINGS, "系统");
    ui->screen_4_list_1_item3 = lv_list_add_btn(ui->screen_4_list_1, LV_SYMBOL_VIDEO, "梯形校正");
    ui->screen_4_list_1_item4 = lv_list_add_btn(ui->screen_4_list_1, LV_SYMBOL_LOOP, "日历");
    ui->screen_4_list_1_item5 = lv_list_add_btn(ui->screen_4_list_1, LV_SYMBOL_IMAGE, "图像");
    lv_obj_set_pos(ui->screen_4_list_1, 21, 87);
    lv_obj_set_size(ui->screen_4_list_1, 322, 667);
    lv_obj_set_scrollbar_mode(ui->screen_4_list_1, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_list_1_main_main_default
    static lv_style_t style_screen_4_list_1_main_main_default;
    ui_init_style(&style_screen_4_list_1_main_main_default);

    lv_style_set_pad_top(&style_screen_4_list_1_main_main_default, 0);
    lv_style_set_pad_left(&style_screen_4_list_1_main_main_default, 0);
    lv_style_set_pad_right(&style_screen_4_list_1_main_main_default, 0);
    lv_style_set_pad_bottom(&style_screen_4_list_1_main_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_list_1_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_list_1_main_main_default, lv_color_hex(0x1e1e1e));
    lv_style_set_bg_grad_dir(&style_screen_4_list_1_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_4_list_1_main_main_default, 2);
    lv_style_set_border_opa(&style_screen_4_list_1_main_main_default, 25);
    lv_style_set_border_color(&style_screen_4_list_1_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_border_side(&style_screen_4_list_1_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_4_list_1_main_main_default, 22);
    lv_style_set_shadow_width(&style_screen_4_list_1_main_main_default, 0);
    lv_obj_add_style(ui->screen_4_list_1, &style_screen_4_list_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_list_1_extra_btns_main_default
    static lv_style_t style_screen_4_list_1_extra_btns_main_default;
    ui_init_style(&style_screen_4_list_1_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_4_list_1_extra_btns_main_default, 26);
    lv_style_set_pad_left(&style_screen_4_list_1_extra_btns_main_default, 26);
    lv_style_set_pad_right(&style_screen_4_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_4_list_1_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_4_list_1_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_4_list_1_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_screen_4_list_1_extra_btns_main_default, &lv_font_Regular_31);
    lv_style_set_text_opa(&style_screen_4_list_1_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_4_list_1_extra_btns_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_list_1_extra_btns_main_default, 0);
    lv_obj_add_style(ui->screen_4_list_1_item5, &style_screen_4_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_4_list_1_item4, &style_screen_4_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_4_list_1_item3, &style_screen_4_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_4_list_1_item2, &style_screen_4_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_4_list_1_item1, &style_screen_4_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_4_list_1_item0, &style_screen_4_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_4_list_1_extra_texts_main_default
    static lv_style_t style_screen_4_list_1_extra_texts_main_default;
    ui_init_style(&style_screen_4_list_1_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_4_list_1_extra_texts_main_default, 50);
    lv_style_set_pad_left(&style_screen_4_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_4_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_4_list_1_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_4_list_1_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_4_list_1_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_screen_4_list_1_extra_texts_main_default, &lv_font_Regular_31);
    lv_style_set_text_opa(&style_screen_4_list_1_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_4_list_1_extra_texts_main_default, 4);
    lv_style_set_transform_width(&style_screen_4_list_1_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_4_list_1_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_4_list_1_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_4_list_1_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_4_label_9
    ui->screen_4_label_9 = lv_label_create(ui->screen_4);
    lv_label_set_text(ui->screen_4_label_9, "设置");
    lv_label_set_long_mode(ui->screen_4_label_9, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_4_label_9, 53, 41);
    lv_obj_set_size(ui->screen_4_label_9, 129, 39);

    //Write style for screen_4_label_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_label_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_label_9, &lv_font_Regular_31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_label_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_label_9, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_4_btn_1
    ui->screen_4_btn_1 = lv_btn_create(ui->screen_4);
    ui->screen_4_btn_1_label = lv_label_create(ui->screen_4_btn_1);
    lv_label_set_text(ui->screen_4_btn_1_label, "");
    lv_label_set_long_mode(ui->screen_4_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_4_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_4_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_4_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_4_btn_1, 14, 4);
    lv_obj_set_size(ui->screen_4_btn_1, 246, 101);

    //Write style for screen_4_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_4_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_4_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_4_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_4_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_4_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_4_btn_1, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_4_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_4_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_4.




    //Update current screen layout.
    lv_obj_update_layout(ui->screen_4);

    //Init events for screen.
    events_init_screen_4(ui);
}
