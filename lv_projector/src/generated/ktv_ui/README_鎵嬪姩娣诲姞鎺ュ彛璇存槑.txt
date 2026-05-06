这两个模块都是“纯内存头插列表”，不请求 json，不依赖网络。
最新添加的数据永远插到第 0 行，旧数据顺次下移。
样式、字体、abcdef 图标位语义均按 rich_song / song 风格。

核心接口：
1) *_adapter_add_media() : 传 media_id/title/artist/is_vip/has_mv，模块自动生成 rich_song 图标位。
2) *_adapter_add_item_head() : 直接头插完整业务 item，你也可以自己塞 text_a~text_f。
3) *_adapter_clear() : 清空列表，恢复“暂无xxx”占位。

注意：这些 add/clear 接口应在 LVGL/UI 线程调用。
