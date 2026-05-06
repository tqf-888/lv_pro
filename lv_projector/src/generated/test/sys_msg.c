#include "sys_msg.h"
#include "com_msg.h"
#include <stdlib.h>

ktv_msg_queue_t *ktv_msg_create(const char *name)
{
    return ktv_com_msg_create(name);
}

int ktv_msg_send(ktv_msg_queue_t *queue, ktv_msg_type_t msg_type,
                 void *ptr, void *ext_ptr, int val0, int val1)
{
    if (!queue)
        return -1;

    ktv_com_msg_enqueue(queue, (int)msg_type, ptr, ext_ptr, val0, val1);
    return 0;
}

int ktv_msg_receive(ktv_msg_queue_t *queue, ktv_sys_msg_t *msg)
{
    ktv_com_msg_t *tmp_msg;

    if (!queue || !msg)
        return -1;

    tmp_msg = ktv_com_msg_dequeue(queue);
    if (!tmp_msg)
        return 0; /* 队列为空 */

    msg->type = (ktv_msg_type_t)tmp_msg->type;
    msg->ptr = tmp_msg->ptr;
    msg->ext_ptr = tmp_msg->ext_ptr;
    msg->val0 = tmp_msg->val0;
    msg->val1 = tmp_msg->val1;

    ktv_com_msg_free(tmp_msg);

    return 1; /* 成功接收 */
}
