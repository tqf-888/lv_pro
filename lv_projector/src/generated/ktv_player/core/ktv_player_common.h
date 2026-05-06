#ifndef __KTV_PLAYER_COMMON_H__
#define __KTV_PLAYER_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "ktv.h"
/*
 * 统一返回值
 * 0 失败
 * 1 成功
 */
#define KTV_PLAYER_RET_FAIL                    (0)
#define KTV_PLAYER_RET_OK                      (1)

/*
 * 通用宏定义
 */
#define KTV_PLAYER_MAX_URL_LEN                 (512)
#define KTV_PLAYER_MAX_INSTANCE_COUNT          (6)

#define KTV_PLAYER_SCREEN_WIDTH                (800)
#define KTV_PLAYER_SCREEN_HEIGHT               (1280)

// #define KTV_PLAYER_SMALL_RECT_X                (400)
// #define KTV_PLAYER_SMALL_RECT_Y                (300)
// #define KTV_PLAYER_SMALL_RECT_W                (400)
// #define KTV_PLAYER_SMALL_RECT_H                (640)

#define KTV_PLAYER_PREPARE_TIMEOUT_MS          (10000)

#define KTV_INPUT_MONITOR_MAX_COUNT            (128)
#define KTV_INPUT_MONITOR_WAIT_TIMEOUT_MS      (3000)

/*
 * 统一日志
 * dbg_print 头文件你自己加
 */
#define KTV_PLAYER_LOGE(fmt, args...)          dbg_print("[KTV_PLAYER][E] " fmt, ##args)
#define KTV_PLAYER_LOGW(fmt, args...)          dbg_print("[KTV_PLAYER][W] " fmt, ##args)
#define KTV_PLAYER_LOGI(fmt, args...)          dbg_print("[KTV_PLAYER][I] " fmt, ##args)
#define KTV_PLAYER_LOGD(fmt, args...)          dbg_print("[KTV_PLAYER][D] " fmt, ##args)

#ifdef __cplusplus
}
#endif

#endif