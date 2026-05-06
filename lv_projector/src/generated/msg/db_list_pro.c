#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "db_list_pro.h"

/*
 * 安全拷贝队列名。
 * - 仅用于日志打印。
 * - 无论源字符串是否过长，都保证目标字符串以 '\0' 结尾。
 */
static void db_list_pro_copy_name(char *dst, size_t dst_size, const char *src)
{
    if (dst == NULL || dst_size == 0) {
        return;
    }

    if (src == NULL) {
        dst[0] = '\0';
        return;
    }

    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
}

/*
 * 在“已持有 mutex”的前提下执行一次真正入队。
 *
 * 注意：
 * - 本函数不做队列满判断。
 * - 调用者必须先保证 queue 有空位。
 * - 本函数只负责写槽位、推进 tail、递增 count。
 */
static int db_list_pro_do_push_locked(db_list_pro_t *queue,
                                      int i1, int i2, int i3,
                                      void *p1, void *p2, void *p3)
{
    db_list_pro_msg_t *slot = NULL;

    if (queue == NULL) {
        errno = EINVAL;
        return -1;
    }

    slot = &queue->buffer[queue->tail];
    slot->i1 = i1;
    slot->i2 = i2;
    slot->i3 = i3;
    slot->p1 = p1;
    slot->p2 = p2;
    slot->p3 = p3;

    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->count++;

    DBP_LOGD("%s push ok, count=%d\n", queue->name, queue->count);
    return 0;
}

/*
 * 在“已持有 mutex”的前提下执行一次真正出队。
 *
 * 注意：
 * - 本函数不做队列空判断。
 * - 调用者必须先保证 queue 有消息。
 * - 为了避免旧数据调试时误判，出队后会把原槽位清零。
 */
static int db_list_pro_do_pop_locked(db_list_pro_t *queue, db_list_pro_msg_t *out_msg)
{
    db_list_pro_msg_t *slot = NULL;

    if (queue == NULL || out_msg == NULL) {
        errno = EINVAL;
        return -1;
    }

    slot = &queue->buffer[queue->head];
    *out_msg = *slot;

    memset(slot, 0, sizeof(*slot));
    queue->head = (queue->head + 1) % queue->capacity;
    queue->count--;

    DBP_LOGD("%s pop ok, count=%d\n", queue->name, queue->count);
    return 0;
}

/* 创建固定容量环形队列。 */
db_list_pro_t *db_list_pro_create(const char *name, int capacity, int block_push, int block_pop)
{
    db_list_pro_t *queue = NULL;
    int ret = 0;

    if (capacity <= 0) {
        errno = EINVAL;
        DBP_LOGE("create failed: invalid capacity=%d\n", capacity);
        return NULL;
    }

    queue = (db_list_pro_t *)malloc(sizeof(db_list_pro_t));
    if (queue == NULL) {
        errno = ENOMEM;
        DBP_LOGE("create failed: malloc queue error\n");
        return NULL;
    }
    memset(queue, 0, sizeof(*queue));

    /*
     * 一次性分配固定数量的消息槽。
     * 后续 push/pop 只在这片固定内存上读写，不再分配节点。
     */
    queue->buffer = (db_list_pro_msg_t *)calloc((size_t)capacity, sizeof(db_list_pro_msg_t));
    if (queue->buffer == NULL) {
        errno = ENOMEM;
        DBP_LOGE("create failed: calloc buffer error\n");
        free(queue);
        return NULL;
    }

    ret = pthread_mutex_init(&queue->mutex, NULL);
    if (ret != 0) {
        errno = ret;
        DBP_LOGE("create failed: pthread_mutex_init error=%d\n", ret);
        free(queue->buffer);
        free(queue);
        return NULL;
    }

    ret = pthread_cond_init(&queue->cond_not_empty, NULL);
    if (ret != 0) {
        errno = ret;
        DBP_LOGE("create failed: pthread_cond_init(not_empty) error=%d\n", ret);
        pthread_mutex_destroy(&queue->mutex);
        free(queue->buffer);
        free(queue);
        return NULL;
    }

    ret = pthread_cond_init(&queue->cond_not_full, NULL);
    if (ret != 0) {
        errno = ret;
        DBP_LOGE("create failed: pthread_cond_init(not_full) error=%d\n", ret);
        pthread_cond_destroy(&queue->cond_not_empty);
        pthread_mutex_destroy(&queue->mutex);
        free(queue->buffer);
        free(queue);
        return NULL;
    }

    queue->capacity = capacity;
    queue->block_push = block_push ? 1 : 0;
    queue->block_pop = block_pop ? 1 : 0;
    queue->is_shutdown = 0;
    db_list_pro_copy_name(queue->name, sizeof(queue->name), name != NULL ? name : "db_list_pro");

    DBP_LOGI("%s create ok, capacity=%d, block_push=%d, block_pop=%d\n",
             queue->name, queue->capacity, queue->block_push, queue->block_pop);
    return queue;
}

