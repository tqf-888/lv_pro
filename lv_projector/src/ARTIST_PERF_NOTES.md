# 歌手页加载性能优化记录

> 目标：解决歌手页（`ktv_ui/artist`，2x4 = 8 张可见）滑到下一页时图片加载约 2 秒的卡顿。
>
> 烧录验证锚点（串口必现）：
>
> ```
> [artist] BUILD MARK v4: probe-x2 + lv_img_cache=16 + sjpg-detach + http-prio + json-prio
> ```
>
> 看不到 v4 = 老固件没烧上去。

---

## 整体优化思路

性能问题的本质是**三件事串行卡住了**：

1. UI 层只在用户已经滑到下一页时才 `lv_img_set_src` → 解码触发延迟。
2. 解码本身慢（硬件 JPG ~50-100 ms / 张），且 LVGL 没缓存解码后的位图，每次入视口都重新解码。
3. 网络下载是单 FIFO 队列，4 个 worker 不分轻重地排队，page JSON 这种"解锁信号"经常被淹在图片下载后面。

四刀分别解决：

| 改动 | 解决的瓶颈 | 收益 |
|---|---|---|
| probe 窗口 ×2 | (1) UI 提前 set_src | 省掉 ~200 ms 的 timer 轮询延迟 |
| sjpg detach + `LV_IMG_CACHE_DEF_SIZE=16` | (2) 解码缓存 | 省掉重入视口时 ~80 ms × 8 张的重复解码 |
| http_pool overflow 优先级队列 | (3) 网络队列乱序 | 当前可见图最坏只等 1 张 LOW 跑完 |
| page JSON 也走 HIGH | (3) 依赖反转 | 解锁信号永远先于它产出的图片 |

---

## 改动 1：probe 窗口扩到 2 页

**文件**：`generated/ktv_ui/artist/adapter/lv_artist_adapter.c`

### 关键宏

```c
#ifndef ARTIST_PROBE_PAGES
#define ARTIST_PROBE_PAGES 2U
#endif
```

### 改动点

- `lv_artist_adapter_start`：`probe_slot_count = ui_batch_size * ARTIST_PROBE_PAGES`（8 → 16）。
- `artist_probe_reset_for_page`：用 `[page_start, page_start + probe_slot_count)` 作为 UI 预绑窗口，不再被 `page_end` 截断。

### 为什么

`lv_vlist` 因为 `overscan_rows_back=1`，物理上已经存在下一页的 cell（只是位置在视口下方）。但旧 probe 只看当前页 8 个，下一页 8 个 cell 永远拿不到 `avatar_local_path`，所以 `lv_img_set_src` 不会被提前触发。

probe ×2 后，timer 一帧就把当前页 + 下一页 16 个 cell 都"喂"了 path，下一页 8 张图的 `lv_img_set_src` 在用户**还在当前页时**就发出去了，解码可以提前进行。

### 边界

只动 UI 层"预绑窗口"，**不动**网络层 prefetch 范围（`artist_calc_prefetch_window` 仍然是 idle +2 页 / 前向 +3 页）。理由：拓宽 `demo_ui_scroll_range` 会让 `img_mgr` 的 `focus_slot_id` 跑得很远，LRU 会先把当前页的 entry 淘汰掉。

---

## 改动 2：硬件 JPG 解码器支持安全 cache

**文件**：
- `lvgl/src/extra/libs/sjpg/sunxijpgd.h`
- `lvgl/src/extra/libs/sjpg/sunxijpgd.c`
- `lvgl/src/extra/libs/sjpg/lv_sjpg.c`
- `lv_projector/src/lv_conf.h`

### 问题

`LV_IMG_CACHE_DEF_SIZE > 0` 配合现有的硬件 JPG 解码器**会必崩**。

- `lv_sjpg.c` 全局只有一个 `static JpegDecoder* jpegdecoder` 单例。
- `decoder_open` 让 `dsc->img_data` 直接指向单例内部的 `mImgFrame.mRGBData`。
- `JpegDecoderGetFrame` 入口会 `lv_mem_free` 上一张图的 `mRGBData` 再重新 alloc。
- cache 一开，N 张 dsc 同时存活，第二张 open 就会把第一张的 `img_data` 释放掉 → use-after-free → SIGSEGV。

### 解法：解码完立即"摘走"缓冲

在 `sunxijpgd` 加两个 API：

```c
/* 把刚 GetFrame 的 RGB 缓冲从单例摘走，所有权交给调用方 */
uint8_t* JpegDecoderDetachFrameBuffer(JpegDecoder* v,
                                      uint32_t* out_display_w,
                                      uint32_t* out_display_h);

/* 释放上面摘出来的缓冲，按编译宏自动选 sunxifb_mem_free / lv_mem_free */
void JpegDecoderFreeFrameBuffer(uint8_t* buf);
```

