#ifndef __DB_LIST_PRO_H__
#define __DB_LIST_PRO_H__

#include <pthread.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * db_list_pro
 * -----------------------------------------------------------------------------
 * 设计目标：
 * 1. 新增一套独立于旧 dbList 的消息队列实现，不污染旧逻辑。
 * 2. 采用固定容量环形队列，避免每次 push/pop 都频繁 malloc/free 节点。
 * 3. 供“非 LVGL 业务线程”和“LVGL 消息接收侧”统一收发固定格式消息。
 * 4. 每条消息固定携带 6 个参数：3 个 int + 3 个 void *。
 *
 * 使用约束：
 * 1. 队列只保存 p1/p2/p3 的“指针值”，不深拷贝指针指向的业务数据。
 * 2. 队列绝不 free p1/p2/p3，谁 pop 出来谁根据业务规则处理。
 * 3. 当前项目约定只传两类指针：
 *    - heap 指针：例如 malloc/calloc/realloc 得到的内存，消费者使用后自行 free。
 *    - static/global 指针：静态区/全局区长期有效地址，消费者只使用，不 free。
 * 4. 禁止传递栈上临时变量地址。
 */

/* ------------------------------ 日志宏配置 --------------------------------- */
#ifndef DB_LIST_PRO_LOG_ENABLE
#define DB_LIST_PRO_LOG_ENABLE 1
#endif

#ifndef DB_LIST_PRO_LOG_LEVEL
#define DB_LIST_PRO_LOG_LEVEL 3
/* 0:none 1:error 2:warn 3:info 4:debug */
#endif

#if DB_LIST_PRO_LOG_ENABLE
#include <stdio.h>
#define DBP_LOGE(fmt, ...) do { if (DB_LIST_PRO_LOG_LEVEL >= 1) printf("[DBP][E] " fmt, ##__VA_ARGS__); } while (0)
#define DBP_LOGW(fmt, ...) do { if (DB_LIST_PRO_LOG_LEVEL >= 2) printf("[DBP][W] " fmt, ##__VA_ARGS__); } while (0)
#define DBP_LOGI(fmt, ...) do { if (DB_LIST_PRO_LOG_LEVEL >= 3) printf("[DBP][I] " fmt, ##__VA_ARGS__); } while (0)
#define DBP_LOGD(fmt, ...) do { if (DB_LIST_PRO_LOG_LEVEL >= 4) printf("[DBP][D] " fmt, ##__VA_ARGS__); } while (0)
#else
#define DBP_LOGE(fmt, ...) do { } while (0)
#define DBP_LOGW(fmt, ...) do { } while (0)
#define DBP_LOGI(fmt, ...) do { } while (0)
#define DBP_LOGD(fmt, ...) do { } while (0)
#endif

/*
 * 一条固定格式消息。
 *
 * 字段约定：
 * - i1/i2/i3 : 三个整型参数。
 * - p1/p2/p3 : 三个指针参数。
 *
 * 推荐约定：
 * - i1 通常作为“消息类型”。
 * - i2/i3 作为附加整型参数。
 * - p1/p2/p3 作为附加上下文指针/数据指针。
 */
typedef struct db_list_pro_msg {
    int i1;
    int i2;
    int i3;
    void *p1;
    void *p2;
    void *p3;
} db_list_pro_msg_t;

/*
 * 队列对象。
 *
 * 说明：
 * - buffer    : 固定容量消息槽数组。
 * - head/tail : 环形队列读写下标。
 * - count     : 当前消息数量。
 * - block_push: 队列满时 push 是否阻塞等待。
 * - block_pop : 队列空时 pop 是否阻塞等待。
 * - is_shutdown: 关闭标记，用于唤醒阻塞线程并让后续收发尽快退出。
 */
typedef struct db_list_pro {
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;

    db_list_pro_msg_t *buffer;

    int capacity;
    int head;
    int tail;
    int count;

    int block_push;
    int block_pop;
    int is_shutdown;

    char name[32];
} db_list_pro_t;

/*
 * 创建固定容量环形队列。
 *
 * @name       队列名，仅用于日志打印。
 * @capacity   队列容量，必须 > 0。
 * @block_push 队列满时是否阻塞：0=立即返回失败，1=阻塞等待空位。
 * @block_pop  队列空时是否阻塞：0=立即返回失败，1=阻塞等待消息。
 *
 * 返回值：
 * - 成功：返回队列句柄。
 * - 失败：返回 NULL，并设置 errno。
 */
db_list_pro_t *db_list_pro_create(const char *name, int capacity, int block_push, int block_pop);

/*
 * 销毁队列对象。
 *
 * 注意：
 * 1. 只释放队列自身资源（mutex/cond/buffer/queue）。
 * 2. 不释放队列残留消息中的 p1/p2/p3。
 * 3. 如果残留消息里放的是 heap 指针，调用者需自行保证业务层已处理或可接受进程退出回收。
 */
void db_list_pro_destroy(db_list_pro_t *queue);

/*
 * 触发 shutdown。
 *
 * 作用：
 * 1. 将队列标记为 shutdown。
 * 2. 唤醒所有阻塞中的 push/pop。
 * 3. 常用于线程退出流程中的“解阻塞”。
 */
int db_list_pro_shutdown(db_list_pro_t *queue);

/*
 * 清空队列中的所有消息槽。
 *
 * 注意：
 * - 只清空消息元数据，不 free p1/p2/p3 指向的业务内存。
 */
int db_list_pro_clear(db_list_pro_t *queue);

/*
 * 入队一条固定 6 参数消息。
 *
 * 返回值：
 * - 0  : 成功。
 * - -1 : 失败，可能是参数错误、队列满且非阻塞、或队列已 shutdown。
 */
int db_list_pro_push(db_list_pro_t *queue,
                     int i1, int i2, int i3,
                     void *p1, void *p2, void *p3);

/*
 * 出队一条消息到 out_msg。
 *
 * 返回值：
 * - 0  : 成功。
 * - -1 : 失败，可能是参数错误、队列空且非阻塞、或队列已 shutdown。
 */
int db_list_pro_pop(db_list_pro_t *queue, db_list_pro_msg_t *out_msg);

/*
 * 非阻塞出队。
 *
 * 说明：
 * - 本接口不等待消息，适合 LVGL 主线程轮询接收 UI 队列消息。
 */
int db_list_pro_try_pop(db_list_pro_t *queue, db_list_pro_msg_t *out_msg);

/* ------------------------------ 状态查询接口 ------------------------------- */
int db_list_pro_is_empty(db_list_pro_t *queue);
int db_list_pro_is_full(db_list_pro_t *queue);
int db_list_pro_get_count(db_list_pro_t *queue);
int db_list_pro_get_capacity(db_list_pro_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* __DB_LIST_PRO_H__ */
