#ifndef HTTP_ENGINE_H
#define HTTP_ENGINE_H

#include "http_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 下载任务语义：
 * - 先写唯一临时文件
 * - 成功后 rename 到最终路径
 * - 失败时删除临时文件
 * - 不会直接写最终路径
 */

http_err_t http_engine_init(const http_config_t *config);
void http_engine_deinit(void);

http_err_t http_engine_execute(http_task_t *task);
void http_engine_cancel(http_task_t *task);
void http_engine_get_stats(http_stats_t *stats);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_ENGINE_H */