#include "common.h"
#include "mdns.h"
#include "esp_log.h"

#define LIGHT_PAYLOAD "{\"~\" : \"csro/%s/nlight3\", \"name\" : \"nlight3_%d\", \"stat_t\" : \"~/state\", \"stat_val_tpl\" : \"{{value_json.state[%d]}}\", \"cmd_t\" : \"~/set/%d\", \"pl_on\" : \"on\", \"pl_off\" : \"off\"}"

static EventGroupHandle_t wifi_event_group;
TaskHandle_t MQTT_TASK;

esp_mqtt_client_handle_t mqtt_client;

static void publish_hass_discovery_config(void)
{
    sprintf(mqttinfo.pub_topic, "csro/light/%s_nlight3_%d/config", sysinfo.mac_str, 1);
    sprintf(mqttinfo.content, LIGHT_PAYLOAD, sysinfo.mac_str, 1, 0, 1);
    if (csro_client_is_idle(mqtt_client))
    {
        debug("pub_config\n");
        esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, mqttinfo.content, 0, 1, 0);
    }
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    if (event->event_id == MQTT_EVENT_CONNECTED)
    {
        publish_hass_discovery_config();
        debug("MQTT_EVENT_CONNECTED\n");
    }
    else if (event->event_id == MQTT_EVENT_DISCONNECTED)
    {
        debug("MQTT_EVENT_DISCONNECTED\n");
    }
    else if (event->event_id == MQTT_EVENT_SUBSCRIBED)
    {
    }
    else if (event->event_id == MQTT_EVENT_UNSUBSCRIBED)
    {
    }
    else if (event->event_id == MQTT_EVENT_PUBLISHED)
    {
        debug("MQTT_EVENT_PUBLISHED, msg_id=%d\n", event->msg_id);
    }
    else if (event->event_id == MQTT_EVENT_DATA)
    {
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
    }
    else if (event->event_id == MQTT_EVENT_ERROR)
    {
        debug("MQTT_EVENT_ERROR\n");
    }
    return ESP_OK;
}

static void mqtt_task(void *pvParameters)
{
    static int count = 0;
    struct ip4_addr addr;
    addr.addr = 0;

    mdns_init();
    esp_err_t err = mdns_query_a("csro-home-server", 3000, &addr);
    while (err != 0)
    {
        err = mdns_query_a("csro-home-server", 3000, &addr);
    }
    sprintf(mqttinfo.broker, "mqtt://%d.%d.%d.%d", ip4_addr1_16(&addr), ip4_addr2_16(&addr), ip4_addr3_16(&addr), ip4_addr4_16(&addr));

    esp_mqtt_client_config_t mqtt_cfg = {
        .event_handle = mqtt_event_handler,
        .client_id = mqttinfo.id,
        .username = mqttinfo.name,
        .password = mqttinfo.pass,
        .uri = mqttinfo.broker,
        .keepalive = 60,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);

    char msg[30];

    while (1)
    {
        //printf("Mqtt task count = %d\n", count);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if (csro_client_is_idle(mqtt_client))
        {
            sprintf(msg, "msg count = %d\n", count);
            esp_mqtt_client_publish(mqtt_client, "esp-mqtt/test/", msg, 0, 0, 0);
            count++;
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