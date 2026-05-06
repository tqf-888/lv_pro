# lv_wifi_list_component_v14

修复点：

1. 不再只依赖 `iwgetid -r` / `wpa_cli status`。
2. 新增 `NetWork_WIFI_GetConnectedSSID()`。
3. 当前连接 SSID 查询顺序：
   - `iwgetid -r`
   - `wpa_cli -i wlan0 status`
   - `wifi -s` 输出解析
   - `wifi -g` 输出解析
   - `wifi -S` 输出解析
4. 适配你板子的日志形式：
   - `wpa state is completed`
   - BSSID
   - freq
   - SSID
   - network id
   - CCMP/WPA2/IP/RSSI
5. 连接成功显示：`连接成功xxx`
6. 开机自动连接后，打开界面会显示：`已连接xxx`

直接替换这些文件：

```text
lv_wifi_list_component.c
lv_wifi_list_component.h
lv_wifi_list_demo.c
lv_wifi_list_demo.h
NetWork_WIFI_Function.c
NetWork_WIFI_Function.h
```

如果你的工程里 `NetWork_WIFI_Function.c/.h` 已经在别处维护，就只合并新增的 `NetWork_WIFI_GetConnectedSSID()` 声明和实现。
