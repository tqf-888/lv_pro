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
#include "favorite_song_page_demo.h"


void setup_scr_screen_14(lv_ui *ui)
{
    //Write codes screen_14
    ui->screen_14 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_14, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_14, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_img_14
    ui->screen_14_img_14 = lv_img_create(ui->screen_14);
    lv_obj_add_flag(ui->screen_14_img_14, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_14_img_14, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\dgvsdgbsdgb.png");
#else
    lv_img_set_src(ui->screen_14_img_14, "S:/usr/share/lv_projector/dgvsdgbsdgb.png");
#endif
    lv_img_set_pivot(ui->screen_14_img_14, 50,50);
    lv_img_set_angle(ui->screen_14_img_14, 0);
    lv_obj_set_pos(ui->screen_14_img_14, 0, 0);
    lv_obj_set_size(ui->screen_14_img_14, 1280, 800);

    //Write style for screen_14_img_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_14_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_14_img_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_14_img_14, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_cont_2
    ui->screen_14_cont_2 = lv_obj_create(ui->screen_14);
    lv_obj_set_pos(ui->screen_14_cont_2, 0, 0);
    lv_obj_set_size(ui->screen_14_cont_2, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_14_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_14_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_14_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_img_12
    ui->screen_14_img_12 = lv_img_create(ui->screen_14_cont_2);
    lv_obj_add_flag(ui->screen_14_img_12, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_14_img_12, &_sdgvdsg_alpha_223x48);
    lv_img_set_pivot(ui->screen_14_img_12, 50,50);
    lv_img_set_angle(ui->screen_14_img_12, 0);
    lv_obj_set_pos(ui->screen_14_img_12, 28, 25);
    lv_obj_set_size(ui->screen_14_img_12, 223, 48);

    //Write style for screen_14_img_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_14_img_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_14_img_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_img_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_14_img_12, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_cont_4
    ui->screen_14_cont_4 = lv_obj_create(ui->screen_14_cont_2);
    lv_obj_set_pos(ui->screen_14_cont_4, 228, 217);
    lv_obj_set_size(ui->screen_14_cont_4, 852, 458);
    lv_obj_set_scrollbar_mode(ui->screen_14_cont_4, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_14_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_14_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_btn_11
    ui->screen_14_btn_11 = lv_btn_create(ui->screen_14_cont_2);
    ui->screen_14_btn_11_label = lv_label_create(ui->screen_14_btn_11);
    lv_label_set_text(ui->screen_14_btn_11_label, "");
    lv_label_set_long_mode(ui->screen_14_btn_11_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_14_btn_11_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_14_btn_11, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_14_btn_11_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_14_btn_11, 8, 2);
    lv_obj_set_size(ui->screen_14_btn_11, 267, 99);

    //Write style for screen_14_btn_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_14_btn_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_14_btn_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_btn_11, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_14_btn_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_14_btn_11, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_14_btn_11, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_14_btn_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_14_btn_11, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_cont_3
    ui->screen_14_cont_3 = lv_obj_create(ui->screen_14_cont_2);
    lv_obj_set_pos(ui->screen_14_cont_3, 8, 690);
    lv_obj_set_size(ui->screen_14_cont_3, 232, 103);
    lv_obj_set_scrollbar_mode(ui->screen_14_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_14_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_14_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_img_10
    ui->screen_14_img_10 = lv_img_create(ui->screen_14_cont_2);
    lv_obj_add_flag(ui->screen_14_img_10, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_14_img_10, &_gvsdrbhgdh_alpha_147x48);
    lv_img_set_pivot(ui->screen_14_img_10, 50,50);
    lv_img_set_angle(ui->screen_14_img_10, 0);
    lv_obj_set_pos(ui->screen_14_img_10, 1127, 113);
    lv_obj_set_size(ui->screen_14_img_10, 147, 48);

    //Write style for screen_14_img_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_14_img_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_14_img_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_img_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_14_img_10, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_btn_10
    ui->screen_14_btn_10 = lv_btn_create(ui->screen_14_cont_2);
    ui->screen_14_btn_10_label = lv_label_create(ui->screen_14_btn_10);
    lv_label_set_text(ui->screen_14_btn_10_label, "");
    lv_label_set_long_mode(ui->screen_14_btn_10_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_14_btn_10_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_14_btn_10, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_14_btn_10_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_14_btn_10, 1080, 108);
    lv_obj_set_size(ui->screen_14_btn_10, 196, 91);

    //Write style for screen_14_btn_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_14_btn_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_14_btn_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_btn_10, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_14_btn_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_14_btn_10, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_14_btn_10, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_14_btn_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_14_btn_10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_14_img_13
    ui->screen_14_img_13 = lv_img_create(ui->screen_14_cont_2);
    lv_obj_add_flag(ui->screen_14_img_13, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_14_img_13, &_sgderhbrdsdfgs_alpha_82x82);
    lv_img_set_pivot(ui->screen_14_img_13, 50,50);
    lv_img_set_angle(ui->screen_14_img_13, 0);
    lv_obj_set_pos(ui->screen_14_img_13, 1176, 12);
    lv_obj_set_size(ui->screen_14_img_13, 82, 82);

    //Write style for screen_14_img_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_14_img_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_14_img_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_14_img_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_14_img_13, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_14.



    //Update current screen layout.
    lv_obj_update_layout(ui->screen_14);

    //Init events for screen.
    events_init_screen_14(ui);
}
