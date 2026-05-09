#ifndef KTV_H
#define KTV_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// 这里的长度定义与 md5_impl.h 中的宏保持一致或直接使用
#define MAX_URL_LEN     2048
#define MAX_PARAM_NUM   64

/* ================= 宏定义 ================= */
#define KTV_DEBUG 1
/* ================= 超酷调试打印宏（带空行+彩色+分隔线） ================= */
#if KTV_DEBUG
#define dbg_print(fmt, ...) do { \
    /* 1. 强制刷新缓冲区，避免其他日志插队 */ \
    fflush(stderr); \
    /* 3. 日志内容（黄字，仅保留KTV标识） */ \
    fprintf(stderr, "\033[33m[KTV] " fmt "\033[0m\n", ##__VA_ARGS__); \
    /* 5. 强制刷新，确保日志完整输出 */ \
    fflush(stderr); \
} while(0)
#else
#define dbg_print(fmt, ...) ((void)0)
#endif

/************************* KTV设备核心配置（统一KTV_前缀） *************************/
// #define KTV_SIGN_TOKEN_URL             "http://192.168.1.4:8080/sdkv2/api/channel/getApiToken"
#define KTV_SIGN_TOKEN_URL             "https://tuoge.djyos.com/sdkv2/api/channel/getApiToken"
// 1. 默认参数（环境变量不存在时使用）
#define KTV_DEFAULT_CHANNEL_VERSION    "1.0.0"
#define KTV_DEFAULT_CHANNEL_ID         "326"
#define KTV_DEFAULT_DEVICE_ID          "ffffffff-c40a-af39-ffff-ffff8bdcb6e8"
#define KTV_DEFAULT_MAC                "C6:F5:66:8D:F9:F6"


// 2. 环境变量名（对应系统环境变量）
#define KTV_ENV_CHANNEL_VERSION        "KTV_CHANNEL_VERSION"
#define KTV_ENV_CHANNEL_ID             "KTV_CHANNEL_ID"
#define KTV_ENV_DEVICE_ID              "KTV_DEVICE_ID"
#define KTV_ENV_MAC                    "KTV_MAC_ADDR"

// 3. URL相关配置
#define KTV_DOMAIN_URL                 "https://t-api.jzurl.cn"
#define KTV_PARAMS_FORMAT              "&channel_version=%s&default_channelid=%s&default_deviceid=%s&default_mac=%s&request_time=%s"

// 4.1_全部歌曲分类列表
#define KTV_4_1_SONG_CLASS_LIST "/sdkv2/song/classList"

// 4.2_搜索歌曲
#define KTV_4_2_SEARCH_SONG "/sdkv2/song/search?name=%s&artist_name=%s&artist_name_strict=%d&pinyin=%s&tag_id=%d&artist_id=%d&empty_search=%d&default_data=%d&abbr_pinyin=%s&page=%d&pagesize=%d"

// 4.3_歌手分类接口
#define KTV_4_3_ARTIST_CLASS_LIST "/sdkv2/artist/classList"

// 4.4_搜索歌手
#define KTV_4_4_SEARCH_ARTIST "/sdkv2/artist/search?name=%s&pinyin=%s&tag_id=%d&empty_search=%d&default_data=%d&abbr_pinyin=%s&page=%d&pagesize=%d"

// 4.6_热门歌手列表
#define KTV_4_6_HOT_ARTIST_LIST "/sdkv2/artist/hotList"

// 4.7_全部歌单列表
#define KTV_4_7_ALL_SONG_SHEET_LIST "/sdkv2/songsheet/allList?pagesize=%d&page=%d"

// 4.8_推荐歌单列表
#define KTV_4_8_RECOMMEND_SONG_SHEET_LIST "/sdkv2/songsheet/recommendList"

// 4.9_模块/儿童/长辈/会员专享歌单
#define KTV_4_9_MODULE_SONG_SHEET_LIST "/sdkv2/songsheet/moduleList?type=%d"

// 4.10_歌单歌曲列表
#define KTV_4_10_SONG_SHEET_SONG_LIST "/sdkv2/songsheet/getSongList?ssid=%d&pagesize=%d&page=%d"

// 4.11_推荐歌曲分类列表
#define KTV_4_11_RECOMMEND_SONG_TAG_LIST "/sdkv2/song/tagList?type=%d"

// 4.12_每日会员用户点唱榜
#define KTV_4_12_VIP_RANKING_SONG_LIST "/sdkv2/ranking/vipRankingSongList?rank_id=%d&page=%d&pagesize=%d"

// 4.13_每日点唱TOP100
#define KTV_4_13_DAILY_TOP100_SONGS "/sdkv2/song/hotSongs"

// 4.14_每日榜单列表
#define KTV_4_14_RANKING_LIST "/sdkv2/ranking/rankingList"

// 4.15_榜单歌曲列表
#define KTV_4_15_RANKING_SONG_LIST "/sdkv2/ranking/rankingSongList?ranking_id=%d&page=%d&pagesize=%d"

// 4.16_改词唱分类菜单列表
#define KTV_4_16_FUN_TAG_LIST "/sdkv2/funsing/funTagList"

// 4.17_改词唱分类菜单歌曲列表
#define KTV_4_17_FUN_TAG_SONG_LIST "/sdkv2/funsing/funTagSongList?tag_id=%d"

// 4.18_改词唱歌曲歌词列表
#define KTV_4_18_FUN_SONG_LYRIC_LIST "/sdkv2/funsing/funSongZrceList?tag_id=%d&song_id=%d"

// 4.19_加速唱倍速菜单列表
#define KTV_4_19_FUN_SPEED_LIST "/sdkv2/funsing/funSingList"

// 4.20_加速唱倍速菜单歌曲列表
#define KTV_4_20_FUN_SPEED_SONG_LIST "/sdkv2/funsing/funSongList?page=%d&pagesize=%d&double_id=%d"


// 4.30 获取歌曲详情
#define KTV_4_30_GET_SONG_INFO "/sdkv2/api/song/getSongInfo?songid=%d&user_token=%s&channel_id=%d"

// 4.34.1 发送验证码
#define KTV_4_34_1_SEND_CODE "/sdkv2/user/sendCode?phone=%s"

// 4.34.2 用户手机号注册登录接口
#define KTV_4_34_2_CHECK_ACCOUNT_BY_PHONE "/sdkv2/user/checkAccountByPhone?tvid=%d&phone=%s&code=%s"

#define SONG_FAVORITE_PAGE "https://tuoge.djyos.com/sys/song/favorite/page?page=%d&limit=50&phone=%s"




#define KTV_474575SEND_CODE "/sdkv2/cdkey/exchangeCode?cdkey_no=7892043293912&default_token=fbe29d501e94250e3442d14979913481"






#define IGNORE_NUM 0x12345

int ktv_build_full_url(char *base_url, size_t buf_size, const char *sign_token,  const char *key) ;
int ktv_build_base_url(const char* api_format, char* final_url, int url_buf_size, ...);
#endif
