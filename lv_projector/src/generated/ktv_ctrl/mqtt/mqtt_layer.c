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
#define DEVICE_SN  "16666666666"
#define QOS        1
#define MQTT_RETRY_INITIAL_SEC 2
#define MQTT_RETRY_MAX_SEC     30

static struct mosquitto *mosq = NULL;
static pthread_mutex_t g_mqtt_start_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_mqtt_started = 0;

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

static void mqtt_add_json_string_or_number(cJSON *object, const char *name, cJSON *value) {
    if (object == NULL || name == NULL || value == NULL) return;
    if (cJSON_IsString(value)) {
        cJSON_AddStringToObject(object, name, value->valuestring);
    } else if (cJSON_IsNumber(value)) {
        cJSON_AddNumberToObject(object, name, value->valuedouble);
    }
}

static void mqtt_publish_json(struct mosquitto *mosq, const char *topic, cJSON *json) {
    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str != NULL) {
        int ret = mosquitto_publish(mosq, NULL, topic,
                                    (int)strlen(json_str), json_str,
                                    QOS, 0);
        if (ret == MOSQ_ERR_SUCCESS) {
            printf("[MQTT-send>%s>] %s\n", topic, json_str);
        } else {
            printf("[MQTT-send] 发送失败, 错误码: %d\n", ret);
        }
        free(json_str);
    }
}

static void mqtt_publish_play_status_update(struct mosquitto *mosq,
                                            cJSON *msg_id,
                                            cJSON *payload,
                                            cJSON *action,
                                            cJSON *song_id,
                                            int play_status) {
    cJSON *device_sn = payload ? cJSON_GetObjectItem(payload, "deviceSn") : NULL;
    cJSON *update = cJSON_CreateObject();
    cJSON *update_payload = cJSON_CreateObject();
    if (update == NULL || update_payload == NULL) {
        cJSON_Delete(update);
        cJSON_Delete(update_payload);
        return;
    }

    cJSON_AddStringToObject(update, "cmd", "songlistUpdate");
    cJSON_AddNumberToObject(update, "t", mqtt_now_ms());
    if (cJSON_IsString(msg_id)) {
        cJSON_AddStringToObject(update, "msgId", msg_id->valuestring);
    }

    cJSON_AddStringToObject(update_payload, "action", "control");
    cJSON_AddStringToObject(update_payload, "deviceSn",
                            cJSON_IsString(device_sn) ? device_sn->valuestring : DEVICE_SN);
    cJSON_AddStringToObject(update_payload, "controlAction", action->valuestring);
    mqtt_add_json_string_or_number(update_payload, "songId", song_id);
    cJSON_AddNumberToObject(update_payload, "playStatus", play_status);
    cJSON_AddItemToObject(update, "payload", update_payload);

    mqtt_publish_json(mosq, TOPIC_PUB_1, update);
    cJSON_Delete(update);
}

static void mqtt_publish_device_control_ack(struct mosquitto *mosq, const char *payload_str) {
    cJSON *root = cJSON_Parse(payload_str);
    if (root == NULL) return;

    cJSON *cmd = cJSON_GetObjectItem(root, "cmd");
    cJSON *msg_id = cJSON_GetObjectItem(root, "msgId");
    cJSON *payload = cJSON_GetObjectItem(root, "payload");
    cJSON *action = payload ? cJSON_GetObjectItem(payload, "action") : NULL;
    cJSON *song_id = payload ? cJSON_GetObjectItem(payload, "songId") : NULL;

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
    mqtt_add_json_string_or_number(reply_payload, "songId", song_id);
    cJSON_AddNumberToObject(reply_payload, "playStatus", play_status);
    cJSON_AddItemToObject(reply, "payload", reply_payload);

    mqtt_publish_json(mosq, TOPIC_PUB_2, reply);
    mqtt_publish_play_status_update(mosq, msg_id, payload, action, song_id, play_status);

    cJSON_Delete(reply);
    cJSON_Delete(root);
}

