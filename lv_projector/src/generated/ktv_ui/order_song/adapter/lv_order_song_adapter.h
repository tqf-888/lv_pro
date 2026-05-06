#ifndef LV_ORDER_SONG_ADAPTER_H
#define LV_ORDER_SONG_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lv_virtual_list.h"
#include "lv_order_song_catalog.h"
#include "lv_order_song_view_style.h"

/*
 * 这个 adapter 负责三件事：
 * 1. 维护歌曲列表的 catalog 数据
 * 2. 管理批次加载状态（ready / loading）
 * 3. 接收异步线程回调，并在 UI 线程里把 json 同步到列表
 *
 * 这里额外加了 request_epoch，目的只有一个：
 * 区分“当前这一轮页面数据”和“reset 之前的旧请求”。
 *
 * 例子：
 * - 第一次进入页面，request_epoch = 1
 * - 调用 reset 后，request_epoch = 2
 * - 如果旧请求在 reset 之后才回来，它带的还是 epoch=1
 * - UI 线程处理回调时发现 1 != 2，就会直接丢弃旧结果
 */
typedef struct {
    /* 绑定的虚拟列表对象 */
    lv_vlist_t *vlist;

    /* 业务数据缓存，真正的每行歌曲数据都落在这里 */
    lv_order_song_catalog_t catalog;

    /*
     * 行布局、列宽、可见区域等样式配置。
     *
     * 警示：
     * - view_style->overscan_rows_front 允许显式为 0；
     * - adapter 层绝不能再把它兜底成 1；
     * - 否则首屏第 0 行会被错当成前向 overscan，导致顶部丢行。
     */
    const lv_order_song_view_style_t *view_style;

    /* 当前总条目数 */
    uint32_t total_count;

    /* 每个批次多少条，例如 50 */
    uint32_t batch_size;

    /* 一共多少个批次 */
    uint32_t batch_count;

    /*
     * 是否已经明确到达末页。
     *
     * 置 1 条件：
     * - 服务端返回空 data；
     * - 或当前批次返回条数 < batch_size。
     *
     * 一旦置 1，adapter 会同步收缩 total_count / batch_count，
     * 后续不再允许继续请求“下一页”。
     */
    uint8_t end_reached;

    /* 已确认的最后一页批次号，仅用于打印和防御性判断。 */
    uint32_t last_batch_index;

    /* 某个批次是否已经成功同步到 catalog */
    uint8_t *batch_ready;

    /* 某个批次是否已经发出请求，避免重复发 */
    uint8_t *batch_loading;

    /*
     * 下面三组 pending_* 是“异步线程 -> UI线程”的中转区。
     * 异步回调不要直接操作 LVGL UI，只往这里写结果。
     * 真正刷新 UI 由 rs_ui_timer_cb 在 UI 线程里完成。
     */
    uint8_t *pending_batch_valid;
    uint8_t *pending_batch_success;
    uint32_t *pending_batch_epoch;

    /* 是否存在待处理回调，ui_timer 会先看这个位，减少无效扫描 */
    uint8_t has_pending;

    /* 保护 pending_* 这些共享数据 */
    pthread_mutex_t pending_lock;

    /* UI 线程定时器：定期把 pending 回调取出来处理 */
    lv_timer_t *ui_timer;

    /* start 成功后置 1，stop 时据此决定是否 destroy mutex */
    uint8_t started;

    /*
     * 当前数据代号。
     * 每次 reset 都会 +1。
     * 只要回调带回来的 epoch != 当前 request_epoch，
     * 就说明它属于旧请求，必须丢弃。
     */
    uint32_t request_epoch;

    /*
     * 最小止血门闩：
     * - batch 0 在飞时，不再无限叠新的首批请求；
     * - 用户连续 reset 时，只记住最后一次；
     * - 等当前首批回调回来后，再补做最后一次 reset。
     */
    uint8_t first_batch_inflight;
    uint8_t deferred_reset;
    uint32_t deferred_total_count;
} lv_order_song_adapter_t;

int lv_order_song_adapter_start(lv_order_song_adapter_t *adapter,
                               const lv_order_song_view_style_t *view_style,
                               uint32_t total_count,
                               uint32_t batch_size);

void lv_order_song_adapter_stop(lv_order_song_adapter_t *adapter);

lv_vlist_t *lv_order_song_adapter_create_vlist(lv_order_song_adapter_t *adapter, lv_obj_t *parent);

/*
 * reset 后会做三件事：
 * 1. request_epoch + 1，旧请求全部失效
 * 2. 清空 catalog 和批次状态
 * 3. open/reset 都会立即主动请求第 0 批，避免必须手点一下才开始刷新
 */
void lv_order_song_adapter_reset(lv_order_song_adapter_t *adapter, uint32_t total_count);

bool lv_order_song_adapter_get_business_item(lv_order_song_adapter_t *adapter, uint32_t item_id, lv_order_song_item_t *out);
void lv_order_song_adapter_toggle_selected(lv_order_song_adapter_t *adapter, uint32_t item_id);

/* 旧接口：不带 epoch，兼容保留。新代码尽量走 with_epoch 版本。 */
void lv_order_song_adapter_notify_batch_ready(lv_order_song_adapter_t *adapter, uint32_t batch_index, bool success);

/*
 * 新接口：异步线程回调时必须把请求发起时的 epoch 一起带回来。
 * 这样 UI 线程才能判断这是新结果还是旧结果。
 */
void lv_order_song_adapter_notify_batch_ready_with_epoch(lv_order_song_adapter_t *adapter,
                                                        uint32_t batch_index,
                                                        bool success,
                                                        uint32_t request_epoch);

lv_order_song_adapter_t *lv_order_song_adapter_get_default(void);

/* 读取当前 adapter 的 request_epoch，给发请求那一侧使用 */
uint32_t lv_order_song_adapter_get_request_epoch(const lv_order_song_adapter_t *adapter);

/* 默认 adapter 的快捷接口，外部取数线程会直接调它 */
uint32_t lv_order_song_adapter_get_default_request_epoch(void);

/* 旧快捷接口：兼容保留。新代码尽量走 with_epoch 版本。 */
void lv_order_song_adapter_notify_default_batch_ready(uint32_t batch_index, bool success);

/* 默认 adapter 的带 epoch 回调接口 */
void lv_order_song_adapter_notify_default_batch_ready_with_epoch(uint32_t batch_index,
                                                                bool success,
                                                                uint32_t request_epoch);

/*
 * 外部异步取数函数，由业务层实现。
 * order_song 接口无分页，这里 batch_index 固定只会传 0。
 */
extern void fetch_order_song_batch(int batch_index);

#ifdef __cplusplus
}
#endif

#endif
