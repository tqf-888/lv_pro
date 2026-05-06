/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl/lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include "ktv_player_ui.h"
#include "lvgl_page_navigation.h"
#include "system_api.h"
#include "page_manager.h"
#include "artist_page_demo.h"
#include "ktv.h"
#include "rich_song_page_demo.h"
#include "rich_rank_page_demo.h"
#include "db_list_pro_worker.h"
int show_cnt = 0;
#include "recommend_video_fetch.h"
#include "lv_linux_folder_demo.h"
#include "lv_wifi_list_demo.h"

static void screen_100_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {

        break;
    }
    default:
        break;
    }
}

static void screen_100_btn_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {

        break;
    }
    default:
        break;
    }
}

void events_init_screen_100 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_100_btn_1, screen_100_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_100_btn_2, screen_100_btn_2_event_handler, LV_EVENT_ALL, ui);
}

static void screen_4_slider_7_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_4_slider_7);
        set_pq_param(P_SYS_ZOOM_DIS_MODE, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_ddlist_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        uint16_t id = lv_dropdown_get_selected(guider_ui.screen_4_ddlist_5);
        set_pq_param(P_ASPECT_RATIO, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_ddlist_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        uint16_t id = lv_dropdown_get_selected(guider_ui.screen_4_ddlist_4);
        set_pq_param(P_COLOR_TEMP, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_slider_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        break;
    }
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_4_slider_4);
        set_pq_param(P_SHARPNESS, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_slider_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_4_slider_3);
        set_pq_param(P_COLOR, id);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_4_slider_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_4_slider_2);
        set_pq_param(P_BRIGHTNESS, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_slider_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_4_slider_1);
        set_pq_param(P_CONTRAST, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_ddlist_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        uint16_t id = lv_dropdown_get_selected(guider_ui.screen_4_ddlist_1);
        set_pq_param(P_PICTURE_MODE, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_ddlist_7_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        uint16_t id = lv_dropdown_get_selected(guider_ui.screen_4_ddlist_7);
        audio_set_param(P_SOUND_OUT_MODE, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_slider_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_4_slider_6);
        set_pq_param(P_SOUND_TREBLE, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_slider_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_4_slider_5);
        set_pq_param(P_SOUND_BASS, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_ddlist_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        uint16_t id = lv_dropdown_get_selected(guider_ui.screen_4_ddlist_6);
        audio_set_param(P_SOUND_MODE, id);
        break;
    }
    default:
        break;
    }
}

static void screen_4_list_1_item0_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_4_cont_43, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_42, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_9, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_18, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_25, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_36, LV_OBJ_FLAG_HIDDEN);

        app_wifi_list_open(guider_ui.screen_4_cont_43);
        break;
    }
    default:
        break;
    }
}

static void screen_4_list_1_item1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_4_cont_18, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_9, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_25, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_36, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_42, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_43, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_4_list_1_item2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_4_cont_25, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_9, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_18, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_36, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_42, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_43, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_4_list_1_item3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_4_cont_36, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_9, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_18, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_25, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_42, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_43, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_4_list_1_item4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_4_cont_42, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_9, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_18, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_25, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_36, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_43, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_4_list_1_item5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {

        lv_obj_clear_flag(guider_ui.screen_4_cont_9, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_4_cont_18, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_25, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_36, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_42, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_4_cont_43, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_4_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        app_wifi_list_close();
        lv_keyboard_set_layout(guider_ui.g_kb_top_layer, LV_KEYBOARD_LAYOUT_AZ);
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_4 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_4_slider_7, screen_4_slider_7_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_ddlist_5, screen_4_ddlist_5_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_ddlist_4, screen_4_ddlist_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_slider_4, screen_4_slider_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_slider_3, screen_4_slider_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_slider_2, screen_4_slider_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_slider_1, screen_4_slider_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_ddlist_1, screen_4_ddlist_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_ddlist_7, screen_4_ddlist_7_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_slider_6, screen_4_slider_6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_slider_5, screen_4_slider_5_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_ddlist_6, screen_4_ddlist_6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_list_1_item0, screen_4_list_1_item0_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_list_1_item1, screen_4_list_1_item1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_list_1_item2, screen_4_list_1_item2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_list_1_item3, screen_4_list_1_item3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_list_1_item4, screen_4_list_1_item4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_list_1_item5, screen_4_list_1_item5_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_4_btn_1, screen_4_btn_1_event_handler, LV_EVENT_ALL, ui);
}

