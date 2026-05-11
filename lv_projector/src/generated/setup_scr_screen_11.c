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


void setup_scr_screen_11(lv_ui *ui)
{
    //Write codes screen_11
    ui->screen_11 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_11, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_11, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_11_img_16
    ui->screen_11_img_16 = lv_img_create(ui->screen_11);
    lv_obj_add_flag(ui->screen_11_img_16, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_11_img_16, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\jougvolubo.png");
#else
    lv_img_set_src(ui->screen_11_img_16, "S:/usr/share/lv_projector/jougvolubo.png");
#endif
    lv_img_set_pivot(ui->screen_11_img_16, 50,50);
    lv_img_set_angle(ui->screen_11_img_16, 0);
    lv_obj_set_pos(ui->screen_11_img_16, 0, 0);
    lv_obj_set_size(ui->screen_11_img_16, 1280, 800);

    //Write style for screen_11_img_16, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_11_img_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_11_img_16, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_11_img_16, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_11_img_16, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_11_cont_2
    ui->screen_11_cont_2 = lv_obj_create(ui->screen_11);
    lv_obj_set_pos(ui->screen_11_cont_2, 0, -1);
    lv_obj_set_size(ui->screen_11_cont_2, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_11_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_11_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_11_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_11_list_1
    ui->screen_11_list_1 = lv_list_create(ui->screen_11_cont_2);
    ui->screen_11_list_1_item0 = lv_list_add_btn(ui->screen_11_list_1, LV_SYMBOL_SAVE, "聚会点唱");
    ui->screen_11_list_1_item1 = lv_list_add_btn(ui->screen_11_list_1, LV_SYMBOL_SAVE, "save_1");
    lv_obj_set_pos(ui->screen_11_list_1, 86, 176);
    lv_obj_set_size(ui->screen_11_list_1, 216, 543);
    lv_obj_set_scrollbar_mode(ui->screen_11_list_1, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_screen_11_list_1_main_main_default
    static lv_style_t style_screen_11_list_1_main_main_default;
    ui_init_style(&style_screen_11_list_1_main_main_default);

    lv_style_set_pad_top(&style_screen_11_list_1_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_11_list_1_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_11_list_1_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_11_list_1_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_11_list_1_main_main_default, 0);
    lv_style_set_border_width(&style_screen_11_list_1_main_main_default, 0);
    lv_style_set_radius(&style_screen_11_list_1_main_main_default, 0);
    lv_style_set_shadow_width(&style_screen_11_list_1_main_main_default, 0);
    lv_obj_add_style(ui->screen_11_list_1, &style_screen_11_list_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_11_list_1_main_scrollbar_default
    static lv_style_t style_screen_11_list_1_main_scrollbar_default;
    ui_init_style(&style_screen_11_list_1_main_scrollbar_default);

    lv_style_set_radius(&style_screen_11_list_1_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_11_list_1_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_11_list_1_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_11_list_1_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_11_list_1, &style_screen_11_list_1_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_11_list_1_extra_btns_main_default
    static lv_style_t style_screen_11_list_1_extra_btns_main_default;
    ui_init_style(&style_screen_11_list_1_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_11_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_11_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_11_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_11_list_1_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_11_list_1_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_11_list_1_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_11_list_1_extra_btns_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_11_list_1_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_11_list_1_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_11_list_1_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_11_list_1_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_11_list_1_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_11_list_1_item1, &style_screen_11_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_11_list_1_item0, &style_screen_11_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_11_list_1_extra_texts_main_default
    static lv_style_t style_screen_11_list_1_extra_texts_main_default;
    ui_init_style(&style_screen_11_list_1_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_11_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_11_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_11_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_11_list_1_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_11_list_1_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_11_list_1_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_11_list_1_extra_texts_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_11_list_1_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_11_list_1_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_11_list_1_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_11_list_1_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_11_list_1_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_11_list_1_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_11_btn_17
    ui->screen_11_btn_17 = lv_btn_create(ui->screen_11_cont_2);
    ui->screen_11_btn_17_label = lv_label_create(ui->screen_11_btn_17);
    lv_label_set_text(ui->screen_11_btn_17_label, "");
    lv_label_set_long_mode(ui->screen_11_btn_17_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_11_btn_17_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_11_btn_17, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_11_btn_17_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_11_btn_17, 8, 11);
    lv_obj_set_size(ui->screen_11_btn_17, 296, 128);

    //Write style for screen_11_btn_17, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_11_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_11_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_11_btn_17, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_11_btn_17, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_11_btn_17, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_11_btn_17, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_11_btn_17, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_11_btn_17, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_11_cont_4
    ui->screen_11_cont_4 = lv_obj_create(ui->screen_11_cont_2);
    lv_obj_set_pos(ui->screen_11_cont_4, 372, 180);
    lv_obj_set_size(ui->screen_11_cont_4, 872, 545);
    lv_obj_set_scrollbar_mode(ui->screen_11_cont_4, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_11_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_11_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_11_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_11_cont_4, 72, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_11_cont_4, lv_color_hex(0x2a2641), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_11_cont_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_11_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_11_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_11_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_11_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_11_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_11_img_15
    ui->screen_11_img_15 = lv_img_create(ui->screen_11_cont_2);
    lv_obj_add_flag(ui->screen_11_img_15, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_11_img_15, &_sgderhbrdsdfgs_alpha_82x82);
    lv_img_set_pivot(ui->screen_11_img_15, 50,50);
    lv_img_set_angle(ui->screen_11_img_15, 0);
    lv_obj_set_pos(ui->screen_11_img_15, 1172, 13);
    lv_obj_set_size(ui->screen_11_img_15, 82, 82);

    //Write style for screen_11_img_15, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_11_img_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_11_img_15, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_11_img_15, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_11_img_15, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_11.





    //Update current screen layout.
    lv_obj_update_layout(ui->screen_11);

    //Init events for screen.
    events_init_screen_11(ui);
}
