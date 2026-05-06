#ifndef HTTP_MQ_H
#define HTTP_MQ_H

#include "http_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct http_mq http_mq_t;

/*
 * 语义：
 * - This is an in-process blocking queue (NOT POSIX mqueue).
 * - http_mq_send()：短等待发送，队列临时满时会等待极短时间，仍失败才返回 HTTP_ERR_QUEUE_FULL
 * - http_mq_recv()：阻塞接收
 * - Queue stores raw bytes; caller controls message struct lifetime/copy semantics.
 */

http_mq_t *http_mq_create(const char *name, uint32_t max_msgs, uint32_t msg_size);
void http_mq_destroy(http_mq_t *mq);

http_err_t http_mq_send(http_mq_t *mq, const void *msg, uint32_t size);
http_err_t http_mq_recv(http_mq_t *mq, void *msg, uint32_t size);

uint32_t http_mq_count(http_mq_t *mq);
void http_mq_clear(http_mq_t *mq);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_MQ_H */