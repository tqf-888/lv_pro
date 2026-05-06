# 艺人列表 2x4 布局 + 解码性能修复总结

记录本轮针对 `ktv_ui/artist` 列表页 + LVGL 硬解 JPEG 链路的所有改动。

---

## 一、总览

本轮共解决 5 类问题：

| # | 问题 | 表现 | 根因 |
|---|------|------|------|
| 1 | 2x4 布局 + 文字图文交合 | 之前没有这种样式 | 新需求 |
| 2 | 视口尾部 cell 永远显示不了图 | 第 7、8 个 cell 是空的；翻页后第 6+ 项也空 | `batch_size=6` 但视口 8 个，probe 队列覆盖不全 |
| 3 | 翻几页内存炸（OOM） | `cedarc CdcDmaheapAllocFd errno=12` / `Out of memory` | 硬解 `JpegDecoderGetFrame` 每次 `CreateVideoDecoder` 覆盖旧指针，旧 ION SBM 缓冲泄漏 |
| 4 | 滑动卡顿（一页 8 张图全部同步解码阻塞 UI） | 滑一下半天才出图 | (a) 之前为修 OOM 在 close 立即销毁 → 反复 ION 抖动；(b) 滑动期间也触发解码 |
| 5 | 没图时露出空 "Loading…" 半透条 | 名字条单独飘在透明背景上很丑 | 没有"占位 + 名字解耦"的状态机 |

---

## 二、改动清单

### 业务侧（`lv_projector/src/generated/ktv_ui/artist/`）

| 文件 | 改动类型 | 说明 |
|------|---------|------|
| `style/lv_artist_view_style.h` | 修改 | `lv_artist_row_style_t` 新增 `name_h` / `name_bg_color` / `name_bg_opa` |
| `style/lv_artist_style_2x4.c` | 新增 | 2 行 × 4 列、cell = 图片 = 160×210、名字条贴底半透明 |
| `style/lv_artist_style_2x4.h` | 新增 | 头文件 |
| `render/lv_renderer_artist.c` | 修改 | name_bar 容器（图文交合）+ bind_cell 三段式状态机 |
| `app/artist_page_demo.c` | 修改 | 注册 2x4、`batch_size = visible_rows × visible_cols` |
| `adapter/lv_artist_adapter.h` | 修改 | probe 增加 `path_applied` 字段 |
| `adapter/lv_artist_adapter.c` | 修改 | ① batch_size 兜底 ② 滑动期间暂停 path apply ③ 250ms settle |

### 底层 LVGL（`lvgl/src/extra/libs/sjpg/`）

| 文件 | 改动类型 | 说明 |
|------|---------|------|
| `sunxijpgd.h` | 修改 | 声明 `JpegDecoderReleaseSession` |
| `sunxijpgd.c` | 修改 | ① GetFrame 入口防御性 destroy 旧 VideoDecoder ② 新增 ReleaseSession 实现 |
| `lv_sjpg.c` | 修改 | ① 移除所有 `fopen("/mnt/UDISK/*.log")` 调试日志 ② 增加错误日志（带文件名） ③ `jpegdecoder == NULL` 防御 ④ decoder_close 改回 lazy release |

---

## 三、每项改动的原理与代码位置

### 1) 2x4 布局 + 图文交合

**做法：** 在 `lv_artist_row_style_t` 加 3 个字段，渲染器按需多包一层 `name_bar` 容器，把 label 居中放进去。

```c
/* lv_artist_view_style.h */
lv_coord_t name_h;          /* > 0 启用名字条容器（图文交合） */
lv_color_t name_bg_color;
lv_opa_t   name_bg_opa;     /* 仅背景半透明，文字始终 LV_OPA_COVER */
```

```c
/* lv_renderer_artist.c artist_create_cell */
if (s->name_h > 0) {
    cell->name_bar = lv_obj_create(cell->root);
    /* ...半透明背景 + 整体 opa=COVER 防止"传染"文字... */
    cell->lbl_name = lv_label_create(cell->name_bar);
    lv_obj_center(cell->lbl_name);
}
```