`Detach` 实现的关键就一行：

```c
p->mImgFrame.mRGBData = NULL;
memset(&p->mImgFrame, 0x00, sizeof(ImgFrame));
```

这样下次 `JpegDecoderGetFrame` 入口的 free 分支就是 NULL 跳过，不会破坏已经摘出去的缓冲。

### `lv_sjpg.c` 的新流程

```text
decoder_open:
  GetFrame → DetachFrameBuffer → ReleaseSession（销毁 VideoDecoder）
  dsc->img_data = 摘出来的独立缓冲

decoder_close:
  JpegDecoderFreeFrameBuffer(dsc->img_data)
  （不再 ReleaseSession，已经在 open 末尾做过）
```

### 安全性 & 资源代价

| 维度 | 旧实现 | 新实现 |
|---|---|---|
| 同一时刻 cedarc VideoDecoder 实例 | 1 | **仍然是 1**（detach 后立即 release） |
| 同一时刻 ION SBM 缓冲 | 1 张 | **仍然是 1 张** |
| 普通堆 RGB 缓冲 | 单例 1 张 ≈ 131 KB | cache 16 张 ≈ 2.1 MB |

### `lv_conf.h`

```c
#define LV_IMG_CACHE_DEF_SIZE 16
```

160×210×4 = ~131 KB / 张 × 16 = ~2.1 MB。

### 警告

**这套 detach 方案是 `LV_IMG_CACHE_DEF_SIZE > 0` 安全的前置条件**。如果哪天有人把 `lv_sjpg.c` 的 `decoder_open` / `decoder_close` 回退成"直接共享单例 mRGBData"，cache 必须同时改回 0，否则必崩。

---

## 改动 3：http_pool 加优先级 overflow

**文件**：
- `generated/ktv_ctrl/http_client/http_common.h`
- `generated/ktv_ctrl/http_client/http_pool.c`
- `generated/ktv_ctrl/http_client/http_api.h/c`
- `generated/ktv_ctrl/http_client/ktv_ctrl.h/c`
- `generated/image_manager.h/c`
- `generated/artist_media_loader.h/c`
- `generated/artist_media_loader_demo.c`
- `generated/ktv_ui/artist/adapter/lv_artist_adapter.c`

### 架构

```
http_task_t.priority = HIGH/LOW
        ↑
http_download_priority / http_fetch_priority
        ↑
Ktv_Ctrl_PostTask 读 KtvRequest_t.priority
        ↑
artist_media_fetch_image / fetch_page 写 req.priority
        ↑
demo_need_image (cb 多 high_priority 参数)
        ↑
img_mgr_notify_need_image_snapshot 根据 visible_range 决定
        ↑
lv_artist_adapter 调 img_mgr_set_visible_range(page_start+1, page_end+1)
```

### 队列层面

`http_pool` 的内部结构：

```
mq (kernel mqueue, 深度 ~10) ← worker 1..4 取
   ↑ 每张 worker 跑完都 try_flush_overflow_locked
overflow (链表) ← 我们插队的地方
```

新增的两个 helper：

```c
http_pool_overflow_push_front_locked(pool, node);  // HIGH
http_pool_overflow_push_locked(pool, node);        // LOW（旧行为）
http_pool_overflow_push_by_priority_locked(pool, node, priority);  // 派发
```

`http_pool_submit` 进 overflow 时按 `task->priority` 选队头/队尾。`try_flush_overflow_locked` 本来就是从队首拉，所以 HIGH 总会先于队中的 LOW 进 mq。

### 不能解决的部分

- **正在跑的 LOW 不可中断**（curl 单条请求不可中断）。HIGH 最坏要等 1 张 LOW 跑完，约几百毫秒。
- **mq 里已经存在的 LOW 也会按 FIFO 跑完**。如果发现 HIGH 还是被淹住，说明 mq 深度太大，可以把 mq 强制压到 `thread_num+1=5`，让 overflow 真正成为"主排队层"。

---

## 改动 4：当前可见页 + page JSON 都标 HIGH

### 谁标 HIGH

| 类型 | 优先级 | 标注地方 |
|---|---|---|
| page JSON（解锁本页 50 张图的 URL）| **HIGH（强制）** | `artist_media_fetch_page` 写死 `req.priority = KTV_PRIORITY_HIGH` |
| 当前可见 8 张图 | HIGH | `img_mgr_set_visible_range` 决定 |
| prefetch 范围内的下一页/上一页图 | LOW | 默认 |
| 远景 prefetch 图 | LOW | 默认 |

### `img_mgr_set_visible_range`

