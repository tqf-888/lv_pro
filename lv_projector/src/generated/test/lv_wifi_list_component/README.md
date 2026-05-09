# lv_wifi_list_component_fixed

本版只保留原来的对外接口，不新增任何 close 相关接口。

修复点：

1. `app_wifi_list_close()` 内部自己判断后台扫描状态。
2. 如果扫描线程还没结束，`app_wifi_list_close()` 不 destroy 页面，只标记 `close_pending`。
3. 扫描线程返回后，统一清理 `inflight/result_ready/ret/count`。
4. LVGL timer 发现 `close_pending && !inflight` 后，再真正 destroy 页面。
5. 没有超时解锁。
6. 不杀扫描线程。
7. 不使用系统命令查询当前 WiFi。
8. 当前连接 SSID 只从 `/usr/share/lv_projector/app_wifi_last_connected_ssid.txt` 读取。
9. 每页固定 7 个可见 WiFi，空 SSID 不进入分页。

对外仍然只用原来的：

```c
app_wifi_list_open(parent);
app_wifi_list_close();
app_wifi_list_refresh();
app_wifi_list_refresh_connected();
app_wifi_list_set_password(password);
```