**向后兼容：** 老 style（`name_h = 0`）走原"label 直接挂 root，无背景"路径，不动。

**2x4 关键参数：**

| 项 | 值 | 含义 |
|----|-----|------|
| `cell_width / cell_height` | 160 × 210 | cell 完全等于图片大小 |
| `gap_x / gap_y` | 16 / 16 | cell 间距 |
| `viewport_width / height` | 688 / 436 | 4*160+3*16 / 2*210+1*16 |
| `name_h` | 32 | 名字条高度 |
| `name_bg_opa` | LV_OPA_60 | 60% 不透明黑 |
| `avatar_bg_color` | 0x2A2A2A | 图片没到位时的占位深灰 |

### 2) batch_size 兜底（修视口尾部 cell 空白）

**根因：** `batch_size=6` 写死，2x4 视口 8 个 cell。`probe_slots` 大小 = batch_size = 6，所以 cell 6/7（即 slot index 6, 7）永远没人 probe → 拿不到 path。

**双层防御：**

```c
/* artist_page_demo.c */
uint32_t batch_size = g_lv_artist_style_2x4.visible_rows *
                      g_lv_artist_style_2x4.visible_cols;
```

```c
/* lv_artist_adapter.c lv_artist_adapter_start */
{
    uint32_t visible = artist_visible_count(adapter);
    adapter->ui_batch_size = (json_page_size >= visible) ? json_page_size : visible;
}
adapter->probe_slot_count = adapter->ui_batch_size;
```

业务层 + adapter 各兜一层，以后再换 5x2、3x4 也不会踩同样的坑。

### 3) 修 cedarc ION DMA 泄漏（OOM）

**根因：**

```c
/* sunxijpgd.c JpegDecoderGetFrame 原代码 */
p->mVideoDecoder = CreateVideoDecoder();   /* 每次调用都直接覆盖旧指针 */
```

旧 `mVideoDecoder` 持有的 SBM 缓冲（≈ `mSrcBufLen + 2MB` ION DMA 堆）从此不可达 = 泄漏。100 张约累积 200MB+ → CMA 池耗尽。

**修复：入口防御性销毁 + 新增 release 接口**

```c
/* sunxijpgd.c JpegDecoderGetFrame 入口 */
if (p->mVideoDecoder != NULL) {
    DestroyVideoDecoder(p->mVideoDecoder);
    p->mVideoDecoder = NULL;
}
p->mVideoDecoder = CreateVideoDecoder();
```

```c
/* sunxijpgd.c 新增 */
void JpegDecoderReleaseSession(JpegDecoder* v) {
    JpegDecoderContext* p = (JpegDecoderContext*)v;
    if (p && p->mVideoDecoder) {
        DestroyVideoDecoder(p->mVideoDecoder);
        p->mVideoDecoder = NULL;
    }
}
```

### 4) 滑动卡顿：lazy release + 滑动期间暂停解码

#### 4.1 lazy release（让"销毁→重建"紧挨着发生）

**之前的错误修复：** `decoder_close` 立刻 `JpegDecoderReleaseSession()` 释放 ION → 下张图 `decoder_open` 又重新分配 → 反复 ION 抖动 → 滑一下卡半天。

**改回 lazy release：**

```c
/* lv_sjpg.c decoder_close */
#ifdef USE_HARDWARE_JPEGDECODER
    /* close 啥都不做，让 VideoDecoder + ION 缓冲挂着，
     * 下张图 GetFrame 入口的防御性清理来回收。 */
    return;
#endif
```

效果：
- 同时刻**仍然只持有 1 份** ION（不泄漏不 OOM）
- close 是 noop（不抖动）
- Destroy 和 Create 紧挨着发生，CMA 复用刚释放的页，kernel reclaim 路径最短