static void screen_log_in_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_obj_clear_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_log_in_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_log_in_cont_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_log_in_cont_1, LV_OBJ_FLAG_HIDDEN);
        extern void set_phone_num(const char *phone_num);
        set_phone_num(lv_textarea_get_text(guider_ui.screen_log_in_ta_1));
        printf("6666");
        fprintf(stdout, "6666");
        break;
    }
    default:
        break;
    }
}

static void screen_log_in_btn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        extern void set_verification_code(const char *code);
        set_verification_code(lv_textarea_get_text(guider_ui.screen_log_in_ta_2));
        break;
    }
    default:
        break;
    }
}

static void screen_log_in_btn_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_textarea_del_char(guider_ui.screen_log_in_ta_2);

        break;
    }
    default:
        break;
    }
}

static void screen_log_in_btn_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_textarea_set_text(guider_ui.screen_log_in_ta_2, "");
        break;
    }
    default:
        break;
    }
}

void events_init_screen_log_in (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_log_in, screen_log_in_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_log_in_btn_1, screen_log_in_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_log_in_btn_3, screen_log_in_btn_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_log_in_btn_4, screen_log_in_btn_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_log_in_btn_5, screen_log_in_btn_5_event_handler, LV_EVENT_ALL, ui);
}

static void screen_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        karaoke_demo_set_positions(LV_ALIGN_TOP_LEFT, 20, 520,LV_ALIGN_BOTTOM_RIGHT, -20, -120);

        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_set_video_pos, 2, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_13_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_12_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_NEXT, subpage_get(), 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_10_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PLAY, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_9_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        show_cnt = 0;
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_8_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_102",&guider_ui.screen_102,&guider_ui.screen_102_del,setup_scr_screen_102);
        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_14_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_REPLAY, subpage_get(), 0, NULL, NULL, NULL);

        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_15_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {

        show_cnt++;

        if (show_cnt % 2 == 1) {
            lv_obj_clear_flag(guider_ui.screen_2_cont_1, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(guider_ui.screen_2_cont_1, LV_OBJ_FLAG_HIDDEN);
        }

        break;
    }
    default:
        break;
    }
}

static void screen_2_btn_16_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_love_song, 0, 0, get_last_artist(), get_last_song_name(), get_last_clicked_url());
        break;
    }
    default:
        break;
    }
}

void events_init_screen_2 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_2, screen_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_13, screen_2_btn_13_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_12, screen_2_btn_12_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_10, screen_2_btn_10_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_9, screen_2_btn_9_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_8, screen_2_btn_8_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_14, screen_2_btn_14_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_15, screen_2_btn_15_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_btn_16, screen_2_btn_16_event_handler, LV_EVENT_ALL, ui);
}

static void screen_7_btn_12_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        subpage_set(0);
        page_nav_push("screen_8",&guider_ui.screen_8,&guider_ui.screen_8_del,setup_scr_screen_8);
        break;
    }
    default:
        break;
    }
}

static void screen_7_btn_15_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_100",&guider_ui.screen_100,&guider_ui.screen_100_del,setup_scr_screen_100);
        break;
    }
    default:
        break;
    }
}

static void screen_7_btn_16_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_4",&guider_ui.screen_4,&guider_ui.screen_4_del,setup_scr_screen_4);
        break;
    }
    default:
        break;
    }
}

