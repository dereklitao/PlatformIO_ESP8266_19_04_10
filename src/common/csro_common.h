#ifndef CSRO_COMMON_H_
#define CSRO_COMMON_H_

#include "esp_system.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "gpio.h"
#include "uart.h"
#include "pwm.h"
#include "esp_smartconfig.h"
#include "string.h"
#include "nvs_flash.h"
#include "gpio.h"
#include "sys\socket.h"
#include "netdb.h"
#include "mqtt\esp-mqtt\include\mqtt_client.h"
#include "sntp\sntp.h"
#include "time.h"

#define DLIGHT

// #define     NLIGHT               3
// #define     DLIGHT               1
// #define     MOTOR                2
// #define     AIR_MONITOR
// #define     AIR_SYSTEM

#define DEBUG
#ifdef DEBUG
#define debug(format, ...) printf(format, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif

#define MQTT_BUFFER_LENGTH 1000
#define MQTT_TOPIC_LENGTH 100
#define MQTT_NAME_ID_LENGTH 50

typedef enum
{
    IDLE = 0,
    SMARTCONFIG = 1,
    SMARTCONFIG_TIMEOUT = 2,
    NORMAL_START_NOWIFI = 3,
    NORMAL_START_NOSERVER = 4,
    NORMAL_START_OK = 5,
    RESET_PENDING = 6
} csro_status;

typedef struct
{
    csro_status status;
    uint8_t router_flag;
    char router_ssid[50];
    char router_pass[50];
    uint8_t mac[6];
    uint8_t ip[4];
    char host_name[20];
    char dev_type[20];
    char mac_str[20];
    char ip_str[20];
    uint32_t power_on_count;
    uint32_t wifi_conn_count;
    uint32_t serv_conn_count;
    bool time_sync;
    time_t start;
    time_t now;
    struct tm timeinfo;
    char time_str[64];
} csro_system;

typedef struct
{
    char id[MQTT_NAME_ID_LENGTH];
    char name[MQTT_NAME_ID_LENGTH];
    char pass[MQTT_NAME_ID_LENGTH];
    char sub_topic[MQTT_TOPIC_LENGTH];
    char pub_topic[MQTT_TOPIC_LENGTH];
    char content[MQTT_BUFFER_LENGTH];
    char lwt_topic[MQTT_TOPIC_LENGTH];
    char broker[50];
    char prefix[50];
    uint16_t interval;
} csro_mqtt;

extern csro_system sysinfo;
extern csro_mqtt mqttinfo;
extern esp_mqtt_client_handle_t mqtt_client;

//common.c
void csro_system_get_info(void);

//csro_start_config.c
void csro_start_smart_config(void);

//csro_mqtt.c
void csro_start_mqtt(void);

#endif