#### 4.2 滑动期间暂停 path apply

**判定 = 只看时间戳：**

```c
#define ARTIST_DECODE_SETTLE_MS 250U   /* LVGL inertial 收敛 ~200~300ms */

static uint8_t artist_is_scrolling(const lv_artist_adapter_t *adapter) {
    if (!adapter || adapter->last_scroll_tick == 0U) return 0U;
    return lv_tick_elaps(adapter->last_scroll_tick) < ARTIST_DECODE_SETTLE_MS;
}
```

**ui_timer 主循环行为表：**

| 操作 | 滑动期间 | 静止时 |
|------|---------|--------|
| `demo_ui_scroll_range`（通知 image_manager 预下载） | ✅ 仍触发 | ✅ |
| `artist_apply_meta`（写名字） | ✅ **立刻应用**（用户能看到名字） | ✅ |
| `artist_apply_image_path`（写路径 → 触发 LVGL 解码） | ❌ **不应用** | ✅ apply 全部 path_ready 项 |
| `query_count` 累计 | ✅ | ✅ |

**probe 三态字段：** `path_ready`（image_manager 已返回路径）+ `path_applied`（已写入 catalog 触发 LVGL 解码），后者只在静止时置 1。

### 5) 没图时灰框占位 + 名字条独立显示

**bind_cell 三段式状态机：**

```c
/* 1. 元数据未就绪 → cell 完全留空 */
if (!biz.ready) {
    lv_obj_add_flag(cell->name_bar, LV_OBJ_FLAG_HIDDEN);
    artist_clear_image(cell);
    return;
}

/* 2. 名字 ready → 立即显示名字条（无论图片状态） */
lv_obj_clear_flag(cell->name_bar, LV_OBJ_FLAG_HIDDEN);
set_name_label(cell->lbl_name, biz.name, s);

/* 3. 图片 ready → 用图片覆盖灰框 */
if (biz.avatar_ready && ...) {
    artist_apply_cover_image(cell, ...);
} else {
    artist_clear_image(cell);   /* avatar_bg 的深灰作为占位 */
}
```

**视觉对照：**

| 状态 | name_bar | avatar 区域 |
|------|---------|-------------|
| `biz.ready == 0` | 隐藏 | 灰框 |
| 名字到了 + 图没到（含滑动期间） | 显示名字 | 灰框 0x2A2A2A 占位 |
| 全部就绪 | 显示名字 | 图片覆盖灰框 |

### 6) 调试日志清理

清掉 `lv_sjpg.c` 里 6 处 `fopen("/mnt/UDISK/*.log")` 的常驻调试日志。每次 `decoder_open` / `decoder_info` 都会 fopen / fwrite / fclose 小文件，会引发 FAT-fs `Invalid FSINFO` 之类的告警，并占 IO。

现在只保留 4 条 `LV_LOG_WARN` 错误日志（受 `LV_USE_LOG` 全局开关控制，正常情况下完全静默）：

| 触发 | 输出 |
|------|------|
| `jpegdecoder` 单例创建失败 | `create hardware jpegdecoder failed` |
| `jpegdecoder == NULL` 但被调用 | `hw jpegdecoder not initialized, file=...` |
| 读文件失败 | `sunxijpgd load file fail: <文件名>` |
| 解码失败 | `JpegDecoderGetFrame fail (file=..., size=..., scale=...)` |

---

## 四、调参速查表

### 视觉

| 想调什么 | 改哪里 | 当前值 |
|---------|--------|------|
| 间距 | `lv_artist_style_2x4.c` `gap_x/gap_y` | 16 |
| 名字条高度 | `lv_artist_style_2x4.c` `name_h` | 32 |
| 名字条透明度 | `lv_artist_style_2x4.c` `name_bg_opa` | LV_OPA_60 |
| 名字条文字大小 | `lv_artist_style_2x4.c` `name_font` | `lv_font_Regular_20` |
| 占位灰框颜色 | `lv_artist_style_2x4.c` `avatar_bg_color` | `0x2A2A2A` |
| 占位灰框可见度 | 上面那行换更亮的灰，如 `0x3A3A3A` 或 `0x5A5A5A` | — |

