#include "csro_device.h"

#ifdef RGBLIGHT

#define RGBLIGHT_RED_PIN 15
#define RGBLIGHT_GREEN_PIN 13
#define RGBLIGHT_BLUE_PIN 12
#define RGBLIGHT_PWM_PERIOD 2550

#define CFG_RGBLIGHT_TOPIC "csro/light/%s_%s/config"
#define CFG_RGBLIGHT_PAYLOAD "{\"~\":\"csro/%s/%s\",\"name\":\"%s_%s\",\"avty_t\":\"~/available\",\"pl_avail\":\"online\",\"pl_not_avail\":\"offline\",\"stat_t\":\"~/state\",\"stat_val_tpl\":\"{{value_json.state.on}}\",\"bri_stat_t\":\"~/state\",\"bri_val_tpl\":\"{{value_json.state.bright}}\",\"bri_scl\":10,\"cmd_t\":\"~/set\",\"pl_on\":1,\"pl_off\":0, \"bri_cmd_t\":\"~/set/bright\",\"rgb_cmd_t\":\"~/set/rgb\", \"rgb_stat_t\":\"~/state\",\"rgb_val_tpl\":\"{{value_json.state.rgb}}\",\"opt\":\"false\",\"qos\":1}"

typedef struct
{
    uint8_t state;
    uint8_t bright;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} csro_rgb_light;

csro_rgb_light rgb_light;
const uint32_t rgb_pin[3] = {RGBLIGHT_RED_PIN, RGBLIGHT_GREEN_PIN, RGBLIGHT_BLUE_PIN};
uint32_t rgb_duties[3] = {0, 0, 0};
int16_t rgb_phases[3] = {0, 0, 0};

static void rgblight_update_state(void)
{
    cJSON *state_json = cJSON_CreateObject();
    cJSON *rgbstate;
    int rgb_value[3];

    cJSON_AddItemToObject(state_json, "state", rgbstate = cJSON_CreateObject());
    cJSON_AddNumberToObject(rgbstate, "on", rgb_light.state);
    cJSON_AddNumberToObject(rgbstate, "bright", rgb_light.bright);
    rgb_value[0] = rgb_light.red;
    rgb_value[1] = rgb_light.green;
    rgb_value[2] = rgb_light.blue;
    cJSON_AddItemToObject(rgbstate, "rgb", cJSON_CreateIntArray(rgb_value, 3));
    cJSON_AddStringToObject(state_json, "time", sysinfo.time_str);
    cJSON_AddNumberToObject(state_json, "run", (int)(sysinfo.now - sysinfo.start));
    cJSON_AddNumberToObject(state_json, "rssi", esp_wifi_get_ap_rssi());
    char *out = cJSON_PrintUnformatted(state_json);
    strcpy(mqttinfo.content, out);
    free(out);
    cJSON_Delete(state_json);
    sprintf(mqttinfo.pub_topic, "csro/%s/%s/state", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, mqttinfo.content, 0, 0, 1);
}

static void rgb_light_pwm_task(void *pvParameters)
{
    uint32_t duty[3];
    while (true)
    {
        bool update_pwm = false;
        pwm_get_duty(0, &duty[0]);
        pwm_get_duty(1, &duty[1]);
        pwm_get_duty(2, &duty[2]);
        if (rgb_light.state == 0)
        {
        }
        if (update_pwm)
        {
            pwm_start();
        }
        vTaskDelay(15 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void csro_rgblight_init(void)
{
    pwm_init(RGBLIGHT_PWM_PERIOD, rgb_duties, 3, rgb_pin);
    pwm_set_channel_invert(0x00);
    pwm_set_phases(rgb_phases);
    pwm_start();
    xTaskCreate(rgb_light_pwm_task, "rgb_light_pwm_task", 2048, NULL, 5, NULL);
}

void csro_rgblight_on_connect(esp_mqtt_client_handle_t client)
{
    sprintf(mqttinfo.sub_topic, "csro/%s/%s/set/#", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_subscribe(client, mqttinfo.sub_topic, 1);

    sprintf(mqttinfo.pub_topic, CFG_RGBLIGHT_TOPIC, sysinfo.mac_str, sysinfo.dev_type);
    sprintf(mqttinfo.content, CFG_RGBLIGHT_PAYLOAD, sysinfo.mac_str, sysinfo.dev_type, sysinfo.dev_type, sysinfo.mac_str);
    esp_mqtt_client_publish(client, mqttinfo.pub_topic, mqttinfo.content, 0, 1, 1);

    sprintf(mqttinfo.pub_topic, "csro/%s/%s/available", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, "online", 0, 1, 1);

    rgblight_update_state();
}

void csro_rgblight_on_message(esp_mqtt_event_handle_t event)
{
    bool update = false;
    char set_state_topic[50];
    char set_bright_topic[50];
    char set_rgb_topic[50];

    sprintf(set_state_topic, "csro/%s/%s/set", sysinfo.mac_str, sysinfo.dev_type);
    sprintf(set_bright_topic, "csro/%s/%s/set/bright", sysinfo.mac_str, sysinfo.dev_type);
    sprintf(set_rgb_topic, "csro/%s/%s/set/rgb", sysinfo.mac_str, sysinfo.dev_type);
    if (event->data_len == 1 && (strncmp(set_state_topic, event->topic, event->topic_len) == 0))
    {
        if (strncmp("0", event->data, event->data_len) == 0)
        {
            rgb_light.state = 0;
            update = true;
        }
        else if (strncmp("1", event->data, event->data_len) == 0)
        {
            if (rgb_light.bright == 0)
            {
                rgb_light.bright = 10;
            }
            rgb_light.state = 1;
            update = true;
        }
    }
    else if ((event->data_len <= 3) && (strncmp(set_bright_topic, event->topic, event->topic_len) == 0))
    {
        char payload[5] = {0};
        strncpy(payload, event->data, event->data_len);
        uint32_t data_number;
        sscanf(payload, "%d", &data_number);
        if (data_number <= 255)
        {
            rgb_light.bright = data_number;
            rgb_light.state = 1;
            update = true;
        }
    }
    else if ((event->data_len <= 12) && (strncmp(set_rgb_topic, event->topic, event->topic_len) == 0))
    {
        char payload[15] = {0};
        strncpy(payload, event->data, event->data_len);
        uint32_t values[3];
        sscanf(payload, "%d,%d,%d", &values[0], &values[1], &values[2]);
        if (values[0] <= 255)
        {
            rgb_light.red = values[0];
            update = true;
        }
        if (values[1] <= 255)
        {
            rgb_light.green = values[1];
            update = true;
        }
        if (values[2] <= 255)
        {
            rgb_light.blue = values[2];
            update = true;
        }
    }
    if (update)
    {
        rgblight_update_state();
    }
}

#endif