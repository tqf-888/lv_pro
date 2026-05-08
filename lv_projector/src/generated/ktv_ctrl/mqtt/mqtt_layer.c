#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <mosquitto.h>
#include "cJSON.h"
#include "biz_parser.h"

#define BROKER     "8.135.14.22"
#define PORT       1883
#define USERNAME   "changba"
#define PASSWORD   "djy@2026"

/* ---------- Topic 定义 ---------- */
#define TOPIC_SUB_1  "cloud/maku-boot/16666666666"
#define TOPIC_PUB_1  "device/maku-boot/16666666666"

#define TOPIC_SUB_2  "device/control/16666666666"
#define TOPIC_PUB_2  "device/maku-boot/16666666666"

#define CLIENT_ID  "c_client_16666666666"
#define QOS        1

static struct mosquitto *mosq = NULL;

static long long mqtt_now_ms(void) {
    return (long long)time(NULL) * 1000LL;
}

static int mqtt_play_status_from_action(const char *action) {
    if (action == NULL) return 0;
    if (strcmp(action, "pause") == 0) return 2;
    if (strcmp(action, "play") == 0) return 1;
    if (strcmp(action, "continue") == 0) return 1;
    if (strcmp(action, "next") == 0) return 1;
    if (strcmp(action, "replay") == 0) return 1;
    return 0;
}

static void mqtt_publish_device_control_ack(struct mosquitto *mosq, const char *payload_str) {
    cJSON *root = cJSON_Parse(payload_str);
    if (root == NULL) return;

    cJSON *cmd = cJSON_GetObjectItem(root, "cmd");
    cJSON *msg_id = cJSON_GetObjectItem(root, "msgId");
    cJSON *payload = cJSON_GetObjectItem(root, "payload");
    cJSON *action = payload ? cJSON_GetObjectItem(payload, "action") : NULL;

    if (!cJSON_IsString(cmd) || strcmp(cmd->valuestring, "deviceControl") != 0 ||
        !cJSON_IsString(msg_id) || !cJSON_IsString(action)) {
        cJSON_Delete(root);
        return;
    }

    int play_status = mqtt_play_status_from_action(action->valuestring);

    cJSON *reply = cJSON_CreateObject();
    cJSON *reply_payload = cJSON_CreateObject();
    if (reply == NULL || reply_payload == NULL) {
        cJSON_Delete(reply);
        cJSON_Delete(reply_payload);
        cJSON_Delete(root);
        return;
    }

    cJSON_AddStringToObject(reply, "cmd", "deviceControl");
    cJSON_AddNumberToObject(reply, "t", mqtt_now_ms());
    cJSON_AddStringToObject(reply, "msgId", msg_id->valuestring);
    cJSON_AddNumberToObject(reply, "responseCode", 0);
    cJSON_AddStringToObject(reply_payload, "action", action->valuestring);
    cJSON_AddNumberToObject(reply_payload, "playStatus", play_status);
    cJSON_AddItemToObject(reply, "payload", reply_payload);

    char *reply_str = cJSON_PrintUnformatted(reply);
    if (reply_str != NULL) {
        int ret = mosquitto_publish(mosq, NULL, TOPIC_PUB_2,
                                    (int)strlen(reply_str), reply_str,
                                    QOS, 0);
        if (ret == MOSQ_ERR_SUCCESS) {
            printf("[MQTT-send>%s>] %s\n", TOPIC_PUB_2, reply_str);
        } else {
            printf("[MQTT-send] 发送失败, 错误码: %d\n", ret);
        }
        free(reply_str);
    }

    cJSON_Delete(reply);
    cJSON_Delete(root);
}

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
    if (payload_str == NULL) return;

    memcpy(payload_str, message->payload, message->payloadlen);
    payload_str[message->payloadlen] = '\0';

    printf("[MQTT-recv<%s>] %s\n", message->topic, payload_str);

    biz_parse(message->topic, payload_str);

    if (strcmp(message->topic, TOPIC_SUB_2) == 0) {
        mqtt_publish_device_control_ack(mosq, payload_str);
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