### 性能

| 想调什么 | 改哪里 | 当前值 | 说明 |
|---------|--------|------|------|
| 滑动停手到出图的延迟 | `lv_artist_adapter.c` `ARTIST_DECODE_SETTLE_MS` | 250 | 100~400 之间合理；LVGL inertial 约 200~300ms |
| ui_timer 周期 | `lv_artist_adapter.c:lv_timer_create` 第二参 | 200 | 不建议低于 100，会增加 CPU 占用 |
| 硬解缩放比 | `lv_sjpg.c` `LV_SJPG_HW_SCALE_RATIO` | `SCALE_DOWN_2` | 500×500 源图配 160×210 cell 时 `_2`(250×250) 最佳；`_4`(125×125) 会被 LVGL 上采样导致糊 |
| 单页 batch_size | `artist_page_demo.c` | `rows*cols=8` | 必须 ≥ 视口 cell 数 |

### 调参常见组合

| 现象 | 试 |
|------|----|
| 滑动尾巴还有抖 | `ARTIST_DECODE_SETTLE_MS = 350` |
| 停下后等出图太久 | `ARTIST_DECODE_SETTLE_MS = 150` + ui_timer 周期 100 |
| 用户觉得"灰框 loading 感"太弱 | `avatar_bg_color = 0x3A3A3A` |
| 想完全静默 LV_LOG_WARN | `lv_conf.h: #define LV_USE_LOG 0` |

---

## 五、运行时序（典型场景）

### A. 首屏

```
t=0     page open / artist_prefetch_page_window 触发
t=0     last_scroll_tick == 0 → not scrolling → 允许 apply
t≈Tdl   ui_timer 200ms tick：path_ready=1，立即 apply → bind_cell → cedarc 解码
t≈Tdl+解码  出图
```

不受 SETTLE 影响；下载完立刻显示。

### B. 滑动期间

```
用户拖动 → top_index 变化 → last_scroll_tick = now
ui_timer 周期 200ms：
    scrolling=true
    ├─ apply_meta（名字立即出来）
    └─ ❌ 不 apply path（path 缓存在 probe）
画面：灰框 + 名字浮在底部
```

### C. 停止后

```
t=0     用户最后一次拖动 → last_scroll_tick=now
t=200   timer：lv_tick_elaps=200 < 250 → scrolling=true，仍不 apply
t=400   timer：lv_tick_elaps=400 ≥ 250 → scrolling=false
        遍历 probe：所有 path_ready && !path_applied 一起 apply
        bind_cell × 8 → cedarc 8 张连续解码 ≈ 250~500ms
t≈900   全屏图片出齐
```

最坏路径：停手到全屏出图 ≈ 600~900ms（200ms timer + 250ms SETTLE + 解码）。

### D. 滑→停→又滑（用户犹豫）

```
t=0     滑 → last_scroll_tick=now
t=300   停手
t=400   timer 检测到 not scrolling，开始 apply 第 1 张
t=500   用户又拖了一下 → last_scroll_tick=t=500
t=600   timer 检测到 scrolling 又变 true，但本张的解码已在跑（cedarc 同步阻塞）
        → 这一帧仍卡，从下一张开始才被暂停
```

会有 1 张图"漏出来"卡到主线程，但只卡这一张（不再是 8 张）。

---

## 六、关键不变量

设计上保证下面这些不变量始终成立。如果以后改这部分代码，务必维持：

