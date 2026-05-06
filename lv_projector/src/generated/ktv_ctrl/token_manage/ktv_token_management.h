#ifndef KTV_TOKEN_MANAGEMENT_H
#define KTV_TOKEN_MANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief 启动时间线程
 *
 * 线程启动后会立即校准一次时间，之后每小时校准一次，
 * 并且每分钟打印一次当前估算时间日志。
 *
 * @return 0 成功，-1 失败
 */
int ktv_time_thread_start(void);

/**
 * @brief 停止时间线程
 */
void ktv_time_thread_stop(void);

/**
 * @brief 获取当前估算时间戳，单位秒
 *
 * 优先使用服务器校准时间 + CLOCK_MONOTONIC 偏移，
 * 未校准时回退系统 CLOCK_REALTIME。
 */
uint32_t ktv_get_current_time_sec(void);
void ktv_time_refresh_label_async(void);

/**
 * @brief 获取当前有效的 API Token（自动处理时间同步、过期刷新）
 *
 * 失败回退策略：
 * - 网络获取失败时，返回“内存缓存/断电持久化缓存”中的值
 * - 若从未获取成功过，返回空字符串（不会返回 NULL，避免调用方拼 URL 时崩溃）
 *
 * @return 静态存储的 token 字符串（永不为 NULL）
 */
const char* ktv_get_token(void);

#ifdef __cplusplus
}
#endif

#endif /* KTV_TOKEN_MANAGEMENT_H */
