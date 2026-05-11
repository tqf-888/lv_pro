// mqtt_handler.h
#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

// 对外暴露的初始化接口（内部会创建线程并运行 MQTT 循环）
void mqtt_run(void);
void mqtt_start_service(void);

// 上报歌曲已结束/被切歌，云端收到后会推进歌单并通知小程序
int mqtt_publish_song_finished(const char *song_id, const char *reason);

#endif
