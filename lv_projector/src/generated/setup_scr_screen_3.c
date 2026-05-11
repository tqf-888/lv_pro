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


void setup_scr_screen_3(lv_ui *ui)
{
    //Write codes screen_3
    ui->screen_3 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_3, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_3, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui->screen_3, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_img_15
    ui->screen_3_img_15 = lv_img_create(ui->screen_3);
    lv_obj_add_flag(ui->screen_3_img_15, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_3_img_15, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\bhdrehrtfdh.png");
#else
    lv_img_set_src(ui->screen_3_img_15, "S:/usr/share/lv_projector/bhdrehrtfdh.png");
#endif
    lv_img_set_pivot(ui->screen_3_img_15, 50,50);
    lv_img_set_angle(ui->screen_3_img_15, 0);
    lv_obj_set_pos(ui->screen_3_img_15, 0, 0);
    lv_obj_set_size(ui->screen_3_img_15, 1280, 800);

    //Write style for screen_3_img_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_3_img_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_3_img_15, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_img_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_3_img_15, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_cont_4
    ui->screen_3_cont_4 = lv_obj_create(ui->screen_3);
    lv_obj_set_pos(ui->screen_3_cont_4, 0, 0);
    lv_obj_set_size(ui->screen_3_cont_4, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_3_cont_4, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_3_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_cont_2
    ui->screen_3_cont_2 = lv_obj_create(ui->screen_3_cont_4);
    lv_obj_set_pos(ui->screen_3_cont_2, 12, 114);
    lv_obj_set_size(ui->screen_3_cont_2, 400, 181);
    lv_obj_set_scrollbar_mode(ui->screen_3_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_3_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_3_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_cont_2, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_3_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_3_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_3_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_3_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_3_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_btn_4
    ui->screen_3_btn_4 = lv_btn_create(ui->screen_3_cont_2);
    ui->screen_3_btn_4_label = lv_label_create(ui->screen_3_btn_4);
    lv_label_set_text(ui->screen_3_btn_4_label, "");
    lv_label_set_long_mode(ui->screen_3_btn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_3_btn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_3_btn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_3_btn_4_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_3_btn_4, 204, 109);
    lv_obj_set_size(ui->screen_3_btn_4, 161, 50);

    //Write style for screen_3_btn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_3_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_3_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_3_btn_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_3_btn_4, &lv_font_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_3_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_3_btn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_btn_3
    ui->screen_3_btn_3 = lv_btn_create(ui->screen_3_cont_2);
    ui->screen_3_btn_3_label = lv_label_create(ui->screen_3_btn_3);
    lv_label_set_text(ui->screen_3_btn_3_label, "");
    lv_label_set_long_mode(ui->screen_3_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_3_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_3_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_3_btn_3_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_3_btn_3, 28, 110);
    lv_obj_set_size(ui->screen_3_btn_3, 161, 50);

    //Write style for screen_3_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_3_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_3_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_3_btn_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_3_btn_3, &lv_font_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_3_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_3_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_ta_1
    ui->screen_3_ta_1 = lv_textarea_create(ui->screen_3_cont_2);
    lv_textarea_set_text(ui->screen_3_ta_1, "");
    lv_textarea_set_placeholder_text(ui->screen_3_ta_1, "请点选歌名首字母");
    lv_textarea_set_password_bullet(ui->screen_3_ta_1, "*");
    lv_textarea_set_password_mode(ui->screen_3_ta_1, false);
    lv_textarea_set_one_line(ui->screen_3_ta_1, false);
    lv_textarea_set_accepted_chars(ui->screen_3_ta_1, "");
    lv_textarea_set_max_length(ui->screen_3_ta_1, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_3_ta_1, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->screen_3_ta_1, 27, 33);
    lv_obj_set_size(ui->screen_3_ta_1, 349, 62);

    //Write style for screen_3_ta_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_3_ta_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_3_ta_1, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_3_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_3_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_3_ta_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_3_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_3_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_3_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_3_ta_1, lv_color_hex(0xb6a91e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_3_ta_1, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_ta_1, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_3_ta_1, lv_color_hex(0xb0855e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_3_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_3_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_3_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_3_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_3_ta_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_3_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_3_ta_1, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_ta_1, 17, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_3_ta_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_3_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_3_btn_6
    ui->screen_3_btn_6 = lv_btn_create(ui->screen_3_cont_4);
    ui->screen_3_btn_6_label = lv_label_create(ui->screen_3_btn_6);
    lv_label_set_text(ui->screen_3_btn_6_label, "\n");
    lv_label_set_long_mode(ui->screen_3_btn_6_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_3_btn_6_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_3_btn_6, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_3_btn_6_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_3_btn_6, 9, 9);
    lv_obj_set_size(ui->screen_3_btn_6, 290, 91);

    //Write style for screen_3_btn_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_3_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_3_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_btn_6, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_3_btn_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_3_btn_6, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_3_btn_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_3_btn_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_cont_3
    ui->screen_3_cont_3 = lv_obj_create(ui->screen_3_cont_4);
    lv_obj_set_pos(ui->screen_3_cont_3, 431, 127);
    lv_obj_set_size(ui->screen_3_cont_3, 807, 631);
    lv_obj_set_scrollbar_mode(ui->screen_3_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_3_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_line_1
    ui->screen_3_line_1 = lv_line_create(ui->screen_3_cont_4);
    static lv_point_t screen_3_line_1[] = {{0, 0},{1280, 0},};
    lv_line_set_points(ui->screen_3_line_1, screen_3_line_1, 2);
    lv_obj_set_pos(ui->screen_3_line_1, 0, 100);
    lv_obj_set_size(ui->screen_3_line_1, 1280, 5);

    //Write style for screen_3_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_3_line_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_3_line_1, lv_color_hex(0x757575), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_3_line_1, 102, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_3_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_img_14
    ui->screen_3_img_14 = lv_img_create(ui->screen_3_cont_4);
    lv_obj_add_flag(ui->screen_3_img_14, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_3_img_14, &_sgderhbrdsdfgs_alpha_82x82);
    lv_img_set_pivot(ui->screen_3_img_14, 50,50);
    lv_img_set_angle(ui->screen_3_img_14, 0);
    lv_obj_set_pos(ui->screen_3_img_14, 1160, 18);
    lv_obj_set_size(ui->screen_3_img_14, 82, 82);

    //Write style for screen_3_img_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_3_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_3_img_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_3_img_14, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_3_label_2
    ui->screen_3_label_2 = lv_label_create(ui->screen_3_cont_4);
    lv_label_set_text(ui->screen_3_label_2, "0");
    lv_label_set_long_mode(ui->screen_3_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_3_label_2, 948, 56);
    lv_obj_set_size(ui->screen_3_label_2, 63, 37);

    //Write style for screen_3_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_3_label_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_3_label_2, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_3_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_3_label_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_3_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_3.



// lv_textarea_set_text(guider_ui.screen_3_ta_1, "");

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_3);

    //Init events for screen.
    events_init_screen_3(ui);
}
