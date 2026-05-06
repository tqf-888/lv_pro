#ifndef GEN_QUEUE_H
#define GEN_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void* gen_queue_handle_t;

/* 创建队列实例 */
gen_queue_handle_t gen_queue_create(const char *name);

/* 销毁队列 */
int gen_queue_destroy(gen_queue_handle_t handle);

/* 清空队列 */
int gen_queue_clear(gen_queue_handle_t handle);

/* 
 * 入队：拷贝数据到队列中 
 * @data: 数据指针
 * @len: 数据长度
 * 返回: 0成功, <0失败
 */
int gen_queue_push(gen_queue_handle_t handle, const void *data, uint32_t len);

/* 
 * 出队：将数据拷贝到 out_buffer 中
 * @out_buffer: 接收数据的缓冲区
 * @buffer_len: 缓冲区大小
 * @out_actual_len: 实际拷贝出的数据长度 (可选，NULL则忽略)
 * 返回: 0成功, <0失败或为空
 */
int gen_queue_pop(gen_queue_handle_t handle, void *out_buffer, uint32_t buffer_len, uint32_t *out_actual_len);

/* 判空 */
int gen_queue_is_empty(gen_queue_handle_t handle);

/* 获取数量 */
int gen_queue_get_count(gen_queue_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* GEN_QUEUE_H */
