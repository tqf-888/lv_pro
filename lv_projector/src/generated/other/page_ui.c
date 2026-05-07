/**
 * @Title: lv_wifi_ui.c
 * @Description: WiFi UI 界面实现
 * @author Jeremy
 * @date 2025-01-29
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include "page_manager.h"
#include "gui_guider.h"
#include "page_ui.h"
#include "ktv.h"
#include "file_io.h"

#include "artist_page_demo.h"


#include "ktv_player_ui.h"
#include "ktv_cloud_order_api.h"
#include "db_list_pro_thread.h"

/* 外部全局变量 */
extern lv_ui guider_ui;
int g_song_id = 0;

/* 字体声明 */
LV_FONT_DECLARE(lv_font_Regular_20);

/* 样式定义 */
lv_style_t style_inf_list_btn_default;
lv_style_t style_inf_list_btn_checked;

/* 类别数组，共8项，与 NAV_COL_MAX 保持一致 */
#define NAV_COL_MAX 33
const char *categories[NAV_COL_MAX] = {
    "聚会点唱", "麦霸专属", "00后", "70后", "80后", "90后", "60后",
    "普通话", "粤语", "闽南语", "外语", "抒情歌曲", "慢歌地带",
    "民歌榜", "广场舞TOP", "经典榜", "校园榜", "抖音榜", "影视榜",
    "流行", "民谣", "嘻哈", "摇滚", "古风", "思念", "幸福", "治愈",
    "孤独", "伤感", "激情", "综艺", "网络歌曲"
};

const int tag_ids[NAV_COL_MAX] = {
    12, 471, 26, 17, 25, 35, 115,
    472, 36, 76, 89, 187, 19,
    83, 31, 56, 29, 799, 39,
    9, 45, 713, 42, 66, 44, 511,
    444, 71, 15, 82, 87, 32
};

/**
 * @brief 创建列表按钮的默认样式和选中样式
 */
void lv_page_list_btn_style_create(void)
{
    lv_style_init(&style_inf_list_btn_default);
    lv_style_set_pad_top(&style_inf_list_btn_default, 14);
    lv_style_set_pad_left(&style_inf_list_btn_default, 5);
    lv_style_set_pad_right(&style_inf_list_btn_default, 5);
    lv_style_set_pad_bottom(&style_inf_list_btn_default, 14);
    lv_style_set_border_width(&style_inf_list_btn_default, 0);
    lv_style_set_text_color(&style_inf_list_btn_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_inf_list_btn_default, &lv_font_Regular_20);
    lv_style_set_text_opa(&style_inf_list_btn_default, 255);
    lv_style_set_radius(&style_inf_list_btn_default, 0);
    lv_style_set_bg_opa(&style_inf_list_btn_default, 0);

    lv_style_init(&style_inf_list_btn_checked);
    lv_style_set_bg_color(&style_inf_list_btn_checked, lv_color_hex(0x0055AA));
    lv_style_set_bg_opa(&style_inf_list_btn_checked, LV_OPA_COVER);
    lv_style_set_text_color(&style_inf_list_btn_checked, lv_color_hex(0xffffff));
}

/**
 * @brief 列表按钮点击事件回调
 * @param e 事件对象
 */
static void list_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    if (!btn) return;

    /* 获取用户数据中保存的行号 */
    int row = (int)(intptr_t)lv_obj_get_user_data(btn);
    if (row >= 0 && row < NAV_COL_MAX) {
        printf("点击了第 %d 行: %s, tag_id = %d\n", row + 1, categories[row], tag_ids[row]);

        subpage_set(tag_ids[row]);
        rich_song_page_demo_reset(1000);

    } else {
        printf("点击了未知行\n");
    }
}

/**
 * @brief 刷新整个列表页面（多次调用需注意线程安全）
 * @note 不允许在其他线程中调用
 */
