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



void setup_scr_screen_log_in(lv_ui *ui)
{
    //Write codes screen_log_in
    ui->screen_log_in = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_log_in, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_log_in, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_log_in, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_log_in, lv_color_hex(0x454055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_log_in, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_cont_1
    ui->screen_log_in_cont_1 = lv_obj_create(ui->screen_log_in);
    lv_obj_set_pos(ui->screen_log_in_cont_1, 225, 105);
    lv_obj_set_size(ui->screen_log_in_cont_1, 822, 564);
    lv_obj_set_scrollbar_mode(ui->screen_log_in_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_log_in_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_ta_1
    ui->screen_log_in_ta_1 = lv_textarea_create(ui->screen_log_in_cont_1);
    lv_textarea_set_text(ui->screen_log_in_ta_1, "16666666666");
    lv_textarea_set_placeholder_text(ui->screen_log_in_ta_1, "");
    lv_textarea_set_password_bullet(ui->screen_log_in_ta_1, "*");
    lv_textarea_set_password_mode(ui->screen_log_in_ta_1, false);
    lv_textarea_set_one_line(ui->screen_log_in_ta_1, false);
    lv_textarea_set_accepted_chars(ui->screen_log_in_ta_1, "");
    lv_textarea_set_max_length(ui->screen_log_in_ta_1, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_log_in_ta_1, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->screen_log_in_ta_1, 25, 33);
    lv_obj_set_size(ui->screen_log_in_ta_1, 400, 70);

    //Write style for screen_log_in_ta_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_log_in_ta_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_ta_1, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_log_in_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_ta_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_log_in_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_log_in_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_log_in_ta_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_log_in_ta_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_log_in_ta_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_log_in_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_log_in_ta_1, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_ta_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_log_in_ta_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_log_in_btn_1
    ui->screen_log_in_btn_1 = lv_btn_create(ui->screen_log_in_cont_1);
    ui->screen_log_in_btn_1_label = lv_label_create(ui->screen_log_in_btn_1);
    lv_label_set_text(ui->screen_log_in_btn_1_label, "发送验证码");
    lv_label_set_long_mode(ui->screen_log_in_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_log_in_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_log_in_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_log_in_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_log_in_btn_1, 480, 32);
    lv_obj_set_size(ui->screen_log_in_btn_1, 200, 70);

    //Write style for screen_log_in_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_log_in_btn_1, lv_color_hex(0xa02020), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_log_in_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_btn_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_log_in_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_log_in_btn_1, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_log_in_btn_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_btn_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_log_in_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_btn_1, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_cont_2
    ui->screen_log_in_cont_2 = lv_obj_create(ui->screen_log_in);
    lv_obj_set_pos(ui->screen_log_in_cont_2, 225, 105);
    lv_obj_set_size(ui->screen_log_in_cont_2, 822, 564);
    lv_obj_set_scrollbar_mode(ui->screen_log_in_cont_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_log_in_cont_2, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_log_in_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_btn_2
    ui->screen_log_in_btn_2 = lv_btn_create(ui->screen_log_in_cont_2);
    ui->screen_log_in_btn_2_label = lv_label_create(ui->screen_log_in_btn_2);
    lv_label_set_text(ui->screen_log_in_btn_2_label, "36s后重新获取");
    lv_label_set_long_mode(ui->screen_log_in_btn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_log_in_btn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_log_in_btn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_log_in_btn_2_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_log_in_btn_2, 480, 32);
    lv_obj_set_size(ui->screen_log_in_btn_2, 200, 70);

    //Write style for screen_log_in_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_log_in_btn_2, lv_color_hex(0xa02020), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_log_in_btn_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_btn_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_log_in_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_log_in_btn_2, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_log_in_btn_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_btn_2, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_log_in_btn_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_btn_2, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_btn_3
    ui->screen_log_in_btn_3 = lv_btn_create(ui->screen_log_in_cont_2);
    ui->screen_log_in_btn_3_label = lv_label_create(ui->screen_log_in_btn_3);
    lv_label_set_text(ui->screen_log_in_btn_3_label, "登录");
    lv_label_set_long_mode(ui->screen_log_in_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_log_in_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_log_in_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_log_in_btn_3_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_log_in_btn_3, 490, 259);
    lv_obj_set_size(ui->screen_log_in_btn_3, 200, 70);

    //Write style for screen_log_in_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_log_in_btn_3, lv_color_hex(0xa02020), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_log_in_btn_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_btn_3, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_log_in_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_log_in_btn_3, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_log_in_btn_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_btn_3, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_log_in_btn_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_btn_3, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_ta_2
    ui->screen_log_in_ta_2 = lv_textarea_create(ui->screen_log_in_cont_2);
    lv_textarea_set_text(ui->screen_log_in_ta_2, "");
    lv_textarea_set_placeholder_text(ui->screen_log_in_ta_2, "请输入验证码");
    lv_textarea_set_password_bullet(ui->screen_log_in_ta_2, "*");
    lv_textarea_set_password_mode(ui->screen_log_in_ta_2, false);
    lv_textarea_set_one_line(ui->screen_log_in_ta_2, false);
    lv_textarea_set_accepted_chars(ui->screen_log_in_ta_2, "");
    lv_textarea_set_max_length(ui->screen_log_in_ta_2, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_log_in_ta_2, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->screen_log_in_ta_2, 25, 33);
    lv_obj_set_size(ui->screen_log_in_ta_2, 400, 70);

    //Write style for screen_log_in_ta_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_log_in_ta_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_ta_2, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_ta_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_log_in_ta_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_ta_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_log_in_ta_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_ta_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_log_in_ta_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_log_in_ta_2, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_log_in_ta_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_ta_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_log_in_ta_2, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_log_in_ta_2, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_log_in_ta_2, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_ta_2, 10, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_log_in_ta_2, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_ta_2, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_ta_2, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_log_in_btn_4
    ui->screen_log_in_btn_4 = lv_btn_create(ui->screen_log_in);
    ui->screen_log_in_btn_4_label = lv_label_create(ui->screen_log_in_btn_4);
    lv_label_set_text(ui->screen_log_in_btn_4_label, "X  删除");
    lv_label_set_long_mode(ui->screen_log_in_btn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_log_in_btn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_log_in_btn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_log_in_btn_4_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_log_in_btn_4, 279, 635);
    lv_obj_set_size(ui->screen_log_in_btn_4, 161, 50);

    //Write style for screen_log_in_btn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_log_in_btn_4, lv_color_hex(0xa02020), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_log_in_btn_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_btn_4, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_log_in_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_log_in_btn_4, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_log_in_btn_4, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_btn_4, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_log_in_btn_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_btn_4, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_btn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_btn_5
    ui->screen_log_in_btn_5 = lv_btn_create(ui->screen_log_in);
    ui->screen_log_in_btn_5_label = lv_label_create(ui->screen_log_in_btn_5);
    lv_label_set_text(ui->screen_log_in_btn_5_label, "⬅️ 清空");
    lv_label_set_long_mode(ui->screen_log_in_btn_5_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_log_in_btn_5_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_log_in_btn_5, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_log_in_btn_5_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_log_in_btn_5, 469, 635);
    lv_obj_set_size(ui->screen_log_in_btn_5, 161, 50);

    //Write style for screen_log_in_btn_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_btn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_log_in_btn_5, lv_color_hex(0xa02020), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_log_in_btn_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_btn_5, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_log_in_btn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_log_in_btn_5, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_log_in_btn_5, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_btn_5, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_btn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_log_in_btn_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_btn_5, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_btn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_btn_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_btn_6
    ui->screen_log_in_btn_6 = lv_btn_create(ui->screen_log_in);
    ui->screen_log_in_btn_6_label = lv_label_create(ui->screen_log_in_btn_6);
    lv_label_set_text(ui->screen_log_in_btn_6_label, "退出");
    lv_label_set_long_mode(ui->screen_log_in_btn_6_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_log_in_btn_6_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_log_in_btn_6, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_log_in_btn_6_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_log_in_btn_6, 2, 4);
    lv_obj_set_size(ui->screen_log_in_btn_6, 179, 101);

    //Write style for screen_log_in_btn_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_btn_6, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_log_in_btn_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_btn_6, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_btn_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_btn_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_log_in_btn_7
    ui->screen_log_in_btn_7 = lv_btn_create(ui->screen_log_in);
    ui->screen_log_in_btn_7_label = lv_label_create(ui->screen_log_in_btn_7);
    lv_label_set_text(ui->screen_log_in_btn_7_label, "设置");
    lv_label_set_long_mode(ui->screen_log_in_btn_7_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_log_in_btn_7_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_log_in_btn_7, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_log_in_btn_7_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_log_in_btn_7, 1115, 14);
    lv_obj_set_size(ui->screen_log_in_btn_7, 148, 76);

    //Write style for screen_log_in_btn_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_log_in_btn_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_log_in_btn_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_log_in_btn_7, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_log_in_btn_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_log_in_btn_7, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_log_in_btn_7, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_log_in_btn_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_log_in_btn_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_log_in.


    lv_obj_set_style_bg_color(guider_ui.g_kb_top_layer, lv_color_hex(0x161619), LV_PART_MAIN);
    /* 2. 按键背景（正常状态） */
    lv_obj_set_style_bg_color  (guider_ui.g_kb_top_layer, lv_color_hex(0x202125), LV_PART_ITEMS);
    lv_obj_set_style_radius    (guider_ui.g_kb_top_layer, 5,                      LV_PART_ITEMS); /* 圆角 */
    lv_obj_set_style_text_color(guider_ui.g_kb_top_layer, lv_color_hex(0xE4E4E4), LV_PART_ITEMS);
    lv_obj_set_style_bg_color  (guider_ui.g_kb_top_layer, lv_color_hex(0x202125), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(guider_ui.g_kb_top_layer, lv_color_hex(0xE4E4E4), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_radius    (guider_ui.g_kb_top_layer, 5,                      LV_PART_ITEMS | LV_STATE_CHECKED); /* 圆角 */
    /* 4. 按键按下时的背景/文字 */
    lv_obj_set_style_bg_color(guider_ui.g_kb_top_layer, lv_color_hex(0x1C1D1E), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(guider_ui.g_kb_top_layer, lv_color_hex(0xffffff), LV_PART_ITEMS | LV_STATE_PRESSED);


    lv_obj_set_align(guider_ui.g_kb_top_layer, LV_ALIGN_DEFAULT);
    lv_obj_set_size(guider_ui.g_kb_top_layer, 380, 380);
    lv_obj_set_pos(guider_ui.g_kb_top_layer, 45, 350);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_log_in);

    //Init events for screen.
    events_init_screen_log_in(ui);
}
