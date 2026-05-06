#include "gen_queue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dbList.h"

/* 内部节点结构：在数据前加一个长度头 */
typedef struct {
    uint32_t data_len;
    uint8_t  data[]; // 柔性数组
} gen_packet_node_t;

struct gen_queue_context {
    db_list_t *list;
    char name[32];
};

gen_queue_handle_t gen_queue_create(const char *name)
{
    struct gen_queue_context *ctx = (struct gen_queue_context *)malloc(sizeof(struct gen_queue_context));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(*ctx));
    if (name) {
        snprintf(ctx->name, sizeof(ctx->name), "%s", name);
    }
    
    // 复用原有的 db_list
    ctx->list = db_list_create(ctx->name, 0); 
    if (!ctx->list) {
        free(ctx);
        return NULL;
    }
    
    return (gen_queue_handle_t)ctx;
}

int gen_queue_destroy(gen_queue_handle_t handle)
{
    struct gen_queue_context *ctx = (struct gen_queue_context *)handle;
    if (!ctx) return -1;
    
    gen_queue_clear(handle);
    
    if (ctx->list) {
        __db_list_destory(ctx->list);
    }
    
    free(ctx);
    return 0;
}

int gen_queue_clear(gen_queue_handle_t handle)
{
    struct gen_queue_context *ctx = (struct gen_queue_context *)handle;
    if (!ctx || !ctx->list) return -1;
    
    gen_packet_node_t *node;
    while (!is_list_empty(ctx->list)) {
        node = (gen_packet_node_t *)__db_list_pop(ctx->list);
        if (node) {
            free(node);
        }
    }
    return 0;
}

int gen_queue_push(gen_queue_handle_t handle, const void *data, uint32_t len)
{
    struct gen_queue_context *ctx = (struct gen_queue_context *)handle;
    if (!ctx || !ctx->list || !data || len == 0) return -1;
    
    // 分配：节点头 + 数据体
    gen_packet_node_t *node = (gen_packet_node_t *)malloc(sizeof(gen_packet_node_t) + len);
    if (!node) return -2; // No mem
    
    node->data_len = len;
    memcpy(node->data, data, len);
    
    if (__db_list_put_tail(ctx->list, node) != 0) {
        free(node);
        return -3;
    }
    
    return 0;
}

int gen_queue_pop(gen_queue_handle_t handle, void *out_buffer, uint32_t buffer_len, uint32_t *out_actual_len)
{
    struct gen_queue_context *ctx = (struct gen_queue_context *)handle;
    if (!ctx || !ctx->list || !out_buffer) return -1;
    
    if (is_list_empty(ctx->list)) return -2; // Empty
    
    gen_packet_node_t *node = (gen_packet_node_t *)__db_list_pop(ctx->list);
    if (!node) return -3;
    
    // 安全拷贝：防止缓冲区溢出
    uint32_t copy_len = (buffer_len < node->data_len) ? buffer_len : node->data_len;
    memcpy(out_buffer, node->data, copy_len);
    
    if (out_actual_len) {
        *out_actual_len = node->data_len;
    }
    
    free(node); // 弹出后释放节点
    return 0;
}

int gen_queue_is_empty(gen_queue_handle_t handle)
{
    struct gen_queue_context *ctx = (struct gen_queue_context *)handle;
    if (!ctx || !ctx->list) return 1;
    return is_list_empty(ctx->list);
}

int gen_queue_get_count(gen_queue_handle_t handle)
{
    struct gen_queue_context *ctx = (struct gen_queue_context *)handle;
    if (!ctx || !ctx->list) return 0;
    return __db_list_get_num(ctx->list);
}
