#ifndef KTV_SYS_MSG_H
#define KTV_SYS_MSG_H

/* 前置声明 */
struct ktv_com_queue;
typedef struct ktv_com_queue ktv_msg_queue_t;

/* 系统消息类型枚举 (精简版) */
typedef enum {


    KTV_MSG_LIST_REQUEST_DATA,   /* UI请求：我需要第N页数据 */
} ktv_msg_type_t;

/* 系统消息结构体 */
typedef struct {
    ktv_msg_type_t type;
    void *ptr;      /* 用于传递参数，如 URL 字符串 */
    void *ext_ptr;
    int val0;
    int val1;
} ktv_sys_msg_t;

/* API 接口 */
ktv_msg_queue_t *ktv_msg_create(const char *name);
int ktv_msg_send(ktv_msg_queue_t *queue, ktv_msg_type_t msg_type,
                 void *ptr, void *ext_ptr, int val0, int val1);
int ktv_msg_receive(ktv_msg_queue_t *queue, ktv_sys_msg_t *msg);

#endif /* KTV_SYS_MSG_H */
