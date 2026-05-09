## 背景与目标

原 `ktv_token_management.[ch]` 同时承担了：
- token 获取/解析/过期判断
- 时间同步（含线程、日志）
- UI label 刷新（LVGL）
- 网络请求

导致问题：
- **层次混乱**：业务、网络、持久化、UI 强耦合
- **复用困难**：想换成 `KTV_REQ_FETCH_MEMORY` 需要改动大
- **失败策略不清晰**：离线/断电后的默认值行为不一致

本次整理的目标是：
- token 与时间拉取统一走 `KTV_REQ_FETCH_MEMORY`（`ktv_http_get_to_memory()`）
- 获取成功后写入 **静态缓存** + **断电持久化**（`/usr/share/lv_projector/xxxx`）
- 获取失败时，对外返回 **默认值**（内存缓存或断电缓存），不崩溃、不阻塞
- 代码分层清晰、低耦合、注释说明关键意图

## 模块拆分

### 1) 对外兼容层：`ktv_token_management.[ch]`

- **职责**：
  - 保留旧接口（`ktv_time_thread_start/stop`、`ktv_get_current_time_sec`、`ktv_get_token`）
  - 把“UI展示/北京时间日志”这类表现层逻辑放在这里
- **依赖**：
  - `ktv_time_service`（时间核心）
  - `ktv_token_service`（token 核心）
  - `gui_guider.h`（LVGL UI）

### 2) 时间核心：`ktv_time_service.[ch]`

- **职责**：
  - 通过 `ktv_http_get_to_memory()` 拉取时间服务器 JSON（字段 `currentTime`，毫秒时间戳）
  - 维护时间基准：`server_sec + (CLOCK_MONOTONIC 差值)`，对外提供 `now_sec()`
  - **断电回退**：保存最后一次 `server_sec` 到 `/usr/share/lv_projector/ktv_time_cache.json`
- **不做的事**：
  - 不直接触碰 UI（通过 hook 把“业务/展示”挂回去）

### 3) token 核心：`ktv_token_service.[ch]`

- **职责**：
  - 通过 `ktv_http_get_to_memory()` 拉取 token JSON（字段 `data.token`、`data.expir_date`）
  - 缓存 `token` 与 `expire_time_sec`，过期时刷新
  - **断电回退**：保存到 `/usr/share/lv_projector/ktv_token_cache.json`
- **失败策略**：
  - 刷新失败：返回缓存值
  - 缓存为空：返回空字符串（永不返回 NULL），避免调用方拼接 URL/签名时崩溃

### 4) 持久化工具：`ktv_persist_util.[ch]`

- **职责**：
  - 小文件读取
  - temp 文件写入 + `rename()` 覆盖（best-effort，尽量避免断电写坏）

## 数据流（高层）

- **时间线程启动**：
  - `ktv_time_thread_start()` → `ktv_time_service_init()`（读断电缓存）→ `ktv_time_service_thread_start()`
  - `ktv_time_service` 内部线程：
    - 启动即 `sync_now()`
    - 每小时 `sync_now()`
    - 每分钟打印一次 tick，并触发 hook（由兼容层刷新 UI / 打印北京时间）

- **token 获取**：
  - `ktv_get_token()` → `ktv_token_service_init()`（读断电缓存）→ `ktv_token_service_get()`
  - `get()` 判断未初始化/过期 → `refresh_now()`（内部带重试）
  - 刷新成功：更新内存缓存并持久化
  - 刷新失败：返回缓存值（或空字符串）

## 关键约束与取舍

- **不阻塞 UI**：token/time 的 HTTP 拉取由控制器线程池执行；token 获取接口会等待请求完成（与现有 `ktv_http_get_to_memory()` 行为一致），但失败后会立刻回退缓存值。
- **断电回退时间的精度**：仅持久化 `server_sec`，重启后以当前 monotonic 作为新基准继续递增，精度足够用于“token过期判断/页面显示”场景。
- **不依赖系统时区**：北京时间展示使用 “UTC + 8小时 + gmtime_r”。

