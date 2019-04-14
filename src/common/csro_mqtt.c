#include "common.h"
#include "mdns.h"
#include "esp_log.h"

static EventGroupHandle_t wifi_event_group;
TaskHandle_t MQTT_TASK;

static const char *TAG = "MQTT_EXAMPLE";

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    }
    return ESP_OK;
}

static void mqtt_task(void *pvParameters)
{
    static int count = 0;
    struct ip4_addr addr;
    addr.addr = 0;
    mdns_init();
    mdns_hostname_set("Derek_ESP8266");
    esp_err_t err = mdns_query_a("csro-home-server", 3000, &addr);
    while (err != 0)
    {
        err = mdns_query_a("csro-home-server", 3000, &addr);
    }
    debug(IPSTR, IP2STR(&addr));

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://192.168.2.51",
        .event_handle = mqtt_event_handler,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

    while (1)
    {
        printf("Mqtt task count = %d\n", count);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        count++;
    }
    vTaskDelete(NULL);
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    if (event->event_id == SYSTEM_EVENT_STA_START)
    {
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
    nvs_handle handle;
    size_t len = 0;
    wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    esp_event_loop_init(wifi_event_handler, NULL);

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);

    nvs_open("system", NVS_READONLY, &handle);
    nvs_get_str(handle, "ssid", NULL, &len);
    nvs_get_str(handle, "ssid", sysinfo.router_ssid, &len);
    nvs_get_str(handle, "pass", NULL, &len);
    nvs_get_str(handle, "pass", sysinfo.router_pass, &len);
    nvs_close(handle);
    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.sta.ssid, (char *)sysinfo.router_ssid);
    strcpy((char *)wifi_config.sta.password, (char *)sysinfo.router_pass);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}