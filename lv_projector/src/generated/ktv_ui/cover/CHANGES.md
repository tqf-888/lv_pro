# cover 封面业务新增说明

## 目标

新增一个和 artist 分层一致的独立 cover 业务，不改原 artist：

- JSON 地址固定为 `http://media.djyos.com/video_categories.json`
- 当前位置 `category_pos = -1` 表示全部分类
- 当前位置 `category_pos = 0/1/2...` 表示 JSON 顶层数组里的第几个分类
- 每个视频 item 使用本地封面池：`/usr/share/lv_projector/pic` 目录下最多扫描 20 张图片
- 每个 cell 显示：上方封面图片，下方 description；description 为空时显示 title
- 点击 cell 后通过弱回调 `app_cover_on_video_clicked(video_id, title, play_url, description)` 交给外部播放逻辑

## 对外接口

```c
void app_cover_set_category_position(int category_pos);
void app_open_cover_page(lv_obj_t *parent);
void app_close_cover_page(void);
void app_reset_cover_page_to_page0(uint32_t total_count);

void app_cover_on_video_clicked(uint32_t video_id,
                                const char *title,
                                const char *play_url,
                                const char *description);
```

## 使用例子

```c
/* 显示全部 */
app_cover_set_category_position(-1);
app_open_cover_page(parent);

/* 只显示第 0 个分类：豪迈励志 */
app_cover_set_category_position(0);
app_reset_cover_page_to_page0(300);

/* 只显示第 1 个分类：古风国风 */
app_cover_set_category_position(1);
app_reset_cover_page_to_page0(300);
```

## 文件说明

- `cover_media_loader.*`：下载并解析 `video_categories.json`，按分类过滤，保存视频 meta，分配本地随机封面
- `cover_media_loader_demo.*`：把 cover 业务接到原来的 `image_manager`
- `lv_cover_adapter.*`：和 artist adapter 一样，负责 vlist、滚动预取、slot 轮询
- `lv_cover_catalog.*`：封面业务 catalog
- `lv_renderer_cover.*`：封面 + description 渲染器
- `cover_page_demo.*`：页面打开/关闭/重置接口
- `lv_cover_style_2x4.*`：2x4 样式，图片在上，description 在下

## 注意

`image_manager.c/h` 继续复用原 artist 里的，不要重复编译第二份 `image_manager.c`，否则会有重复符号。
