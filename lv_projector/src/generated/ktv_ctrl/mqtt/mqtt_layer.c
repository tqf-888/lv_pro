#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <mosquitto.h>
#include "biz_parser.h"

#define BROKER     "8.135.14.22"
#define PORT       1883
#define USERNAME   "changba"
#define PASSWORD   "djy@2026"

/* ---------- Topic 定义 ---------- */
#define TOPIC_SUB_1  "cloud/maku-boot/16666666666"
#define TOPIC_PUB_1  "device/maku-boot/16666666666"

#define TOPIC_SUB_2  "device/control/16666666666"
#define TOPIC_PUB_2  "device/maku-boot/16666666666"   // 按 cloud/device 对应规律推断的回复 Topic

#define CLIENT_ID  "c_client_16666666666"
#define QOS        1

static struct mosquitto *mosq = NULL;

static void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
        printf("[MQTT] 连接成功，订阅下行 Topic:\n");
        printf("       %s\n", TOPIC_SUB_1);
        printf("       %s\n", TOPIC_SUB_2);
        mosquitto_subscribe(mosq, NULL, TOPIC_SUB_1, QOS);
        mosquitto_subscribe(mosq, NULL, TOPIC_SUB_2, QOS);
    } else {
        printf("[MQTT] 连接失败，错误码: %d\n", rc);
    }
}

static void on_disconnect(struct mosquitto *mosq, void *obj, int rc) {
    printf("[MQTT] 连接断开，错误码: %d\n", rc);
}

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    if (message->payloadlen <= 0) return;
    
    char *payload_str = malloc(message->payloadlen + 1);
    memcpy(payload_str, message->payload, message->payloadlen);
    payload_str[message->payloadlen] = '\0';
    
    printf("[MQTT-recv<%s>] %s\n", message->topic, payload_str);

    /* ---------- 1. 业务解析（将 topic 传给业务层） ---------- */
    biz_parse(message->topic, payload_str);

    /* ---------- 2. 回复逻辑调整（防止自己收到自己发的消息） ---------- */
    const char *reply_topic = NULL;
    int need_reply = 0; // 增加一个是否需要回复的标志

    if (strcmp(message->topic, TOPIC_SUB_1) == 0) {
        // 云端下发的歌单消息：如果业务需要回复 ACK，在这里组装新的 JSON 回复
        // 假设云端不需要原样回复，这里设为 0
        need_reply = 0; 
    } else if (strcmp(message->topic, TOPIC_SUB_2) == 0) {
        // 设备控制指令：千万不要原样发回 cloud/maku-boot，否则会再次触发 on_message
        // 如果一定要回复状态，应该组装一个 {"cmd":"deviceControlAck", ...} 的独立报文
        need_reply = 0; 
    }

    if (need_reply && reply_topic != NULL) {
        int ret = mosquitto_publish(mosq, NULL, reply_topic, message->payloadlen, payload_str, QOS, false);
        if (ret == MOSQ_ERR_SUCCESS) {
            printf("[MQTT-send>%s>] %s\n", reply_topic, payload_str);
        } else {
            printf("[MQTT-send] 发送失败, 错误码: %d\n", ret);
        }
    }

    free(payload_str);
}


static void *mqtt_thread_task(void *arg) {
    mosquitto_lib_init();
    mosq = mosquitto_new(CLIENT_ID, true, NULL);

    if (!mosq) {
        printf("[MQTT] 创建实例失败\n");
        mosquitto_lib_cleanup();
        return NULL;
    }

    mosquitto_username_pw_set(mosq, USERNAME, PASSWORD);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_disconnect_callback_set(mosq, on_disconnect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, BROKER, PORT, 60) != MOSQ_ERR_SUCCESS) {
        printf("[MQTT] 无法连接到服务器 %s:%d\n", BROKER, PORT);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return NULL;
    }

    mosquitto_loop_start(mosq);

    while (1) {
        sleep(10);
    }

    return NULL;
}

void mqtt_run(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, mqtt_thread_task, NULL);
    pthread_detach(tid);
}
