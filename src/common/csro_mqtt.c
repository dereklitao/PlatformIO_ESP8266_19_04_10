#include "csro_common.h"
#include "mdns.h"
#include "esp_log.h"
#include "./device/csro_device.h"

static EventGroupHandle_t wifi_event_group;
TaskHandle_t MQTT_TASK;
esp_mqtt_client_handle_t mqtt_client;

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    if (event->event_id == MQTT_EVENT_CONNECTED)
    {
        debug("Mqtt Connected!\n");
        csro_device_on_connect(client);
    }
    else if (event->event_id == MQTT_EVENT_DATA)
    {
        csro_device_on_message(event);
    }
    return ESP_OK;
}

static void mqtt_task(void *pvParameters)
{
    struct ip4_addr addr;
    addr.addr = 0;

    mdns_init();
    esp_err_t err = mdns_query_a("csro-home-server", 3000, &addr);
    while (err != 0)
    {
        err = mdns_query_a("csro-home-server", 3000, &addr);
    }
    sprintf(mqttinfo.broker, "mqtt://%d.%d.%d.%d", ip4_addr1_16(&addr), ip4_addr2_16(&addr), ip4_addr3_16(&addr), ip4_addr4_16(&addr));
    sprintf(mqttinfo.lwt_topic, "csro/%s/%s/available", sysinfo.mac_str, sysinfo.dev_type);

    esp_mqtt_client_config_t mqtt_cfg = {
        .event_handle = mqtt_event_handler,
        .client_id = mqttinfo.id,
        .username = mqttinfo.name,
        .password = mqttinfo.pass,
        .uri = mqttinfo.broker,
        .keepalive = 60,
        .lwt_topic = mqttinfo.lwt_topic,
        .lwt_msg = "offline",
        .lwt_retain = 1,
        .lwt_qos = 1,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
    while (true)
    {
        if (xSemaphoreTake(state_msg_semaphore, portMAX_DELAY) == pdTRUE)
        {
            esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, mqttinfo.content, 0, 0, 1);
        }
    }
    vTaskDelete(NULL);
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    if (event->event_id == SYSTEM_EVENT_STA_START)
    {
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, sysinfo.host_name);
        esp_wifi_connect();
    }
    else if (event->event_id == SYSTEM_EVENT_STA_GOT_IP)
    {
        if (MQTT_TASK == NULL)
        {
            xTaskCreate(mqtt_task, "mqtt_task", 4096, NULL, 6, &MQTT_TASK);
        }
    }
    else if (event->event_id == SYSTEM_EVENT_STA_DISCONNECTED)
    {
        if (MQTT_TASK != NULL)
        {
            vTaskDelete(MQTT_TASK);
            MQTT_TASK = NULL;
        }
        esp_wifi_connect();
    }
    mdns_handle_system_event(ctx, event);
    return ESP_OK;
}

void csro_start_mqtt(void)
{
    csro_system_get_info();

    state_msg_semaphore = xSemaphoreCreateBinary();
    xSemaphoreTake(state_msg_semaphore, 10);

    wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    esp_event_loop_init(wifi_event_handler, NULL);

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);

    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.sta.ssid, (char *)sysinfo.router_ssid);
    strcpy((char *)wifi_config.sta.password, (char *)sysinfo.router_pass);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}