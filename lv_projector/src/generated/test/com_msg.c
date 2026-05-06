#include "com_msg.h"
#include <stdlib.h>
#include <string.h>

ktv_com_queue_t *ktv_com_msg_create(const char *name)
{
    ktv_com_queue_t *queue;
    int ret;

    (void)name;

    queue = malloc(sizeof(*queue));
    if (!queue)
        return NULL;

    INIT_LIST_HEAD(&queue->head);

    ret = pthread_mutex_init(&queue->mutex, NULL);
    if (ret != 0) {
        free(queue);
        return NULL;
    }

    return queue;
}

void ktv_com_msg_destroy(ktv_com_queue_t *queue)
{
    if (!queue)
        return;

    pthread_mutex_destroy(&queue->mutex);
    free(queue);
}

void ktv_com_msg_enqueue(ktv_com_queue_t *queue, int type,
                         void *ptr, void *ext_ptr, int val0, int val1)
{
    ktv_com_msg_t *new_msg;

    if (!queue)
        return;

    new_msg = malloc(sizeof(*new_msg));
    if (!new_msg)
        return;

    new_msg->type = type;
    new_msg->ptr = ptr;
    new_msg->ext_ptr = ext_ptr;
    new_msg->val0 = val0;
    new_msg->val1 = val1;

    pthread_mutex_lock(&queue->mutex);
    list_add_tail(&new_msg->list, &queue->head);
    pthread_mutex_unlock(&queue->mutex);
}

ktv_com_msg_t *ktv_com_msg_dequeue(ktv_com_queue_t *queue)
{
    ktv_com_msg_t *msg = NULL;
    struct list_head *first;

    if (!queue)
        return NULL;

    pthread_mutex_lock(&queue->mutex);

    if (!list_empty(&queue->head)) {
        first = queue->head.next;
        list_del(first);
        msg = list_entry(first, ktv_com_msg_t, list);
    }

    pthread_mutex_unlock(&queue->mutex);
    return msg;
}

void ktv_com_msg_free(ktv_com_msg_t *msg)
{
    if (msg)
        free(msg);
}
