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


void setup_scr_screen_9(lv_ui *ui)
{
    //Write codes screen_9
    ui->screen_9 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_9, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_9, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_9, lv_color_hex(0x170d2a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_9, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_img_1
    ui->screen_9_img_1 = lv_img_create(ui->screen_9);
    lv_obj_add_flag(ui->screen_9_img_1, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_9_img_1, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\gesrhbrtsh.png");
#else
    lv_img_set_src(ui->screen_9_img_1, "S:/usr/share/lv_projector/gesrhbrtsh.png");
#endif
    lv_img_set_pivot(ui->screen_9_img_1, 50,50);
    lv_img_set_angle(ui->screen_9_img_1, 0);
    lv_obj_set_pos(ui->screen_9_img_1, 0, 0);
    lv_obj_set_size(ui->screen_9_img_1, 1280, 800);

    //Write style for screen_9_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_9_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_9_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_9_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_cont_2
    ui->screen_9_cont_2 = lv_obj_create(ui->screen_9);
    lv_obj_set_pos(ui->screen_9_cont_2, 0, 0);
    lv_obj_set_size(ui->screen_9_cont_2, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_9_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_9_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_9_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_cont_4
    ui->screen_9_cont_4 = lv_obj_create(ui->screen_9_cont_2);
    lv_obj_set_pos(ui->screen_9_cont_4, 412, 115);
    lv_obj_set_size(ui->screen_9_cont_4, 866, 672);
    lv_obj_set_scrollbar_mode(ui->screen_9_cont_4, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_9_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_9_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_btn_6
    ui->screen_9_btn_6 = lv_btn_create(ui->screen_9_cont_2);
    ui->screen_9_btn_6_label = lv_label_create(ui->screen_9_btn_6);
    lv_label_set_text(ui->screen_9_btn_6_label, "");
    lv_label_set_long_mode(ui->screen_9_btn_6_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_9_btn_6_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_9_btn_6, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_9_btn_6_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_9_btn_6, 45, 197);
    lv_obj_set_size(ui->screen_9_btn_6, 240, 563);

    //Write style for screen_9_btn_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_9_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_9_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_btn_6, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_9_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_9_btn_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_9_btn_6, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_9_btn_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_9_btn_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_btn_5
    ui->screen_9_btn_5 = lv_btn_create(ui->screen_9_cont_2);
    ui->screen_9_btn_5_label = lv_label_create(ui->screen_9_btn_5);
    lv_label_set_text(ui->screen_9_btn_5_label, "");
    lv_label_set_long_mode(ui->screen_9_btn_5_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_9_btn_5_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_9_btn_5, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_9_btn_5_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_9_btn_5, 7, 5);
    lv_obj_set_size(ui->screen_9_btn_5, 263, 122);

    //Write style for screen_9_btn_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_9_btn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_9_btn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_btn_5, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_9_btn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_9_btn_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_9_btn_5, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_9_btn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_9_btn_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_img_14
    ui->screen_9_img_14 = lv_img_create(ui->screen_9_cont_2);
    lv_obj_add_flag(ui->screen_9_img_14, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_9_img_14, &_sgderhbrdsdfgs_alpha_82x82);
    lv_img_set_pivot(ui->screen_9_img_14, 50,50);
    lv_img_set_angle(ui->screen_9_img_14, 0);
    lv_obj_set_pos(ui->screen_9_img_14, 1176, 12);
    lv_obj_set_size(ui->screen_9_img_14, 82, 82);

    //Write style for screen_9_img_14, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_9_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_9_img_14, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_img_14, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_9_img_14, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_9.
    page_manager_init();
subpage_set(1);

    lv_obj_add_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);//键盘




    //Update current screen layout.
    lv_obj_update_layout(ui->screen_9);

    //Init events for screen.
    events_init_screen_9(ui);
}