```c
void img_mgr_set_visible_range(uint32_t start_slot_id, uint32_t end_slot_id);
```

`lv_artist_adapter` 的 `artist_prefetch_page_window` 在 `demo_ui_scroll_range` 之前调一次：

```c
img_mgr_set_visible_range(page_start + 1U, page_end + 1U);
demo_ui_scroll_range(prefetch_start + 1U, prefetch_end + 1U);
```

`img_mgr` 触发 `need_image` 通知时根据 slot 是否在 `[visible_start, visible_end]` 内决定 `high_priority` 参数，cb 链路一直传到 `KtvRequest_t.priority`。

### 为什么 page JSON 必须最高

依赖反转风险：用户快速翻页时，多个 page JSON 一起入队，如果它们走 LOW 而图片走 HIGH/LOW 混排，可能出现：

```
queue: image_p0_1..8(HIGH) → image_p0_9..50(LOW) → page1.json(LOW) → page2.json(LOW) ...
```

后面页 JSON 卡在前面页图片后面，这一页图的 URL 永远等不到，UI 该出图的位置就一直是占位符。强制 page JSON HIGH 后，每页"解锁信号"总会比它产出的图片任务更早跑完。

---

## 关键文件 / 关键宏速查

| 文件 | 关键改动 |
|---|---|
| `lv_projector/src/lv_conf.h` | `LV_IMG_CACHE_DEF_SIZE 16` |
| `lvgl/src/extra/libs/sjpg/sunxijpgd.h` | `JpegDecoderDetachFrameBuffer` / `JpegDecoderFreeFrameBuffer` 声明 |
| `lvgl/src/extra/libs/sjpg/sunxijpgd.c` | 上面两个 API 实现 |
| `lvgl/src/extra/libs/sjpg/lv_sjpg.c` | `decoder_open` / `decoder_close` 改成"独立缓冲所有权"模型 |
| `generated/ktv_ctrl/http_client/http_common.h` | `http_priority_t` 枚举 + `http_task.priority` 字段 |
| `generated/ktv_ctrl/http_client/http_pool.c` | overflow 插队逻辑 |
| `generated/ktv_ctrl/http_client/http_api.h/c` | `http_download_priority` / `http_fetch_priority` |
| `generated/ktv_ctrl/http_client/ktv_ctrl.h/c` | `KtvPriority_t` + `KtvRequest_t.priority` 字段 |
| `generated/image_manager.h/c` | `img_mgr_set_visible_range` + `need_image` cb 多带 `high_priority` |
| `generated/artist_media_loader.c` | `artist_media_fetch_page` 写 HIGH；`artist_media_fetch_image` 透传 |
| `generated/ktv_ui/artist/adapter/lv_artist_adapter.c` | `ARTIST_PROBE_PAGES=2`，translate visible range |

---

## 烧录验证清单

- [ ] **必清**：`make clean`，确保 `lv_sjpg.c`、`sunxijpgd.c`、`lv_obj.c`、`lv_img_cache.c`、`lv_draw_img.c`、`http_pool.c`、`image_manager.c`、`lv_artist_adapter.c` 全部重新出 `.o`。
- [ ] **串口必看**：`BUILD MARK v4: probe-x2 + lv_img_cache=16 + sjpg-detach + http-prio + json-prio`。
- [ ] **probe**：`probe_slots=16 probe_pages=2`，旧固件这里是 8。
- [ ] **场景**：歌手页第 0 → 第 1 页滑动，下一页 8 张图应该已经"摸过"，部分能直接 blit cache。
- [ ] **不崩**：连续翻 5+ 页不出现 SIGSEGV / `JpegMallocFrmBuffer` warn 后随即 segfault。
- [ ] **依赖顺序**：`[artist_media][D] image download ok: slot=N` 不再出现"slot=7 在 slot=50 之后"这种倒挂；page JSON 应该一发一收，不会在图片之间被淹很久。

---

## 后续优化候选（按优先级）

1. **mq 深度强制收紧**：把 `http_pool_create` 的 mq 深度限制为 `thread_num+1=5`，让 overflow 真正成为唯一主排队层，HIGH 插队效果更强。
2. **远 prefetch 任务可取消**：当前 LOW 入 mq 就跑到底；若用户已经滑出去，可以加"过期/取消"标记，worker 取出后跳过。
3. **其他 page JSON 同样标 HIGH**：歌单页、收藏页、推荐页等等，凡是"JSON 解锁后续图片 URL"的列表入口，都应当 HIGH。
4. **解码 worker 化**：把硬件 JPG 解码移到独立线程预解码 16 张，UI 线程只做 blit；进一步把"解码"也从 UI 关键路径上摘出去。