/*
 * 销毁队列对象。
 *
 * 说明：
 * - 这里不处理业务指针释放。
 * - 当前项目模型下，业务指针由 pop 后的消费者自己处理。
 */
void db_list_pro_destroy(db_list_pro_t *queue)
{
    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("destroy failed: queue is null\n");
        return;
    }

    pthread_mutex_lock(&queue->mutex);

    /*
     * 先置 shutdown 并广播唤醒，防止外部线程仍阻塞在 push/pop 上。
     */
    queue->is_shutdown = 1;
    pthread_cond_broadcast(&queue->cond_not_empty);
    pthread_cond_broadcast(&queue->cond_not_full);
    DBP_LOGI("%s destroy, remain_count=%d\n", queue->name, queue->count);

    pthread_mutex_unlock(&queue->mutex);

    pthread_cond_destroy(&queue->cond_not_empty);
    pthread_cond_destroy(&queue->cond_not_full);
    pthread_mutex_destroy(&queue->mutex);

    free(queue->buffer);
    queue->buffer = NULL;
    free(queue);
}

/*
 * 关闭队列并唤醒所有阻塞线程。
 *
 * 典型用途：
 * - 工作线程退出前，先 shutdown 队列，让阻塞的 pop/push 及时返回。
 */
int db_list_pro_shutdown(db_list_pro_t *queue)
{
    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("shutdown failed: queue is null\n");
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);
    queue->is_shutdown = 1;
    pthread_cond_broadcast(&queue->cond_not_empty);
    pthread_cond_broadcast(&queue->cond_not_full);
    pthread_mutex_unlock(&queue->mutex);

    DBP_LOGI("%s shutdown signaled\n", queue->name);
    return 0;
}

/*
 * 清空队列中的所有槽位。
 *
 * 注意：
 * - 这里只重置消息槽和队列计数。
 * - 不处理业务数据指针的释放。
 */
int db_list_pro_clear(db_list_pro_t *queue)
{
    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("clear failed: queue is null\n");
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);

    memset(queue->buffer, 0, (size_t)queue->capacity * sizeof(db_list_pro_msg_t));
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;

    /*
     * 队列被清空后，原来可能阻塞在“队列满”的生产者应当被唤醒。
     */
    pthread_cond_broadcast(&queue->cond_not_full);

    pthread_mutex_unlock(&queue->mutex);

    DBP_LOGI("%s clear ok\n", queue->name);
    return 0;
}

/*
 * 入队接口。
 *
 * 实现要点：
 * 1. 队列满时，依据 block_push 决定立即失败还是阻塞等待。
 * 2. 使用 while 重检条件，防止伪唤醒。
 * 3. shutdown 后不再允许继续入队。
 */