static void screen_7_btn_18_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_13",&guider_ui.screen_13,&guider_ui.screen_13_del,setup_scr_screen_13);
        break;
    }
    default:
        break;
    }
}

static void screen_7_btn_11_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_4",&guider_ui.screen_4,&guider_ui.screen_4_del,setup_scr_screen_4);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_7 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_7_btn_12, screen_7_btn_12_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_7_btn_15, screen_7_btn_15_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_7_btn_16, screen_7_btn_16_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_7_btn_18, screen_7_btn_18_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_7_btn_11, screen_7_btn_11_event_handler, LV_EVENT_ALL, ui);
}

static void screen_8_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_AUTO_PLAY, 0, 0, NULL, NULL, NULL);

        karaoke_demo_set_positions(LV_ALIGN_TOP_LEFT, 4000, 4000,LV_ALIGN_BOTTOM_RIGHT, 4000, 4000);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_set_video_pos, 1, 0, NULL, NULL, NULL);

        extern void ktv_time_refresh_label_async(void);
        ktv_time_refresh_label_async();
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_18_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_9",&guider_ui.screen_9,&guider_ui.screen_9_del,setup_scr_screen_9);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_17_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_11",&guider_ui.screen_11,&guider_ui.screen_11_del,setup_scr_screen_11);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_16_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_13",&guider_ui.screen_13,&guider_ui.screen_13_del,setup_scr_screen_13);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_15_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_1",&guider_ui.screen_1,&guider_ui.screen_1_del,setup_scr_screen_1);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_14_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_14",&guider_ui.screen_14,&guider_ui.screen_14_del,setup_scr_screen_14);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_13_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        subpage_set(1);
        page_nav_push("screen_3",&guider_ui.screen_3,&guider_ui.screen_3_del,setup_scr_screen_3);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_12_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_10",&guider_ui.screen_10,&guider_ui.screen_10_del,setup_scr_screen_10);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_30_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_2",&guider_ui.screen_2,&guider_ui.screen_2_del,setup_scr_screen_2);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_19_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_5",&guider_ui.screen_5,&guider_ui.screen_5_del,setup_scr_screen_5);
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_20_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PAUSE, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_21_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_NEXT, 1, 0, NULL, NULL, NULL);

        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_22_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        page_nav_push("screen_2",&guider_ui.screen_2,&guider_ui.screen_2_del,setup_scr_screen_2);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_25_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_PLAY, 0, 0, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_26_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_EXIT_VIDEO, subpage_get(), 0, NULL, NULL, NULL);
        page_manager_init();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_27_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_102",&guider_ui.screen_102,&guider_ui.screen_102_del,setup_scr_screen_102);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_29_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_REPLAY, subpage_get(), 0, NULL, NULL, NULL);

        break;
    }
    default:
        break;
    }
}

static void screen_8_slider_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int32_t id = lv_slider_get_value(guider_ui.screen_8_slider_1);
        set_pq_param(P_SOUND_TREBLE, id);
        break;
    }
    default:
        break;
    }
}

static void screen_8_btn_31_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_push("screen_4",&guider_ui.screen_4,&guider_ui.screen_4_del,setup_scr_screen_4);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_8 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_8, screen_8_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_18, screen_8_btn_18_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_17, screen_8_btn_17_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_16, screen_8_btn_16_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_15, screen_8_btn_15_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_14, screen_8_btn_14_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_13, screen_8_btn_13_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_12, screen_8_btn_12_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_30, screen_8_btn_30_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_19, screen_8_btn_19_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_20, screen_8_btn_20_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_21, screen_8_btn_21_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_22, screen_8_btn_22_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_25, screen_8_btn_25_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_26, screen_8_btn_26_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_27, screen_8_btn_27_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_29, screen_8_btn_29_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_slider_1, screen_8_slider_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_btn_31, screen_8_btn_31_event_handler, LV_EVENT_ALL, ui);
}

