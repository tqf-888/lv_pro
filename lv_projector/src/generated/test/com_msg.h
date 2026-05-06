#ifndef KTV_COM_MSG_H
#define KTV_COM_MSG_H

#include <pthread.h>
#include "my_list.h"

/* 底层消息结构体 */
typedef struct ktv_com_msg {
    int type;
    void *ptr;
    void *ext_ptr;
    int val0;
    int val1;
    struct list_head list;
} ktv_com_msg_t;

/* 底层队列结构体 */
typedef struct ktv_com_queue {
    struct list_head head;
    pthread_mutex_t mutex;
} ktv_com_queue_t;

/* 底层接口 (仅供 sys_msg.c 调用) */
ktv_com_queue_t *ktv_com_msg_create(const char *name);
void ktv_com_msg_destroy(ktv_com_queue_t *queue);
void ktv_com_msg_enqueue(ktv_com_queue_t *queue, int type,
                         void *ptr, void *ext_ptr, int val0, int val1);
ktv_com_msg_t *ktv_com_msg_dequeue(ktv_com_queue_t *queue);
void ktv_com_msg_free(ktv_com_msg_t *msg);

#endif /* KTV_COM_MSG_H */
