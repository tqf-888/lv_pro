#ifndef __SYSTEM_API_H__
#define __SYSTEM_API_H__

#include "sys_param.h"

/* 系统初始化 */
void system_init(void);

/* 系统关机 */
void system_power_off(void);

/* 系统待机 */
void system_standby(void);

/* 设置 PQ 参数（图像模式/对比度/亮度/色彩/锐度/色温） */
int set_pq_param(sys_param_id type, int value);
int audio_set_param(sys_param_id type, int value);


#endif /* __SYSTEM_API_H__ */