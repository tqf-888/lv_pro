/**
 * @Title: page_ui.h
 * @Description: 页面UI界面接口（原WiFi UI功能迁移）
 * @author Jeremy
 * @date 2025-01-29
 */

#ifndef PAGE_UI_H
#define PAGE_UI_H

/**
 * @brief 刷新整个页面列表界面（原WiFi列表刷新）
 * @param info 页面数据信息（原WiFi扫描结果信息）
 */
void lv_page_reflash_all();
void lv_page_list_btn_style_create();

#endif /* PAGE_UI_H */