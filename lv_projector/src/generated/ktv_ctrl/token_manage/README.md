# ktv_token_management（兼容层）+ time/token service

## 使用

系统初始化时调用：

```c
#include "ktv_token_management.h"

ktv_time_thread_start();
```

退出程序或模块反初始化时调用：

```c
ktv_time_thread_stop();
```

## 行为（对外保持不变）

- 线程启动后立即校准一次时间；之后每小时校准一次。
- 每分钟打印一次当前估算时间日志，日志固定显示北京时间（UTC+8），不依赖系统时区。
- 时间计算使用：服务器时间 + `CLOCK_MONOTONIC` 偏移。
- 使用 `KTV_REQ_FETCH_MEMORY`（`ktv_http_get_to_memory()`）进行 token 与时间的 HTTP 拉取。
- 增加断电持久化：token 与最后一次服务器时间会保存到 `/usr/share/lv_projector/`，离线时作为默认回退值。

## 分层与解耦

- `ktv_token_management.[ch]`
  - **对外兼容层**：保留旧 API（时间线程、取时间、取 token）
  - **UI展示**：分钟 tick 时刷新 UI label（不进入核心模块）
- `ktv_time_service.[ch]`
  - **时间核心**：时间同步、单调时钟估算、断电回退
  - **可选 hook**：通过 `ktv_time_service_set_tick_hook()` 把“打印北京时间/刷新UI”等业务挂回去
- `ktv_token_service.[ch]`
  - **token 核心**：token 拉取、过期判断、缓存、断电回退
- `ktv_persist_util.[ch]`
  - **持久化工具**：小文件读取、temp+rename 原子写入（best-effort）

## 编译注意

需要链接 pthread：

```makefile
LDFLAGS += -lpthread
```


## 北京时间修正

日志格式现在是：

```text
[KTV_TIME] minute_log utc=1777183505 beijing=2026-04-26 14:05:05
```

转换方式固定为：

```c
time_t t = (time_t)(utc_sec + 8 * 60 * 60);
gmtime_r(&t, &tm_info);
```

不再使用 `localtime_r()`，避免板子系统时区未设置时打印错误时间。