int db_list_pro_push(db_list_pro_t *queue,
                     int i1, int i2, int i3,
                     void *p1, void *p2, void *p3)
{
    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("push failed: queue is null\n");
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);

    while (queue->count >= queue->capacity && !queue->is_shutdown) {
        if (!queue->block_push) {
            pthread_mutex_unlock(&queue->mutex);
            errno = EAGAIN;
            DBP_LOGW("%s push failed: queue full\n", queue->name);
            return -1;
        }

        DBP_LOGD("%s push wait: queue full\n", queue->name);
        pthread_cond_wait(&queue->cond_not_full, &queue->mutex);
    }

    if (queue->is_shutdown) {
        pthread_mutex_unlock(&queue->mutex);
        errno = ECANCELED;
        DBP_LOGW("%s push canceled: queue shutdown\n", queue->name);
        return -1;
    }

    db_list_pro_do_push_locked(queue, i1, i2, i3, p1, p2, p3);

    /*
     * 入队成功后，通知“等待消息”的消费者。
     */
    pthread_cond_signal(&queue->cond_not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

/*
 * 出队接口。
 *
 * 实现要点：
 * 1. 队列空时，依据 block_pop 决定立即失败还是阻塞等待。
 * 2. 使用 while 重检条件，防止伪唤醒。
 * 3. shutdown 后，阻塞 pop 会被唤醒并返回失败。
 */
int db_list_pro_pop(db_list_pro_t *queue, db_list_pro_msg_t *out_msg)
{
    if (queue == NULL || out_msg == NULL) {
        errno = EINVAL;
        DBP_LOGE("pop failed: invalid param\n");
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);

    while (queue->count <= 0 && !queue->is_shutdown) {
        if (!queue->block_pop) {
            pthread_mutex_unlock(&queue->mutex);
            errno = EAGAIN;
            return -1;
        }

        DBP_LOGD("%s pop wait: queue empty\n", queue->name);
        pthread_cond_wait(&queue->cond_not_empty, &queue->mutex);
    }

    if (queue->count <= 0 && queue->is_shutdown) {
        pthread_mutex_unlock(&queue->mutex);
        errno = ECANCELED;
        DBP_LOGW("%s pop canceled: queue shutdown\n", queue->name);
        return -1;
    }

    db_list_pro_do_pop_locked(queue, out_msg);

    /*
     * 出队成功后，通知“等待空位”的生产者。
     */
    pthread_cond_signal(&queue->cond_not_full);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

/*
 * 非阻塞出队。
 *
 * 说明：
 * - 临时关闭 block_pop 语义，复用统一的 pop 逻辑。
 * - 适合 UI 主线程“轮询式”接收消息。
 */
int db_list_pro_try_pop(db_list_pro_t *queue, db_list_pro_msg_t *out_msg)
{
    int ret = 0;
    int old_block_pop = 0;

    if (queue == NULL || out_msg == NULL) {
        errno = EINVAL;
        DBP_LOGE("try_pop failed: invalid param\n");
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);
    old_block_pop = queue->block_pop;
    queue->block_pop = 0;
    pthread_mutex_unlock(&queue->mutex);

    ret = db_list_pro_pop(queue, out_msg);

    pthread_mutex_lock(&queue->mutex);
    queue->block_pop = old_block_pop;
    pthread_mutex_unlock(&queue->mutex);

    return ret;
}

/* 查询是否为空。 */
int db_list_pro_is_empty(db_list_pro_t *queue)
{
    int ret = 0;

    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("is_empty failed: queue is null\n");
        return 1;
    }

    pthread_mutex_lock(&queue->mutex);
    ret = (queue->count == 0);
    pthread_mutex_unlock(&queue->mutex);
    return ret;
}

/* 查询是否已满。 */
int db_list_pro_is_full(db_list_pro_t *queue)
{
    int ret = 0;

    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("is_full failed: queue is null\n");
        return 0;
    }

    pthread_mutex_lock(&queue->mutex);
    ret = (queue->count >= queue->capacity);
    pthread_mutex_unlock(&queue->mutex);
    return ret;
}

/* 查询当前消息数量。 */
int db_list_pro_get_count(db_list_pro_t *queue)
{
    int ret = 0;

    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("get_count failed: queue is null\n");
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);
    ret = queue->count;
    pthread_mutex_unlock(&queue->mutex);
    return ret;
}

/* 查询队列总容量。 */
int db_list_pro_get_capacity(db_list_pro_t *queue)
{
    int ret = 0;

    if (queue == NULL) {
        errno = EINVAL;
        DBP_LOGE("get_capacity failed: queue is null\n");
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);
    ret = queue->capacity;
    pthread_mutex_unlock(&queue->mutex);
    return ret;
}
