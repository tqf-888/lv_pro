// mqtt_handler.h
#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

// 对外暴露的唯一初始化接口（内部会创建线程并阻塞式运行MQTT循环）
void mqtt_start_service(void);

#endif
