#ifndef NETWORK_WIFI_FUNCTION_H
#define NETWORK_WIFI_FUNCTION_H

// 必须的基础头文件：定义bool类型和标准类型
#include <stdbool.h>   // 提供bool/true/false定义
#include <stddef.h>    // 提供NULL/const等基础定义

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// 功能：连接WIFI网络
// 参数：ssid：WIFI名称（SSID），password：WIFI密码
// 返回：true：连接成功或命令执行成功；false：连接失败
// 说明：此函数会启动wifi守护进程，设置为station模式，然后连接指定的WIFI
// =============================================================================
bool NetWork_WIFI_Connect(const char *ssid, const char *password);

// =============================================================================
// 功能：断开WIFI连接
// 参数：无
// 返回：true：断开成功；false：断开失败
// =============================================================================
bool NetWork_WIFI_Disconnect(void);

// =============================================================================
// 功能：检查WIFI连接状态
// 参数：无
// 返回：true：已连接；false：未连接
// =============================================================================
bool NetWork_WIFI_IsConnected(void);

// =============================================================================
// 功能：保存当前连接 WiFi 的 SSID 到持久文件
// 说明：写入 /usr/share/lv_projector/app_wifi_last_connected_ssid.txt
// =============================================================================
bool NetWork_WIFI_SaveConnectedSSID(const char *ssid);

// =============================================================================
// 功能：读取缓存的当前 WiFi SSID
// 说明：只读持久文件，不执行系统命令查询当前 WiFi。
// =============================================================================
bool NetWork_WIFI_GetConnectedSSID(char *ssid_buf, size_t ssid_buf_size);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_WIFI_FUNCTION_H
