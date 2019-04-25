#include "csro_device.h"

#ifdef DLIGHT

#define DLIGHT_DLIGHT1_PIN 14
#define DLIGHT_DLIGHT2_PIN 4
#define DLIGHT_PWM_PERIOD 2550

#define CFG_DLIGHT_TOPIC "csro/light/%s_%s_%d/config"
#define CFG_DLIGHT_PAYLOAD "{\"~\":\"csro/%s/%s\",\"name\":\"%s_%d_%s\",\"avty_t\":\"~/available\",\"pl_avail\":\"online\",\"pl_not_avail\":\"offline\",\"stat_t\":\"~/state\",\"stat_val_tpl\":\"{{value_json.state%d.on}}\",\"bri_stat_t\":\"~/state\",\"bri_val_tpl\":\"{{value_json.state%d.bright}}\",\"cmd_t\":\"~/set/%d\",\"pl_on\":1,\"pl_off\":0, \"bri_cmd_t\":\"~/set/bright%d\",\"opt\":\"false\",\"qos\":1}"

typedef struct
{
    uint8_t state;
    uint8_t bright;
} csro_dim_light;

csro_dim_light dim_light[2];

const uint32_t dim_pin[2] = {DLIGHT_DLIGHT1_PIN, DLIGHT_DLIGHT2_PIN};
uint32_t dim_duties[2] = {0, 0};
int16_t dim_phases[2] = {0, 0};

static void dlight_update_state(void)
{
    cJSON *state_json = cJSON_CreateObject();
    cJSON *state1, *state2;
    cJSON_AddItemToObject(state_json, "state1", state1 = cJSON_CreateObject());
    cJSON_AddNumberToObject(state1, "on", dim_light[0].state);
    cJSON_AddNumberToObject(state1, "bright", dim_light[0].bright);
    cJSON_AddItemToObject(state_json, "state2", state2 = cJSON_CreateObject());
    cJSON_AddNumberToObject(state2, "on", dim_light[1].state);
    cJSON_AddNumberToObject(state2, "bright", dim_light[1].bright);
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

static void dim_light_pwm_task(void *pvParameters)
{
    uint32_t duty[2];
    while (true)
    {
        bool update_pwm = false;
        for (size_t i = 0; i < 2; i++)
        {
            pwm_get_duty(i, &duty[i]);
            if (dim_light[i].state == 0)
            {
                if (duty[i] != 0)
                {

                    pwm_set_duty(i, duty[i] - 10);
                    update_pwm = true;
                }
            }
            else
            {
                if (duty[i] > dim_light[i].bright * 10)
                {
                    pwm_set_duty(i, duty[i] - 10);
                    update_pwm = true;
                }
                else if (duty[i] < dim_light[i].bright * 10)
                {
                    pwm_set_duty(i, duty[i] + 10);
                    update_pwm = true;
                }
            }
        }
        if (update_pwm)
        {
            pwm_start();
        }
        vTaskDelay(15 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void csro_dlight_init(void)
{
    pwm_init(DLIGHT_PWM_PERIOD, dim_duties, 2, dim_pin);
    pwm_set_channel_invert(0x00);
    pwm_set_phases(dim_phases);
    pwm_start();
    xTaskCreate(dim_light_pwm_task, "dim_light_pwm_task", 2048, NULL, 5, NULL);
}

void csro_dlight_on_connect(esp_mqtt_client_handle_t client)
{
    sprintf(mqttinfo.sub_topic, "csro/%s/%s/set/#", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_subscribe(client, mqttinfo.sub_topic, 1);

    for (size_t i = 0; i < DLIGHT; i++)
    {
        sprintf(mqttinfo.pub_topic, CFG_DLIGHT_TOPIC, sysinfo.mac_str, sysinfo.dev_type, i + 1);
        sprintf(mqttinfo.content, CFG_DLIGHT_PAYLOAD, sysinfo.mac_str, sysinfo.dev_type, sysinfo.dev_type, i + 1, sysinfo.mac_str, i + 1, i + 1, i + 1, i + 1);
        esp_mqtt_client_publish(client, mqttinfo.pub_topic, mqttinfo.content, 0, 1, 1);
    }
    sprintf(mqttinfo.pub_topic, "csro/%s/%s/available", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, "online", 0, 1, 1);

    dlight_update_state();
}

void csro_dlight_on_message(esp_mqtt_event_handle_t event)
{
    bool update = false;
    char set_state_topic[50];
    char set_bright_topic[50];
    for (size_t i = 0; i < DLIGHT; i++)
    {
        sprintf(set_state_topic, "csro/%s/%s/set/%d", sysinfo.mac_str, sysinfo.dev_type, i + 1);
        sprintf(set_bright_topic, "csro/%s/%s/set/bright%d", sysinfo.mac_str, sysinfo.dev_type, i + 1);
        if (event->data_len == 1 && (strncmp(set_state_topic, event->topic, event->topic_len) == 0))
        {
            if (strncmp("0", event->data, event->data_len) == 0)
            {
                dim_light[i].state = 0;
                update = true;
            }
            else if (strncmp("1", event->data, event->data_len) == 0)
            {
                if (dim_light[i].bright == 0)
                {
                    dim_light[i].bright = 255;
                }
                dim_light[i].state = 1;
                update = true;
            }
        }
        else if ((event->data_len <= 3) && (strncmp(set_bright_topic, event->topic, event->topic_len) == 0))
        {
            char payload[5] = {0, 0, 0, 0, 0};
            strncpy(payload, event->data, event->data_len);
            uint32_t data_number;
            sscanf(payload, "%d", &data_number);
            if (data_number <= 255)
            {
                dim_light[i].bright = data_number;
                dim_light[i].state = 1;
                update = true;
            }
        }
    }
    if (update)
    {
        dlight_update_state();
    }
}

#endif