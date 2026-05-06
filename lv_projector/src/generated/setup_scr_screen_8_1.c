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
#include "rich_song_page_demo.h"


void setup_scr_screen_8_1(lv_ui *ui)
{
    //Write codes screen_8_1
    ui->screen_8_1 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_8_1, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_8_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_8_1, LV_OBJ_FLAG_CLICKABLE);

    //Write style for screen_8_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_1_cont_5
    ui->screen_8_1_cont_5 = lv_obj_create(ui->screen_8_1);
    lv_obj_set_pos(ui->screen_8_1_cont_5, 2, 2);
    lv_obj_set_size(ui->screen_8_1_cont_5, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_8_1_cont_5, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_8_1_cont_5, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_8_1_cont_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_1_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_1_cont_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_1_cont_5, lv_color_hex(0x221936), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_1_cont_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_1_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_1_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_1_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_1_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_1_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_1_img_1
    ui->screen_8_1_img_1 = lv_img_create(ui->screen_8_1_cont_5);
    lv_obj_add_flag(ui->screen_8_1_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_8_1_img_1, &_speake1_1280x94);
    lv_img_set_pivot(ui->screen_8_1_img_1, 50,50);
    lv_img_set_angle(ui->screen_8_1_img_1, 0);
    lv_obj_set_pos(ui->screen_8_1_img_1, 0, 0);
    lv_obj_set_size(ui->screen_8_1_img_1, 1280, 94);

    //Write style for screen_8_1_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_8_1_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_8_1_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_8_1_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_1_cont_1
    ui->screen_8_1_cont_1 = lv_obj_create(ui->screen_8_1_cont_5);
    lv_obj_set_pos(ui->screen_8_1_cont_1, 23, 129);
    lv_obj_set_size(ui->screen_8_1_cont_1, 400, 551);
    lv_obj_set_scrollbar_mode(ui->screen_8_1_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_1_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_1_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_cont_1, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_1_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_1_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_1_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_1_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_1_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_1_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_1_ta_2
    ui->screen_8_1_ta_2 = lv_textarea_create(ui->screen_8_1_cont_1);
    lv_textarea_set_text(ui->screen_8_1_ta_2, "");
    lv_textarea_set_placeholder_text(ui->screen_8_1_ta_2, "请点选歌名首字母");
    lv_textarea_set_password_bullet(ui->screen_8_1_ta_2, "*");
    lv_textarea_set_password_mode(ui->screen_8_1_ta_2, false);
    lv_textarea_set_one_line(ui->screen_8_1_ta_2, false);
    lv_textarea_set_accepted_chars(ui->screen_8_1_ta_2, "");
    lv_textarea_set_max_length(ui->screen_8_1_ta_2, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_8_1_ta_2, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->screen_8_1_ta_2, 26, 22);
    lv_obj_set_size(ui->screen_8_1_ta_2, 349, 62);

    //Write style for screen_8_1_ta_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_8_1_ta_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_1_ta_2, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_1_ta_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_8_1_ta_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_1_ta_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_1_ta_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_1_ta_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_8_1_ta_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_8_1_ta_2, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_8_1_ta_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_1_ta_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_1_ta_2, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_1_ta_2, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_1_ta_2, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_ta_2, 17, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_8_1_ta_2, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_1_ta_2, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_ta_2, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_8_1_btn_10
    ui->screen_8_1_btn_10 = lv_btn_create(ui->screen_8_1_cont_1);
    ui->screen_8_1_btn_10_label = lv_label_create(ui->screen_8_1_btn_10);
    lv_label_set_text(ui->screen_8_1_btn_10_label, "清空");
    lv_label_set_long_mode(ui->screen_8_1_btn_10_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_1_btn_10_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_1_btn_10, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_1_btn_10_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_1_btn_10, 210, 112);
    lv_obj_set_size(ui->screen_8_1_btn_10, 161, 50);

    //Write style for screen_8_1_btn_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_1_btn_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_1_btn_10, lv_color_hex(0x1C1D1E), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_1_btn_10, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_1_btn_10, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_8_1_btn_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_8_1_btn_10, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_8_1_btn_10, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_btn_10, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_1_btn_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_1_btn_10, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_1_btn_10, &lv_font_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_1_btn_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_1_btn_10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_1_btn_9
    ui->screen_8_1_btn_9 = lv_btn_create(ui->screen_8_1_cont_1);
    ui->screen_8_1_btn_9_label = lv_label_create(ui->screen_8_1_btn_9);
    lv_label_set_text(ui->screen_8_1_btn_9_label, "X  删除");
    lv_label_set_long_mode(ui->screen_8_1_btn_9_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_1_btn_9_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_1_btn_9, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_1_btn_9_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_1_btn_9, 25, 114);
    lv_obj_set_size(ui->screen_8_1_btn_9, 161, 50);

    //Write style for screen_8_1_btn_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_1_btn_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_8_1_btn_9, lv_color_hex(0x1C1D1E), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_8_1_btn_9, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_1_btn_9, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_8_1_btn_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_8_1_btn_9, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_8_1_btn_9, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_btn_9, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_1_btn_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_1_btn_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_1_btn_9, &lv_font_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_1_btn_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_1_btn_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_1_btn_6
    ui->screen_8_1_btn_6 = lv_btn_create(ui->screen_8_1_cont_5);
    ui->screen_8_1_btn_6_label = lv_label_create(ui->screen_8_1_btn_6);
    lv_label_set_text(ui->screen_8_1_btn_6_label, "\n");
    lv_label_set_long_mode(ui->screen_8_1_btn_6_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_8_1_btn_6_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_8_1_btn_6, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_8_1_btn_6_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_8_1_btn_6, 6, 2);
    lv_obj_set_size(ui->screen_8_1_btn_6, 246, 106);

    //Write style for screen_8_1_btn_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_8_1_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_8_1_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_btn_6, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_1_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_8_1_btn_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_8_1_btn_6, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_8_1_btn_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_8_1_btn_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_8_1_cont_2
    ui->screen_8_1_cont_2 = lv_obj_create(ui->screen_8_1_cont_5);
    lv_obj_set_pos(ui->screen_8_1_cont_2, 434, 123);
    lv_obj_set_size(ui->screen_8_1_cont_2, 893, 677);
    lv_obj_set_scrollbar_mode(ui->screen_8_1_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_8_1_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_8_1_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_8_1.


//page_manager_init();

    lv_obj_set_style_bg_color(ui->g_kb_top_layer, lv_color_hex(0x161619), LV_PART_MAIN);
    /* 2. 按键背景（正常状态） */
    lv_obj_set_style_bg_color  (ui->g_kb_top_layer, lv_color_hex(0x202125), LV_PART_ITEMS);
    lv_obj_set_style_radius    (ui->g_kb_top_layer, 5,                      LV_PART_ITEMS); /* 圆角 */
    lv_obj_set_style_text_color(ui->g_kb_top_layer, lv_color_hex(0xE4E4E4), LV_PART_ITEMS);
    lv_obj_set_style_bg_color  (ui->g_kb_top_layer, lv_color_hex(0x202125), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->g_kb_top_layer, lv_color_hex(0xE4E4E4), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_radius    (ui->g_kb_top_layer, 5,                      LV_PART_ITEMS | LV_STATE_CHECKED); /* 圆角 */
    /* 4. 按键按下时的背景/文字 */
    lv_obj_set_style_bg_color(ui->g_kb_top_layer, lv_color_hex(0x1C1D1E), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->g_kb_top_layer, lv_color_hex(0xffffff), LV_PART_ITEMS | LV_STATE_PRESSED);


    lv_obj_set_align(ui->g_kb_top_layer, LV_ALIGN_DEFAULT);
    lv_obj_set_size(ui->g_kb_top_layer, 380, 380);
    lv_obj_set_pos(ui->g_kb_top_layer, 45, 350);




    //Update current screen layout.
    lv_obj_update_layout(ui->screen_8_1);

    //Init events for screen.
    events_init_screen_8_1(ui);
}
