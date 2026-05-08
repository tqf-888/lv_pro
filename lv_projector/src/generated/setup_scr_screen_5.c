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
#include "artist_page_demo.h"
#include "ktv.h"


void setup_scr_screen_5(lv_ui *ui)
{
    //Write codes screen_5
    ui->screen_5 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_5, 1280, 800);
    lv_obj_set_scrollbar_mode(ui->screen_5, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_4
    ui->screen_5_img_4 = lv_img_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_img_4, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_img_set_src(ui->screen_5_img_4, "C:\\NXP\\GUI-Guider-Projects\\KTV_1280_800\\import\\image\\sdpnvbpsd.png");
#else
    lv_img_set_src(ui->screen_5_img_4, "S:/usr/share/lv_projector/sdpnvbpsd.png");
#endif
    lv_img_set_pivot(ui->screen_5_img_4, 50,50);
    lv_img_set_angle(ui->screen_5_img_4, 0);
    lv_obj_set_pos(ui->screen_5_img_4, 1, 0);
    lv_obj_set_size(ui->screen_5_img_4, 1280, 800);

    //Write style for screen_5_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_btn_3
    ui->screen_5_btn_3 = lv_btn_create(ui->screen_5);
    ui->screen_5_btn_3_label = lv_label_create(ui->screen_5_btn_3);
    lv_label_set_text(ui->screen_5_btn_3_label, " 删除");
    lv_label_set_long_mode(ui->screen_5_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_5_btn_3_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_5_btn_3, 82, 305);
    lv_obj_set_size(ui->screen_5_btn_3, 161, 38);

    //Write style for screen_5_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5_btn_3, lv_color_hex(0x1C1D1E), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5_btn_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_5_btn_3, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_5_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_5_btn_3, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_5_btn_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_btn_3, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_btn_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_btn_3, &lv_font_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_btn_4
    ui->screen_5_btn_4 = lv_btn_create(ui->screen_5);
    ui->screen_5_btn_4_label = lv_label_create(ui->screen_5_btn_4);
    lv_label_set_text(ui->screen_5_btn_4_label, "清空");
    lv_label_set_long_mode(ui->screen_5_btn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_btn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_btn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_5_btn_4_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_5_btn_4, 264, 305);
    lv_obj_set_size(ui->screen_5_btn_4, 161, 39);

    //Write style for screen_5_btn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5_btn_4, lv_color_hex(0x1C1D1E), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5_btn_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_5_btn_4, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_5_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_5_btn_4, lv_color_hex(0x3b3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_5_btn_4, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_btn_4, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_btn_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_btn_4, &lv_font_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_btn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_btn_6
    ui->screen_5_btn_6 = lv_btn_create(ui->screen_5);
    ui->screen_5_btn_6_label = lv_label_create(ui->screen_5_btn_6);
    lv_label_set_text(ui->screen_5_btn_6_label, "\n");
    lv_label_set_long_mode(ui->screen_5_btn_6_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_btn_6_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_btn_6, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_5_btn_6_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_5_btn_6, 11, 13);
    lv_obj_set_size(ui->screen_5_btn_6, 204, 91);

    //Write style for screen_5_btn_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_5_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_btn_6, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_btn_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_btn_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_btn_6, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_btn_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_btn_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_3
    ui->screen_5_img_3 = lv_img_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_3, &_sgderhbrdsdfgs_alpha_82x82);
    lv_img_set_pivot(ui->screen_5_img_3, 50,50);
    lv_img_set_angle(ui->screen_5_img_3, 0);
    lv_obj_set_pos(ui->screen_5_img_3, 1163, 27);
    lv_obj_set_size(ui->screen_5_img_3, 82, 82);

    //Write style for screen_5_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_imgbtn_1
    ui->screen_5_imgbtn_1 = lv_imgbtn_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_1, LV_IMGBTN_STATE_RELEASED, NULL, &_dbdfsbdf_alpha_250x75, NULL);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_1, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_hfgjnfgcj_alpha_250x75, NULL);
    ui->screen_5_imgbtn_1_label = lv_label_create(ui->screen_5_imgbtn_1);
    lv_label_set_text(ui->screen_5_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->screen_5_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_imgbtn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_5_imgbtn_1, 68, 152);
    lv_obj_set_size(ui->screen_5_imgbtn_1, 250, 75);

    //Write style for screen_5_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_5_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_5_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_1, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_1, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes screen_5_imgbtn_2
    ui->screen_5_imgbtn_2 = lv_imgbtn_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_imgbtn_2, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_2, LV_IMGBTN_STATE_RELEASED, NULL, &_dvdafvd_alpha_219x75, NULL);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_2, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_fbhdfhn_alpha_219x75, NULL);
    ui->screen_5_imgbtn_2_label = lv_label_create(ui->screen_5_imgbtn_2);
    lv_label_set_text(ui->screen_5_imgbtn_2_label, "");
    lv_label_set_long_mode(ui->screen_5_imgbtn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_imgbtn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_imgbtn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_5_imgbtn_2, 318, 152);
    lv_obj_set_size(ui->screen_5_imgbtn_2, 219, 75);

    //Write style for screen_5_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_5_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_5_imgbtn_2, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_2, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_2, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes screen_5_imgbtn_3
    ui->screen_5_imgbtn_3 = lv_imgbtn_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_imgbtn_3, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_3, LV_IMGBTN_STATE_RELEASED, NULL, &_dfgdsfg_alpha_221x71, NULL);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_3, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_bfdhbndfxhnb_alpha_221x71, NULL);
    ui->screen_5_imgbtn_3_label = lv_label_create(ui->screen_5_imgbtn_3);
    lv_label_set_text(ui->screen_5_imgbtn_3_label, "");
    lv_label_set_long_mode(ui->screen_5_imgbtn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_imgbtn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_imgbtn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_5_imgbtn_3, 537, 152);
    lv_obj_set_size(ui->screen_5_imgbtn_3, 221, 71);

    //Write style for screen_5_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_3, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_3, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_5_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_3, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_3, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_5_imgbtn_3, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_3, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_3, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes screen_5_imgbtn_4
    ui->screen_5_imgbtn_4 = lv_imgbtn_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_imgbtn_4, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_4, LV_IMGBTN_STATE_RELEASED, NULL, &_afsdfv_alpha_236x75, NULL);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_4, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_nfgnjgfj_alpha_236x75, NULL);
    ui->screen_5_imgbtn_4_label = lv_label_create(ui->screen_5_imgbtn_4);
    lv_label_set_text(ui->screen_5_imgbtn_4_label, "");
    lv_label_set_long_mode(ui->screen_5_imgbtn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_imgbtn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_imgbtn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_5_imgbtn_4, 759, 152);
    lv_obj_set_size(ui->screen_5_imgbtn_4, 236, 75);

    //Write style for screen_5_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_4, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_5_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_4, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_5_imgbtn_4, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_4, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_4, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes screen_5_imgbtn_5
    ui->screen_5_imgbtn_5 = lv_imgbtn_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_imgbtn_5, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_5, LV_IMGBTN_STATE_RELEASED, NULL, &_dfbdsf_alpha_231x74, NULL);
    lv_imgbtn_set_src(ui->screen_5_imgbtn_5, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_njfnjg_alpha_231x74, NULL);
    ui->screen_5_imgbtn_5_label = lv_label_create(ui->screen_5_imgbtn_5);
    lv_label_set_text(ui->screen_5_imgbtn_5_label, "");
    lv_label_set_long_mode(ui->screen_5_imgbtn_5_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_5_imgbtn_5_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_5_imgbtn_5, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_5_imgbtn_5, 995, 153);
    lv_obj_set_size(ui->screen_5_imgbtn_5, 231, 74);

    //Write style for screen_5_imgbtn_5, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_5, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_5, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_5_imgbtn_5, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_5_imgbtn_5, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_5_imgbtn_5, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_5_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_5_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_5_imgbtn_5, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_imgbtn_5, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->screen_5_imgbtn_5, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes screen_5_ta_1
    ui->screen_5_ta_1 = lv_textarea_create(ui->screen_5);
    lv_textarea_set_text(ui->screen_5_ta_1, "");
    lv_textarea_set_placeholder_text(ui->screen_5_ta_1, "请点选歌名首字母");
    lv_textarea_set_password_bullet(ui->screen_5_ta_1, "*");
    lv_textarea_set_password_mode(ui->screen_5_ta_1, false);
    lv_textarea_set_one_line(ui->screen_5_ta_1, false);
    lv_textarea_set_accepted_chars(ui->screen_5_ta_1, "");
    lv_textarea_set_max_length(ui->screen_5_ta_1, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_5_ta_1, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->screen_5_ta_1, 78, 232);
    lv_obj_set_size(ui->screen_5_ta_1, 349, 57);

    //Write style for screen_5_ta_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_5_ta_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_ta_1, &lv_font_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_ta_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_5_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_5_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_5_ta_1, lv_color_hex(0xb6a91e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_5_ta_1, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_ta_1, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_5_ta_1, lv_color_hex(0xb0855e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_5_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_5_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_5_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_5_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_ta_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_ta_1, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_ta_1, 17, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_5_ta_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_5_cont_1
    ui->screen_5_cont_1 = lv_obj_create(ui->screen_5);
    lv_obj_set_pos(ui->screen_5_cont_1, 486, 242);
    lv_obj_set_size(ui->screen_5_cont_1, 715, 241);
    lv_obj_set_scrollbar_mode(ui->screen_5_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_cont_2
    ui->screen_5_cont_2 = lv_obj_create(ui->screen_5);
    lv_obj_set_pos(ui->screen_5_cont_2, 486, 472);
    lv_obj_set_size(ui->screen_5_cont_2, 715, 241);
    lv_obj_set_scrollbar_mode(ui->screen_5_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_cont_3
    ui->screen_5_cont_3 = lv_obj_create(ui->screen_5);
    lv_obj_set_pos(ui->screen_5_cont_3, 453, 242);
    lv_obj_set_size(ui->screen_5_cont_3, 747, 463);
    lv_obj_set_scrollbar_mode(ui->screen_5_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_5.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_5);

    //Init events for screen.
    events_init_screen_5(ui);
}