static void screen_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_obj_clear_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        demo_app_songs_list(guider_ui.screen_3_cont_3);
        page_set(3);//不可以删，键盘事件要用
        break;
    }
    default:
        break;
    }
}

static void screen_3_btn_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_textarea_set_text(guider_ui.screen_3_ta_1, "");
        rich_song_page_demo_reset(300);
        break;
    }
    default:
        break;
    }
}

static void screen_3_btn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_textarea_del_char(guider_ui.screen_3_ta_1);
        rich_song_page_demo_reset(300);
        break;
    }
    default:
        break;
    }
}

static void screen_3_btn_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        lv_obj_add_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        //page_manager_init();
        rich_song_page_demo_close();

        page_nav_back();
        karaoke_demo_set_positions(LV_ALIGN_TOP_LEFT, 1800, 1800, LV_ALIGN_BOTTOM_RIGHT, 1800, 1800);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_3 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_3, screen_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_3_btn_4, screen_3_btn_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_3_btn_3, screen_3_btn_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_3_btn_6, screen_3_btn_6_event_handler, LV_EVENT_ALL, ui);
}

static void screen_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_obj_clear_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        app_open_artist_page(guider_ui.screen_5_cont_3);
        break;
    }
    default:
        break;
    }
}

static void screen_5_btn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_textarea_del_char(guider_ui.screen_5_ta_1);
        app_reset_artist_page_to_page0(300);
        break;
    }
    default:
        break;
    }
}

static void screen_5_btn_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_textarea_set_text(guider_ui.screen_5_ta_1, "");
        app_reset_artist_page_to_page0(300);
        break;
    }
    default:
        break;
    }
}

static void screen_5_btn_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        //page_manager_init();
        lv_obj_add_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        subpage_set(1);
        app_close_artist_page();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

static void screen_5_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        subpage_set(3);
        app_reset_artist_page_to_page0(300);
        break;
    }
    default:
        break;
    }
}

static void screen_5_imgbtn_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        subpage_set(1);

        app_reset_artist_page_to_page0(300);
        break;
    }
    default:
        break;
    }
}

static void screen_5_imgbtn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        subpage_set(2);

        app_reset_artist_page_to_page0(300);
        break;
    }
    default:
        break;
    }
}

static void screen_5_imgbtn_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        subpage_set(3);

        app_reset_artist_page_to_page0(300);
        break;
    }
    default:
        break;
    }
}

static void screen_5_imgbtn_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        subpage_set(4);

        app_reset_artist_page_to_page0(300);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_5 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_5, screen_5_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_btn_3, screen_5_btn_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_btn_4, screen_5_btn_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_btn_6, screen_5_btn_6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_imgbtn_1, screen_5_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_imgbtn_2, screen_5_imgbtn_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_imgbtn_3, screen_5_imgbtn_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_imgbtn_4, screen_5_imgbtn_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_5_imgbtn_5, screen_5_imgbtn_5_event_handler, LV_EVENT_ALL, ui);
}

static void screen_13_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_13 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_13_btn_1, screen_13_btn_1_event_handler, LV_EVENT_ALL, ui);
}

static void screen_8_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_obj_clear_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        demo_app_songs_list(guider_ui.screen_8_1_cont_2);
        break;
    }
    default:
        break;
    }
}

static void screen_8_1_btn_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        //page_manager_init();
        rich_song_page_demo_close();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_8_1 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_8_1, screen_8_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_8_1_btn_6, screen_8_1_btn_6_event_handler, LV_EVENT_ALL, ui);
}

static void screen_14_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        demo_app_favorite_songs_list(guider_ui.screen_14_cont_4);
        break;
    }
    default:
        break;
    }
}

static void screen_14_btn_11_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        favorite_song_page_demo_close();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

static void screen_14_btn_10_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        favorite_song_page_demo_reset(300);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_14 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_14, screen_14_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_14_btn_11, screen_14_btn_11_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_14_btn_10, screen_14_btn_10_event_handler, LV_EVENT_ALL, ui);
}

