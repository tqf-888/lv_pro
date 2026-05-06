# lv_linux_folder_list_component_v4

LVGL + Linux 文件浏览组件。

## 本版改动

- 固定每页显示 8 行。
- 字体统一使用 `lv_font_Regular_20`。
- 按 1280x800 屏幕重新调整布局：顶部路径栏、8 行列表、底部分页栏。
- 点击目录进入下一级，并打印日志。
- 点击图片文件后，优先 `fopen + fread` 读入内存，然后：

```c
lv_img_set_src(img, &lv_img_dsc_t);
```

避免 LVGL 不认识 `/mnt/SDCARD/...` 裸 Linux 路径。

- 点击视频文件只调用 `video_cb`，由外部跳转播放器。
- 销毁组件时自动关闭并释放图片预览。

## 注意

工程里需要有字体符号：

```c
lv_font_Regular_20
```

组件源文件里已经声明：

```c
LV_FONT_DECLARE(lv_font_Regular_20);
```

如果你的字体文件没有加入编译，会链接失败。
