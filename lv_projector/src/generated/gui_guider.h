/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

typedef struct
{
  
	lv_obj_t *screen_100;
	bool screen_100_del;
	lv_obj_t *screen_100_btn_1;
	lv_obj_t *screen_100_btn_1_label;
	lv_obj_t *screen_100_btn_2;
	lv_obj_t *screen_100_btn_2_label;
	lv_obj_t *screen_100_label_1;
	lv_obj_t *screen_100_img_1;
	lv_obj_t *screen_100_img_2;
	lv_obj_t *screen_100_img_3;
	lv_obj_t *screen_100_img_4;
	lv_obj_t *screen_4;
	bool screen_4_del;
	lv_obj_t *screen_4_cont_9;
	lv_obj_t *screen_4_cont_17;
	lv_obj_t *screen_4_label_8;
	lv_obj_t *screen_4_slider_7;
	lv_obj_t *screen_4_cont_16;
	lv_obj_t *screen_4_label_7;
	lv_obj_t *screen_4_ddlist_5;
	lv_obj_t *screen_4_cont_15;
	lv_obj_t *screen_4_label_6;
	lv_obj_t *screen_4_ddlist_4;
	lv_obj_t *screen_4_cont_14;
	lv_obj_t *screen_4_label_5;
	lv_obj_t *screen_4_slider_4;
	lv_obj_t *screen_4_cont_13;
	lv_obj_t *screen_4_label_4;
	lv_obj_t *screen_4_slider_3;
	lv_obj_t *screen_4_cont_12;
	lv_obj_t *screen_4_label_3;
	lv_obj_t *screen_4_slider_2;
	lv_obj_t *screen_4_cont_11;
	lv_obj_t *screen_4_label_2;
	lv_obj_t *screen_4_slider_1;
	lv_obj_t *screen_4_cont_10;
	lv_obj_t *screen_4_label_1;
	lv_obj_t *screen_4_ddlist_1;
	lv_obj_t *screen_4_cont_18;
	lv_obj_t *screen_4_cont_24;
	lv_obj_t *screen_4_label_15;
	lv_obj_t *screen_4_ddlist_7;
	lv_obj_t *screen_4_cont_21;
	lv_obj_t *screen_4_label_12;
	lv_obj_t *screen_4_slider_6;
	lv_obj_t *screen_4_cont_20;
	lv_obj_t *screen_4_label_11;
	lv_obj_t *screen_4_slider_5;
	lv_obj_t *screen_4_cont_19;
	lv_obj_t *screen_4_label_10;
	lv_obj_t *screen_4_ddlist_6;
	lv_obj_t *screen_4_cont_25;
	lv_obj_t *screen_4_cont_31;
	lv_obj_t *screen_4_label_21;
	lv_obj_t *screen_4_ddlist_9;
	lv_obj_t *screen_4_cont_26;
	lv_obj_t *screen_4_label_16;
	lv_obj_t *screen_4_ddlist_8;
	lv_obj_t *screen_4_cont_35;
	lv_obj_t *screen_4_label_25;
	lv_obj_t *screen_4_cont_34;
	lv_obj_t *screen_4_label_24;
	lv_obj_t *screen_4_cont_33;
	lv_obj_t *screen_4_label_23;
	lv_obj_t *screen_4_ddlist_11;
	lv_obj_t *screen_4_cont_36;
	lv_obj_t *screen_4_cont_41;
	lv_obj_t *screen_4_label_30;
	lv_obj_t *screen_4_cont_40;
	lv_obj_t *screen_4_label_29;
	lv_obj_t *screen_4_sw_1;
	lv_obj_t *screen_4_cont_39;
	lv_obj_t *screen_4_label_28;
	lv_obj_t *screen_4_label_31;
	lv_obj_t *screen_4_cont_42;
	lv_obj_t *screen_4_calendar_1;
	lv_obj_t *screen_4_cont_43;
	lv_obj_t *screen_4_list_1;
	lv_obj_t *screen_4_list_1_item0;
	lv_obj_t *screen_4_list_1_item1;
	lv_obj_t *screen_4_list_1_item2;
	lv_obj_t *screen_4_list_1_item3;
	lv_obj_t *screen_4_list_1_item4;
	lv_obj_t *screen_4_list_1_item5;
	lv_obj_t *screen_4_label_9;
	lv_obj_t *screen_4_btn_1;
	lv_obj_t *screen_4_btn_1_label;
	lv_obj_t *screen_log_in;
	bool screen_log_in_del;
	lv_obj_t *screen_log_in_cont_1;
	lv_obj_t *screen_log_in_ta_1;
	lv_obj_t *screen_log_in_btn_1;
	lv_obj_t *screen_log_in_btn_1_label;
	lv_obj_t *screen_log_in_cont_2;
	lv_obj_t *screen_log_in_btn_2;
	lv_obj_t *screen_log_in_btn_2_label;
	lv_obj_t *screen_log_in_btn_3;
	lv_obj_t *screen_log_in_btn_3_label;
	lv_obj_t *screen_log_in_ta_2;
	lv_obj_t *screen_log_in_btn_4;
	lv_obj_t *screen_log_in_btn_4_label;
	lv_obj_t *screen_log_in_btn_5;
	lv_obj_t *screen_log_in_btn_5_label;
	lv_obj_t *screen_2;
	bool screen_2_del;
	lv_obj_t *screen_2_cont_1;
	lv_obj_t *screen_2_img_12;
	lv_obj_t *screen_2_img_10;
	lv_obj_t *screen_2_img_9;
	lv_obj_t *screen_2_img_8;
	lv_obj_t *screen_2_img_7;
	lv_obj_t *screen_2_img_6;
	lv_obj_t *screen_2_img_5;
	lv_obj_t *screen_2_img_4;
	lv_obj_t *screen_2_img_3;
	lv_obj_t *screen_2_btn_13;
	lv_obj_t *screen_2_btn_13_label;
	lv_obj_t *screen_2_btn_12;
	lv_obj_t *screen_2_btn_12_label;
	lv_obj_t *screen_2_btn_10;
	lv_obj_t *screen_2_btn_10_label;
	lv_obj_t *screen_2_btn_9;
	lv_obj_t *screen_2_btn_9_label;
	lv_obj_t *screen_2_btn_8;
	lv_obj_t *screen_2_btn_8_label;
	lv_obj_t *screen_2_btn_14;
	lv_obj_t *screen_2_btn_14_label;
	lv_obj_t *screen_2_btn_15;
	lv_obj_t *screen_2_btn_15_label;
	lv_obj_t *screen_2_btn_16;
	lv_obj_t *screen_2_btn_16_label;
	lv_obj_t *screen_7;
	bool screen_7_del;
	lv_obj_t *screen_7_img_4;
	lv_obj_t *screen_7_btn_12;
	lv_obj_t *screen_7_btn_12_label;
	lv_obj_t *screen_7_btn_13;
	lv_obj_t *screen_7_btn_13_label;
	lv_obj_t *screen_7_btn_14;
	lv_obj_t *screen_7_btn_14_label;
	lv_obj_t *screen_7_btn_15;
	lv_obj_t *screen_7_btn_15_label;
	lv_obj_t *screen_7_btn_16;
	lv_obj_t *screen_7_btn_16_label;
	lv_obj_t *screen_7_btn_17;
	lv_obj_t *screen_7_btn_17_label;
	lv_obj_t *screen_7_btn_18;
	lv_obj_t *screen_7_btn_18_label;
	lv_obj_t *screen_7_btn_19;
	lv_obj_t *screen_7_btn_19_label;
	lv_obj_t *screen_7_btn_20;
	lv_obj_t *screen_7_btn_20_label;
	lv_obj_t *screen_7_cont_1;
	lv_obj_t *screen_7_img_2;
	lv_obj_t *screen_7_label_1;
	lv_obj_t *screen_7_img_3;
	lv_obj_t *screen_7_btn_11;
	lv_obj_t *screen_7_btn_11_label;
	lv_obj_t *screen_8;
	bool screen_8_del;
	lv_obj_t *screen_8_cont_13;
	lv_obj_t *screen_8_cont_14;
	lv_obj_t *screen_8_cont_16;
	lv_obj_t *screen_8_cont_15;
	lv_obj_t *screen_8_cont_6;
	lv_obj_t *screen_8_img_100;
	lv_obj_t *screen_8_btn_18;
	lv_obj_t *screen_8_btn_18_label;
	lv_obj_t *screen_8_btn_17;
	lv_obj_t *screen_8_btn_17_label;
	lv_obj_t *screen_8_btn_16;
	lv_obj_t *screen_8_btn_16_label;
	lv_obj_t *screen_8_btn_15;
	lv_obj_t *screen_8_btn_15_label;
	lv_obj_t *screen_8_btn_14;
	lv_obj_t *screen_8_btn_14_label;
	lv_obj_t *screen_8_btn_13;
	lv_obj_t *screen_8_btn_13_label;
	lv_obj_t *screen_8_btn_12;
	lv_obj_t *screen_8_btn_12_label;
	lv_obj_t *screen_8_btn_30;
	lv_obj_t *screen_8_btn_30_label;
	lv_obj_t *screen_8_btn_19;
	lv_obj_t *screen_8_btn_19_label;
	lv_obj_t *screen_8_cont_10;
	lv_obj_t *screen_8_img_85;
	lv_obj_t *screen_8_img_86;
	lv_obj_t *screen_8_img_87;
	lv_obj_t *screen_8_img_88;
	lv_obj_t *screen_8_img_91;
	lv_obj_t *screen_8_img_90;
	lv_obj_t *screen_8_img_89;
	lv_obj_t *screen_8_img_94;
	lv_obj_t *screen_8_img_93;
	lv_obj_t *screen_8_img_92;
	lv_obj_t *screen_8_btn_20;
	lv_obj_t *screen_8_btn_20_label;
	lv_obj_t *screen_8_btn_21;
	lv_obj_t *screen_8_btn_21_label;
	lv_obj_t *screen_8_btn_22;
	lv_obj_t *screen_8_btn_22_label;
	lv_obj_t *screen_8_btn_25;
	lv_obj_t *screen_8_btn_25_label;
	lv_obj_t *screen_8_btn_26;
	lv_obj_t *screen_8_btn_26_label;
	lv_obj_t *screen_8_btn_27;
	lv_obj_t *screen_8_btn_27_label;
	lv_obj_t *screen_8_btn_29;
	lv_obj_t *screen_8_btn_29_label;
	lv_obj_t *screen_8_cont_11;
	lv_obj_t *screen_8_slider_1;
	lv_obj_t *screen_8_cont_12;
	lv_obj_t *screen_8_img_98;
	lv_obj_t *screen_8_img_99;
	lv_obj_t *screen_8_label_2;
	lv_obj_t *screen_8_btn_31;
	lv_obj_t *screen_8_btn_31_label;
	lv_obj_t *screen_3;
	bool screen_3_del;
	lv_obj_t *screen_3_img_15;
	lv_obj_t *screen_3_cont_4;
	lv_obj_t *screen_3_cont_2;
	lv_obj_t *screen_3_btn_4;
	lv_obj_t *screen_3_btn_4_label;
	lv_obj_t *screen_3_btn_3;
	lv_obj_t *screen_3_btn_3_label;
	lv_obj_t *screen_3_ta_1;
	lv_obj_t *screen_3_btn_6;
	lv_obj_t *screen_3_btn_6_label;
	lv_obj_t *screen_3_cont_3;
	lv_obj_t *screen_3_line_1;
	lv_obj_t *screen_3_img_14;
	lv_obj_t *screen_3_label_2;
	lv_obj_t *screen_5;
	bool screen_5_del;
	lv_obj_t *screen_5_img_4;
	lv_obj_t *screen_5_btn_3;
	lv_obj_t *screen_5_btn_3_label;
	lv_obj_t *screen_5_btn_4;
	lv_obj_t *screen_5_btn_4_label;
	lv_obj_t *screen_5_btn_6;
	lv_obj_t *screen_5_btn_6_label;
	lv_obj_t *screen_5_img_3;
	lv_obj_t *screen_5_imgbtn_1;
	lv_obj_t *screen_5_imgbtn_1_label;
	lv_obj_t *screen_5_imgbtn_2;
	lv_obj_t *screen_5_imgbtn_2_label;
	lv_obj_t *screen_5_imgbtn_3;
	lv_obj_t *screen_5_imgbtn_3_label;
	lv_obj_t *screen_5_imgbtn_4;
	lv_obj_t *screen_5_imgbtn_4_label;
	lv_obj_t *screen_5_imgbtn_5;
	lv_obj_t *screen_5_imgbtn_5_label;
	lv_obj_t *screen_5_ta_1;
	lv_obj_t *screen_5_cont_1;
	lv_obj_t *screen_5_cont_2;
	lv_obj_t *screen_5_cont_3;
	lv_obj_t *screen_13;
	bool screen_13_del;
	lv_obj_t *screen_13_img_1;
	lv_obj_t *screen_13_btn_1;
	lv_obj_t *screen_13_btn_1_label;
	lv_obj_t *screen_13_img_2;
	lv_obj_t *screen_8_1;
	bool screen_8_1_del;
	lv_obj_t *screen_8_1_cont_5;
	lv_obj_t *screen_8_1_img_1;
	lv_obj_t *screen_8_1_cont_1;
	lv_obj_t *screen_8_1_ta_2;
	lv_obj_t *screen_8_1_btn_10;
	lv_obj_t *screen_8_1_btn_10_label;
	lv_obj_t *screen_8_1_btn_9;
	lv_obj_t *screen_8_1_btn_9_label;
	lv_obj_t *screen_8_1_btn_6;
	lv_obj_t *screen_8_1_btn_6_label;
	lv_obj_t *screen_8_1_cont_2;
	lv_obj_t *screen_14;
	bool screen_14_del;
	lv_obj_t *screen_14_img_14;
	lv_obj_t *screen_14_cont_2;
	lv_obj_t *screen_14_img_12;
	lv_obj_t *screen_14_cont_4;
	lv_obj_t *screen_14_btn_11;
	lv_obj_t *screen_14_btn_11_label;
	lv_obj_t *screen_14_cont_3;
	lv_obj_t *screen_14_img_10;
	lv_obj_t *screen_14_btn_10;
	lv_obj_t *screen_14_btn_10_label;
	lv_obj_t *screen_14_img_13;
	lv_obj_t *screen_1;
	bool screen_1_del;
	lv_obj_t *screen_1_img_25;
	lv_obj_t *screen_1_cont_3;
	lv_obj_t *screen_1_btn_20;
	lv_obj_t *screen_1_btn_20_label;
	lv_obj_t *screen_1_line_2;
	lv_obj_t *screen_1_btn_25;
	lv_obj_t *screen_1_btn_25_label;
	lv_obj_t *screen_1_btn_24;
	lv_obj_t *screen_1_btn_24_label;
	lv_obj_t *screen_1_btn_23;
	lv_obj_t *screen_1_btn_23_label;
	lv_obj_t *screen_1_cont_10;
	lv_obj_t *screen_1_cont_11;
	lv_obj_t *screen_1_img_26;
	lv_obj_t *screen_11;
	bool screen_11_del;
	lv_obj_t *screen_11_cont_2;
	lv_obj_t *screen_11_img_14;
	lv_obj_t *screen_11_list_1;
	lv_obj_t *screen_11_list_1_item0;
	lv_obj_t *screen_11_list_1_item1;
	lv_obj_t *screen_11_btn_17;
	lv_obj_t *screen_11_btn_17_label;
	lv_obj_t *screen_11_line_1;
	lv_obj_t *screen_11_cont_4;
	lv_obj_t *screen_11_img_15;
	lv_obj_t *screen_11_img_2;
	lv_obj_t *screen_9;
	bool screen_9_del;
	lv_obj_t *screen_9_img_1;
	lv_obj_t *screen_9_cont_2;
	lv_obj_t *screen_9_cont_4;
	lv_obj_t *screen_9_btn_6;
	lv_obj_t *screen_9_btn_6_label;
	lv_obj_t *screen_9_btn_5;
	lv_obj_t *screen_9_btn_5_label;
	lv_obj_t *screen_9_img_14;
	lv_obj_t *screen_10;
	bool screen_10_del;
	lv_obj_t *screen_10_img_14;
	lv_obj_t *screen_10_cont_3;
	lv_obj_t *screen_10_img_2;
	lv_obj_t *screen_10_cont_1;
	lv_obj_t *screen_10_btn_1;
	lv_obj_t *screen_10_btn_1_label;
	lv_obj_t *screen_10_img_12;
	lv_obj_t *screen_10_btn_12;
	lv_obj_t *screen_10_btn_12_label;
	lv_obj_t *screen_10_img_13;
	lv_obj_t *screen_101;
	bool screen_101_del;
	lv_obj_t *screen_101_cont_1;
	lv_obj_t *screen_101_btn_1;
	lv_obj_t *screen_101_btn_1_label;
	lv_obj_t *screen_102;
	bool screen_102_del;
	lv_obj_t *screen_102_img_2;
	lv_obj_t *screen_102_btn_1;
	lv_obj_t *screen_102_btn_1_label;
	lv_obj_t *screen_102_img_1;
	lv_obj_t *screen_102_btn_3;
	lv_obj_t *screen_102_btn_3_label;
	lv_obj_t *screen_103;
	bool screen_103_del;
	lv_obj_t *g_kb_top_layer;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen_100(lv_ui *ui);
void setup_scr_screen_4(lv_ui *ui);
void setup_scr_screen_log_in(lv_ui *ui);
void setup_scr_screen_2(lv_ui *ui);
void setup_scr_screen_7(lv_ui *ui);
void setup_scr_screen_8(lv_ui *ui);
void setup_scr_screen_3(lv_ui *ui);
void setup_scr_screen_5(lv_ui *ui);
void setup_scr_screen_13(lv_ui *ui);
void setup_scr_screen_8_1(lv_ui *ui);
void setup_scr_screen_14(lv_ui *ui);
void setup_scr_screen_1(lv_ui *ui);
void setup_scr_screen_11(lv_ui *ui);
void setup_scr_screen_9(lv_ui *ui);
void setup_scr_screen_10(lv_ui *ui);
void setup_scr_screen_101(lv_ui *ui);
void setup_scr_screen_102(lv_ui *ui);
void setup_scr_screen_103(lv_ui *ui);
LV_IMG_DECLARE(_ererhgredh1_alpha_44x35);
LV_IMG_DECLARE(_ererhgredh4_alpha_44x35);
LV_IMG_DECLARE(_ererhgredh3_alpha_44x35);
LV_IMG_DECLARE(_ererhgredh2_alpha_44x35);
LV_IMG_DECLARE(_123124_1280x103);
LV_IMG_DECLARE(_DIV1_alpha_80x112);
LV_IMG_DECLARE(_DIV5_alpha_80x112);
LV_IMG_DECLARE(_DIV6_alpha_80x112);
LV_IMG_DECLARE(_DIV4_alpha_80x112);
LV_IMG_DECLARE(_DIV3_alpha_80x112);
LV_IMG_DECLARE(_DIV7_alpha_80x112);
LV_IMG_DECLARE(_DIV2_alpha_80x112);
LV_IMG_DECLARE(_DIV8_alpha_80x112);
LV_IMG_DECLARE(_sgderhbrd_alpha_324x122);
LV_IMG_DECLARE(_rhbstrhrtfd_alpha_214x49);
LV_IMG_DECLARE(_123124_1280x103);
LV_IMG_DECLARE(_DIV_alpha_80x112);
LV_IMG_DECLARE(_DIV1_alpha_80x112);
LV_IMG_DECLARE(_DIV5_alpha_80x112);
LV_IMG_DECLARE(_DIV6_alpha_80x112);
LV_IMG_DECLARE(_DIV4_alpha_80x112);
LV_IMG_DECLARE(_DIV3_alpha_80x112);
LV_IMG_DECLARE(_DIV7_alpha_80x112);
LV_IMG_DECLARE(_DIV2_alpha_80x112);
LV_IMG_DECLARE(_DIV8_alpha_80x112);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_100x100);
LV_IMG_DECLARE(_rhbstrhrtfd_alpha_214x49);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_82x82);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_82x82);
LV_IMG_DECLARE(_dbdfsbdf_alpha_250x75);
LV_IMG_DECLARE(_hfgjnfgcj_alpha_250x75);
LV_IMG_DECLARE(_dvdafvd_alpha_219x75);
LV_IMG_DECLARE(_fbhdfhn_alpha_219x75);
LV_IMG_DECLARE(_dfgdsfg_alpha_221x71);
LV_IMG_DECLARE(_bfdhbndfxhnb_alpha_221x71);
LV_IMG_DECLARE(_afsdfv_alpha_236x75);
LV_IMG_DECLARE(_nfgnjgfj_alpha_236x75);
LV_IMG_DECLARE(_dfbdsf_alpha_231x74);
LV_IMG_DECLARE(_njfnjg_alpha_231x74);
LV_IMG_DECLARE(_dhbdhnbfd_alpha_409x418);
LV_IMG_DECLARE(_speake1_1280x94);
LV_IMG_DECLARE(_sdgvdsg_alpha_223x48);
LV_IMG_DECLARE(_gvsdrbhgdh_alpha_147x48);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_82x82);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_82x82);
LV_IMG_DECLARE(_vdsbdf_alpha_223x48);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_82x82);
LV_IMG_DECLARE(_ewafe_404x83);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_82x82);
LV_IMG_DECLARE(_vdsbdf_alpha_223x48);
LV_IMG_DECLARE(_gvsdrbhgdh_alpha_147x47);
LV_IMG_DECLARE(_sgderhbrdsdfgs_alpha_82x82);
LV_IMG_DECLARE(_vdsbdf_alpha_223x48);

LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_lv_font_ktv_30_30)
LV_FONT_DECLARE(lv_font_Regular_31)
LV_FONT_DECLARE(lv_font_Regular_24)
LV_FONT_DECLARE(lv_font_Regular_20)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_montserratMedium_34)
LV_FONT_DECLARE(lv_font_Regular_28)
LV_FONT_DECLARE(lv_font_Regular_23)


#ifdef __cplusplus
}
#endif
#endif