static void screen_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        demo_app_top100_songs_list(guider_ui.screen_1_cont_11);
        demo_app_order_songs_list(guider_ui.screen_1_cont_10);
        break;
    }
    default:
        break;
    }
}

static void screen_1_btn_20_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
        //page_manager_init();
        top100_song_page_demo_close();
        order_song_page_demo_close();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

static void screen_1_btn_24_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.screen_1_cont_10, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_1_cont_11, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_1_btn_23_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_1_cont_10, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_1_cont_11, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_1 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_1, screen_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_btn_20, screen_1_btn_20_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_btn_24, screen_1_btn_24_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_btn_23, screen_1_btn_23_event_handler, LV_EVENT_ALL, ui);
}

static void screen_11_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        demo_app_songs_list(guider_ui.screen_11_cont_4);
        break;
    }
    default:
        break;
    }
}

static void screen_11_btn_17_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_manager_init();
        rich_song_page_demo_close();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_11 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_11, screen_11_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_11_btn_17, screen_11_btn_17_event_handler, LV_EVENT_ALL, ui);
}

static void screen_9_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        demo_app_rank_list(guider_ui.screen_9_cont_4);
        break;
    }
    default:
        break;
    }
}

static void screen_9_btn_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t *btn = lv_event_get_target(e);

        // ====================== LVGL V8 专用 获取点击坐标 ======================
        lv_indev_t *indev = lv_indev_get_act();
        lv_point_t point;
        lv_indev_get_point(indev, &point);  // V8 这个函数是可以用的

        // ====================== V8 替代函数：获取控件屏幕坐标 ======================
        // V9: lv_obj_get_screen_y(btn)
        // V8: lv_obj_get_y相对于父组件 + 父组件逐级坐标 → 官方用 lv_obj_get_coords
        lv_area_t coords;
        lv_obj_get_coords(btn, &coords);    // V8 专用：获取按钮在屏幕上的绝对区域
        lv_coord_t btn_abs_y = coords.y1;   // 按钮顶部绝对Y坐标

        lv_coord_t rel_y = point.y - btn_abs_y;
        lv_coord_t height = lv_obj_get_height(btn);

        if (height <= 0) return;

        // 分成6份
        int segment = (rel_y * 6) / height;
        if (segment < 0) segment = 0;
        if (segment >= 6) segment = 5;

        int result = segment + 1;
        printf("点击位置段：%d (偏移=%d, 高度=%d)\n", result, rel_y, height);

        subpage_set(result);
        rich_rank_page_demo_reset(300);
        break;
    }
    default:
        break;
    }
}

static void screen_9_btn_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        singer_id_set(IGNORE_NUM);
        rich_rank_page_demo_close();
        subpage_set(1);

        page_nav_back();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_9 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_9, screen_9_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_9_btn_6, screen_9_btn_6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_9_btn_5, screen_9_btn_5_event_handler, LV_EVENT_ALL, ui);
}

static void screen_10_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {

        demo_app_order_songs_list(guider_ui.screen_10_cont_1);
        break;
    }
    default:
        break;
    }
}

static void screen_10_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        order_song_page_demo_close();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

static void screen_10_btn_12_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        order_song_page_demo_reset(300);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_10 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_10, screen_10_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_10_btn_1, screen_10_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_10_btn_12, screen_10_btn_12_event_handler, LV_EVENT_ALL, ui);
}

static void screen_101_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        test_close_folder_browser();
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_101 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_101_btn_1, screen_101_btn_1_event_handler, LV_EVENT_ALL, ui);
}

static void screen_102_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_add_song, 0, 0, get_last_artist(), get_last_song_name(), get_last_clicked_url());
        break;
    }
    default:
        break;
    }
}

static void screen_102_btn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        page_nav_back();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_102 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_102_btn_1, screen_102_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_102_btn_3, screen_102_btn_3_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
