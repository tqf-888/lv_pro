#ifndef KTV_FETCH_H
#define KTV_FETCH_H

#include <stdio.h>

/* ==================== 函数声明 ==================== */
/**
 * 4.8 获取推荐歌单列表（异步下载）
 * @param page 页码（从0或1开始，由具体API决定）
 */
void fetch_recommend_song_sheet_list(int page);

/**
 * 4.9 获取模块歌单列表（异步下载）
 * @param page 页码
 */
void fetch_module_song_sheet_list(int page);

/**
 * 4.10 根据歌单获取歌曲列表（异步下载）
 * @param page 页码
 */
void fetch_song_list_by_songsheet(int page);
void fetch_recommend_taglist(int page);
void fetch_vip_rank_song_list(int page);
void fetch_4_4_artist_search(int page);
void fetch_top_100_songs_4_13(int page);
void fetch_rankingList_4_14(int page);
void fetch_song_by_rank_list_4_15(int page);
void fetch_fun_taglist_4_16(int page);
void fetch_4_17(int page);

#endif /* KTV_FETCH_H */