int mqtt_publish_song_finished(const char *song_id, const char *reason) {
    if (mosq == NULL) {
        printf("[MQTT-songFinished] MQTT 未初始化，发送失败\n");
        return MOSQ_ERR_INVAL;
    }
    if (song_id == NULL || song_id[0] == '\0') {
        printf("[MQTT-songFinished] songId 为空，发送失败\n");
        return MOSQ_ERR_INVAL;
    }

    long long now = mqtt_now_ms();
    char msg_id[64];
    snprintf(msg_id, sizeof(msg_id), "sf_%lld", now);

    cJSON *root = cJSON_CreateObject();
    cJSON *payload = cJSON_CreateObject();
    if (root == NULL || payload == NULL) {
        cJSON_Delete(root);
        cJSON_Delete(payload);
        return MOSQ_ERR_NOMEM;
    }

    cJSON_AddStringToObject(root, "cmd", "songFinished");
    cJSON_AddStringToObject(root, "msgId", msg_id);
    cJSON_AddNumberToObject(root, "t", now);
    cJSON_AddNumberToObject(root, "responseCode", 0);
    cJSON_AddStringToObject(payload, "songId", song_id);
    cJSON_AddStringToObject(payload, "reason",
                            (reason != NULL && reason[0] != '\0') ? reason : "skipped");
    cJSON_AddItemToObject(root, "payload", payload);

    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        cJSON_Delete(root);
        return MOSQ_ERR_NOMEM;
    }

    int ret = mosquitto_publish(mosq, NULL, TOPIC_PUB_1,
                                (int)strlen(json_str), json_str,
                                QOS, 0);
    if (ret == MOSQ_ERR_SUCCESS) {
        printf("[MQTT-send>%s>] %s\n", TOPIC_PUB_1, json_str);
    } else {
        printf("[MQTT-songFinished] 发送失败, 错误码: %d (%s)\n",
               ret, mosquitto_strerror(ret));
    }

    free(json_str);
    cJSON_Delete(root);
    return ret;
}

static void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
        printf("[MQTT] 连接成功，订阅下行 Topic:\n");
        printf("       %s\n", TOPIC_SUB_1);
        printf("       %s\n", TOPIC_SUB_2);
        mosquitto_subscribe(mosq, NULL, TOPIC_SUB_1, QOS);
        mosquitto_subscribe(mosq, NULL, TOPIC_SUB_2, QOS);
    } else {
        printf("[MQTT] 连接失败，错误码: %d (%s)\n", rc, mosquitto_connack_string(rc));
    }
}

static void on_disconnect(struct mosquitto *mosq, void *obj, int rc) {
    printf("[MQTT] 连接断开，错误码: %d (%s)\n", rc, mosquitto_strerror(rc));
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
    int retry_sec = MQTT_RETRY_INITIAL_SEC;
    int ret;

    (void)arg;

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
    mosquitto_reconnect_delay_set(mosq, MQTT_RETRY_INITIAL_SEC, MQTT_RETRY_MAX_SEC, true);

    while (1) {
        ret = mosquitto_connect(mosq, BROKER, PORT, 60);
        if (ret == MOSQ_ERR_SUCCESS) {
            break;
        }

        printf("[MQTT] 无法连接到服务器 %s:%d, 错误码: %d (%s), %d 秒后重试\n",
               BROKER, PORT, ret, mosquitto_strerror(ret), retry_sec);
        sleep(retry_sec);
        if (retry_sec < MQTT_RETRY_MAX_SEC) {
            retry_sec *= 2;
            if (retry_sec > MQTT_RETRY_MAX_SEC) {
                retry_sec = MQTT_RETRY_MAX_SEC;
            }
        }
    }

    while (1) {
        ret = mosquitto_loop_forever(mosq, -1, 1);
        printf("[MQTT] loop 退出，错误码: %d (%s), %d 秒后重连\n",
               ret, mosquitto_strerror(ret), MQTT_RETRY_INITIAL_SEC);
        sleep(MQTT_RETRY_INITIAL_SEC);
    }

    return NULL;
}

void mqtt_run(void) {
    pthread_t tid;
    pthread_mutex_lock(&g_mqtt_start_lock);
    if (g_mqtt_started) {
        pthread_mutex_unlock(&g_mqtt_start_lock);
        return;
    }
    g_mqtt_started = 1;
    pthread_mutex_unlock(&g_mqtt_start_lock);

    if (pthread_create(&tid, NULL, mqtt_thread_task, NULL) != 0) {
        pthread_mutex_lock(&g_mqtt_start_lock);
        g_mqtt_started = 0;
        pthread_mutex_unlock(&g_mqtt_start_lock);
        printf("[MQTT] 创建线程失败\n");
        return;
    }
    pthread_detach(tid);
}

void mqtt_start_service(void) {
    mqtt_run();
}
