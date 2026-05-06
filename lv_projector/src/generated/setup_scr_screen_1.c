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
#include "top100_song_page_demo.h"


void setup_scr_screen_1(lv_ui *ui)
{
    //Write codes screen_1
    ui->screen_1 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_1, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1, lv_color_hex(0x111520), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_img_25
    ui->screen_1_img_25 = lv_img_create(ui->screen_1);
    lv_obj_add_flag(ui->screen_1_img_25, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_1_img_25, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\shgbsgbds.png");
#else
    lv_img_set_src(ui->screen_1_img_25, "S:/usr/share/lv_projector/shgbsgbds.png");
#endif
    lv_img_set_pivot(ui->screen_1_img_25, 50,50);
    lv_img_set_angle(ui->screen_1_img_25, 0);
    lv_obj_set_pos(ui->screen_1_img_25, 0, 0);
    lv_obj_set_size(ui->screen_1_img_25, 1280, 800);

    //Write style for screen_1_img_25, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_1_img_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_1_img_25, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_img_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_1_img_25, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_cont_3
    ui->screen_1_cont_3 = lv_obj_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_cont_3, 0, -1);
    lv_obj_set_size(ui->screen_1_cont_3, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_1_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_1_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_btn_20
    ui->screen_1_btn_20 = lv_btn_create(ui->screen_1_cont_3);
    ui->screen_1_btn_20_label = lv_label_create(ui->screen_1_btn_20);
    lv_label_set_text(ui->screen_1_btn_20_label, "");
    lv_label_set_long_mode(ui->screen_1_btn_20_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_btn_20_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_btn_20, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_btn_20_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_btn_20, 10, 7);
    lv_obj_set_size(ui->screen_1_btn_20, 267, 99);

    //Write style for screen_1_btn_20, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_btn_20, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_btn_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_btn_20, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_btn_20, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_btn_20, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_btn_20, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_line_2
    ui->screen_1_line_2 = lv_line_create(ui->screen_1_cont_3);
    static lv_point_t screen_1_line_2[] = {{0, 0},{1280, 0},};
    lv_line_set_points(ui->screen_1_line_2, screen_1_line_2, 2);
    lv_obj_set_pos(ui->screen_1_line_2, 0, 100);
    lv_obj_set_size(ui->screen_1_line_2, 1280, 5);

    //Write style for screen_1_line_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_1_line_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_1_line_2, lv_color_hex(0x757575), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_1_line_2, 102, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_1_line_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_btn_25
    ui->screen_1_btn_25 = lv_btn_create(ui->screen_1_cont_3);
    ui->screen_1_btn_25_label = lv_label_create(ui->screen_1_btn_25);
    lv_label_set_text(ui->screen_1_btn_25_label, "");
    lv_label_set_long_mode(ui->screen_1_btn_25_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_btn_25_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_btn_25, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_btn_25_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_btn_25, 40, 575);
    lv_obj_set_size(ui->screen_1_btn_25, 238, 192);

    //Write style for screen_1_btn_25, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_btn_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_btn_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_btn_25, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_btn_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_btn_25, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_btn_25, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_btn_25, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_btn_25, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_btn_24
    ui->screen_1_btn_24 = lv_btn_create(ui->screen_1_cont_3);
    ui->screen_1_btn_24_label = lv_label_create(ui->screen_1_btn_24);
    lv_label_set_text(ui->screen_1_btn_24_label, "");
    lv_label_set_long_mode(ui->screen_1_btn_24_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_btn_24_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_btn_24, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_btn_24_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_btn_24, 42, 360);
    lv_obj_set_size(ui->screen_1_btn_24, 238, 192);

    //Write style for screen_1_btn_24, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_btn_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_btn_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_btn_24, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_btn_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_btn_24, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_btn_24, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_btn_24, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_btn_24, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_btn_23
    ui->screen_1_btn_23 = lv_btn_create(ui->screen_1_cont_3);
    ui->screen_1_btn_23_label = lv_label_create(ui->screen_1_btn_23);
    lv_label_set_text(ui->screen_1_btn_23_label, "");
    lv_label_set_long_mode(ui->screen_1_btn_23_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_btn_23_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_btn_23, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_btn_23_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_btn_23, 40, 150);
    lv_obj_set_size(ui->screen_1_btn_23, 238, 192);

    //Write style for screen_1_btn_23, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_btn_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_btn_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_btn_23, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_btn_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_btn_23, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_btn_23, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_btn_23, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_btn_23, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_cont_10
    ui->screen_1_cont_10 = lv_obj_create(ui->screen_1_cont_3);
    lv_obj_set_pos(ui->screen_1_cont_10, 287, 128);
    lv_obj_set_size(ui->screen_1_cont_10, 1023, 669);
    lv_obj_set_scrollbar_mode(ui->screen_1_cont_10, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_1_cont_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_cont_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_cont_11
    ui->screen_1_cont_11 = lv_obj_create(ui->screen_1_cont_3);
    lv_obj_set_pos(ui->screen_1_cont_11, 287, 128);
    lv_obj_set_size(ui->screen_1_cont_11, 1016, 674);
    lv_obj_set_scrollbar_mode(ui->screen_1_cont_11, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_1_cont_11, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_1_cont_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_cont_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_img_26
    ui->screen_1_img_26 = lv_img_create(ui->screen_1_cont_3);
    lv_obj_add_flag(ui->screen_1_img_26, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_1_img_26, &_sgderhbrdsdfgs_alpha_82x82);
    lv_img_set_pivot(ui->screen_1_img_26, 50,50);
    lv_img_set_angle(ui->screen_1_img_26, 0);
    lv_obj_set_pos(ui->screen_1_img_26, 1176, 12);
    lv_obj_set_size(ui->screen_1_img_26, 82, 82);

    //Write style for screen_1_img_26, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_1_img_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_1_img_26, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_img_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_1_img_26, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_1.
    lv_obj_add_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);

page_set(6);//不可以删



    //Update current screen layout.
    lv_obj_update_layout(ui->screen_1);

    //Init events for screen.
    events_init_screen_1(ui);
}
