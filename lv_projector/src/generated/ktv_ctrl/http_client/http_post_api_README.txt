新增文件说明
===========

这套文件是“独立 POST 模块”，不需要改动任何现有 http_api/http_engine/http_pool/http_mq 源文件。

新增文件：
1. http_post_api.h
2. http_post_api.c

特性：
- 只新增，不覆盖旧文件
- 调用 http_post() 时，url/body/content_type 立即深拷贝
- 调用方不用释放请求体
- callback 内可直接读 task->response_data
- callback 返回后，模块自动释放 response_data
- 内部自带 lazy worker 队列，不需要显式 init

对外接口：
- http_post(...)
- http_post_wait_all()
- http_post_shutdown()

最小调用示例：

#include "http_post_api.h"

static void on_post_done(http_task_t *task, void *user_data)
{
    (void)user_data;
    printf("post done: err=%d http=%ld size=%u\n",
           task->err_code,
           task->http_status,
           (unsigned)task->response_size);

    if (task->response_data) {
        printf("resp=%s\n", task->response_data);
    }
}

void demo_send_post(void)
{
    http_post("https://example.com/api/demo",
              "{\"id\":123}",
              "application/json",
              on_post_done,
              NULL);
}

编译接入：
- 只需要把 http_post_api.c 加入你的编译列表
- 使用处 include "http_post_api.h"
- 链接 libcurl 和 pthread
