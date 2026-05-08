# lv_wifi_list_component_fixed

本版修复：打开页面没有扫描动作的问题。

规则：

1. `app_wifi_list_open()` 每次调用都会触发一次后台扫描。
2. 如果 WiFi 页面已经创建过，再次打开也会重新扫一次，不再只是显示旧页面。
3. 不使用这些命令查询当前连接 WiFi，避免卡 UI：
   - `iwgetid -r`
   - `iw dev wlan0 link`
   - `iwconfig wlan0`
   - `wpa_cli -i wlan0 status`
   - `wifi -s / wifi -g / wifi -S`
4. 当前连接 SSID 只读取：
   - `/usr/share/lv_projector/app_wifi_last_connected_ssid.txt`
5. 每页固定 7 个可见 WiFi，空 SSID 不进入分页。

替换这些文件：

```text
lv_wifi_list_component.c
lv_wifi_list_component.h
lv_wifi_list_demo.c
lv_wifi_list_demo.h
NetWork_WIFI_Function.c
NetWork_WIFI_Function.h
```