1. **同时刻最多持有 1 份 cedarc ION SBM 缓冲。** 由 `JpegDecoderGetFrame` 入口防御性销毁 + lazy release 联合保证。
2. **probe_slot_count ≥ 视口 cell 数（rows × cols）。** 由 `lv_artist_adapter_start` 的兜底保证。
3. **probe 索引（0-based） + 1 = image_manager 用的 slot_id（1-based）。** 这套偏移在 `demo_ui_get_image_path(slot+1, …)` 一次性约定，全链路一致。
4. **`name_h > 0` 必走 name_bar 容器路径，文字在容器中 `lv_obj_center`；`name_h == 0` 必走老路径（label 直接挂 root，无背景）。** 兼容性约束。
5. **滑动期间 `path_applied` 不被置 1。** 静止后 timer tick 才 apply，避免 cedarc 在主线程同步解码阻塞。

---

## 七、已知遗留 / 进一步优化方向

### 1. cedarc 同步解码仍占主线程

当前架构中，`lv_img_set_src` → `decoder_open` → `JpegDecoderGetFrame` 全部在 LVGL 主线程同步执行。500×500 jpeg 单张耗时 ~30~80ms，一屏 8 张连续解码 ~250~500ms 期间主线程不可响应。

**进一步优化（方案 B）：** 后台 pthread 异步预解码 + 解码缓存（按 `content_id` 索引 LRU），renderer 用 `lv_img_dsc_t*` 而不是 file path。工作量约 500~800 行新代码，需要先确认 cedarc 是否线程安全。

### 2. 硬解器单例的隐藏 use-after-free 风险

硬解器 `jpegdecoder` 是单例，`mImgFrame.mRGBData` 也是单例。每次 `GetFrame` 入口会 `lv_mem_free` 上一次的 RGBData。如果 LVGL 同时缓存多个 dsc（`LV_IMG_CACHE_DEF_SIZE > 1`），老 dsc 的 `img_data` 会变成野指针。

当前没复现，因为 image cache 实测每次只活一份；如果以后开了 cache 出现花屏 / 串图，要把 RGBData 改成"每个 session 独立分配，由 LVGL `decoder_close` 释放"。

### 3. `avatar_placeholder` label 当前是个空对象

每个 cell 多一个空 lv_label（约 200B），8 个 cell 共 ~1.6KB，不是性能瓶颈。可在确认 UI 不再需要回退到 placeholder 文字时删除。

### 4. ui_timer 200ms 周期

不算高频，但每次 tick 跑 8 个 probe + 多次 lv_tick_elaps + memcpy。当前 CPU 占用很低，未来如果发现 timer 自身有开销，可以考虑：
- 仅在 `vlist_top_index` 改变时 dispatch，而不是固定周期
- 用 LVGL event 替代 timer

---

## 八、文件级 diff 索引（按依赖顺序）

```
lvgl/src/extra/libs/sjpg/sunxijpgd.h          [+] JpegDecoderReleaseSession 声明
lvgl/src/extra/libs/sjpg/sunxijpgd.c          [+] GetFrame 入口防御 + ReleaseSession 实现
lvgl/src/extra/libs/sjpg/lv_sjpg.c            [-] fopen 调试日志 [↻] decoder_close 改 lazy release [+] NULL 防御 + 错误日志带文件名
                                              ↑ 底层
                                              ─────────────────────────────────
                                              ↓ 业务
ktv_ui/artist/style/lv_artist_view_style.h    [+] name_h, name_bg_color, name_bg_opa
ktv_ui/artist/style/lv_artist_style_2x4.{c,h} [新文件] 2 行 × 4 列布局
ktv_ui/artist/render/lv_renderer_artist.c     [+] name_bar 容器 [+] bind_cell 三段式状态机
ktv_ui/artist/adapter/lv_artist_adapter.h     [+] path_applied
ktv_ui/artist/adapter/lv_artist_adapter.c     [+] ARTIST_DECODE_SETTLE_MS [+] artist_is_scrolling
                                              [↻] ui_timer 主循环加滑动判定 [+] batch_size 兜底
ktv_ui/artist/app/artist_page_demo.c          [↻] 注册 2x4 [↻] batch_size = visible 大小
```

