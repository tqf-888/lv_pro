# page_nav_stack_home_register

## 新规则

保留 3 个接口：

```c
page_nav_register_home()
page_nav_push()
page_nav_back()
```

## 1. 注册桌面

只允许调用一次：

```c
page_nav_register_home("home",
                       &guider_ui.screen_7,
                       &guider_ui.screen_7_del,
                       setup_scr_screen_7);
```

特点：

- 不跳转
- 不调用 `setup_scr()`
- 不调用 `ui_load_scr_animation()`
- 只记录当前桌面
- 第二次调用返回 `PAGE_NAV_ERR_BUSY`

## 2. 正向进入页面

以后不要直接调用 `ui_load_scr_animation()`。

统一：

```c
page_nav_push("screen_8",
              &guider_ui.screen_8,
              &guider_ui.screen_8_del,
              setup_scr_screen_8);
```

内部固定：

```c
ui_load_scr_animation(&guider_ui,
                      new_scr,
                      *new_scr_del,
                      old->scr_del,
                      setup_scr,
                      LV_SCR_LOAD_ANIM_NONE,
                      0,
                      0,
                      0,
                      0);
```

## 3. 返回

```c
page_nav_back();
```

内部也固定无动画：

```c
LV_SCR_LOAD_ANIM_NONE, 0, 0, 0, 0
```

## 4. 注意

`setup_scr_xxx()` 里不要再主动调用 `page_nav_push()`。

如果旧代码里还有这种残留调用，当前文件用 `g_page_nav_loading` 忽略它，避免重复入栈。