void lv_page_reflash_all(void)
{
    if (!lv_obj_is_valid(guider_ui.screen_11)) return;

    /* 清空列表容器 */
    lv_obj_clean(guider_ui.screen_11_list_1);

    /* 重新添加所有按钮 */
    for (int i = 0; i < NAV_COL_MAX; i++) {
        lv_obj_t *list_btn = lv_list_add_btn(guider_ui.screen_11_list_1, NULL, categories[i]);
        lv_obj_add_style(list_btn, &style_inf_list_btn_default, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(list_btn, &style_inf_list_btn_checked, LV_PART_MAIN | LV_STATE_CHECKED);

        /* 保存行号到用户数据 */
        lv_obj_set_user_data(list_btn, (void *)(intptr_t)i);

        /* 添加点击事件回调 */
        lv_obj_add_event_cb(list_btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);

           dbg_print("添加按钮: %s", categories[i]);

    }
}



#include "recommend_video_fetch.h" // 提供 VideoInfo_t, get_recommend_video_list

/* 外部全局 UI 对象（由 GUI Guider 生成） */
extern lv_ui guider_ui;


/**
 * @brief 视频列表按钮点击事件回调
 */
static void video_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    if (!btn) return;

    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    VideoInfo_t *videos = NULL;
    int count = get_recommend_video_list(&videos);

    if (idx >= 0 && idx < count && videos) {
        printf("点击视频: %s\n", videos[idx].title);
        printf("播放地址: %s\n", videos[idx].play_url);
        record_last_clicked_url(videos[idx].play_url);

        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_set_video_pos, 0, 0, NULL, NULL, NULL);
        ktv_cloud_order_resolved_t resolved;
        char current_json[8192];
        char songinfo_json[16384];
        
        ktv_cloud_order_resolve_first_play_param("16666666666",
                        "fbe29d501e94250e3442d14979913481",
                        0,   // 或 1
                        g_song_id,
                        videos[idx].play_url,
                        &resolved,
                        current_json,
                        sizeof(current_json),
                        songinfo_json,
                        sizeof(songinfo_json));

        ktv_player_ui_play(&resolved.play_param);
        /* TODO: 调用播放器或页面跳转 */
    } else {
        printf("点击了无效视频索引 %d\n", idx);
    }
}



static uint8_t g_video_list_refresh_pending = 0;

static lv_obj_t *get_video_list_obj(void)
{
    lv_obj_t *scr;
    lv_obj_t *child0;
    lv_obj_t *child2;
    lv_obj_t *list;

    scr = lv_scr_act();
    if (!lv_obj_is_valid(scr)) {
        return NULL;
    }

    child0 = lv_obj_get_child(scr, 0);
    if (!lv_obj_is_valid(child0)) {
        return NULL;
    }

    /* 取 child0 的第 3 个子对象 */
    child2 = lv_obj_get_child(child0, 2);
    if (!lv_obj_is_valid(child2)) {
        return NULL;
    }

    /* 列表是 child2 的第 1 个子对象 */
    list = lv_obj_get_child(child2, 0);
    if (!lv_obj_is_valid(list)) {
        return NULL;
    }

    return list;
}

static void lv_page_refresh_video_list_async_cb(void *user_data)
{
    lv_obj_t *list_obj;
    VideoInfo_t *videos = NULL;
    int count;
    int i;

    (void)user_data;
    g_video_list_refresh_pending = 0;

    list_obj = lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 0), 0), 2);
    if (list_obj == NULL) {
        printf("screen_8_1_list_1 无效\n");
        return;
    }

    lv_obj_clean(list_obj);

    count = get_recommend_video_list(&videos);
    if (count <= 0 || videos == NULL) {
        lv_obj_t *tip_btn;
        printf("视频列表为空，请先调用 fetch_video_list 下载数据\n");
        tip_btn = lv_list_add_btn(list_obj, NULL, "暂无视频，请稍后...");
        lv_obj_add_style(tip_btn, &style_inf_list_btn_default, LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }

    for (i = 0; i < count; i++) {
        char btn_text[128];
        lv_obj_t *btn;

        snprintf(btn_text, sizeof(btn_text), "%s", videos[i].title);

        btn = lv_list_add_btn(list_obj, NULL, btn_text);
        lv_obj_add_style(btn, &style_inf_list_btn_default, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(btn, &style_inf_list_btn_checked, LV_PART_MAIN | LV_STATE_CHECKED);

        lv_obj_set_user_data(btn, (void *)(intptr_t)i);
        lv_obj_add_event_cb(btn, video_btn_event_cb, LV_EVENT_CLICKED, NULL);
    }

    printf("视频列表刷新完成，共添加 %d 项\n", count);
}

void lv_page_refresh_video_list(void)
{
    if (g_video_list_refresh_pending) {
        return;
    }

    g_video_list_refresh_pending = 1;
    lv_async_call(lv_page_refresh_video_list_async_cb, NULL);
}