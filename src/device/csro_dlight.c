#include "csro_device.h"

#ifdef DLIGHT

#define DLIGHT_RED_PIN 15
#define DLIGHT_GREEN_PIN 13
#define DLIGHT_BLUE_PIN 12
#define DLIGHT_WHITE1_PIN 14
#define DLIGHT_WHITE2_PIN 4
#define DLIGHT_PWM_PERIOD 2550

#define CFG_DLIGHT_TOPIC "csro/light/%s_%s_%d/config"
#define CFG_DLIGHT_PAYLOAD "{\"~\":\"csro/%s/%s\",\"name\":\"%s_%d_%s\",\"avty_t\":\"~/available\",\"pl_avail\":\"online\",\"pl_not_avail\":\"offline\",\"stat_t\":\"~/state\",\"stat_val_tpl\":\"{{value_json.state%d.on}}\",\"bri_stat_t\":\"~/state\",\"bri_val_tpl\":\"{{value_json.state%d.bright}}\",\"cmd_t\":\"~/set/%d\",\"pl_on\":1,\"pl_off\":0, \"bri_cmd_t\":\"~/set/bright%d\",\"opt\":\"false\",\"qos\":1}"

typedef struct
{
    uint8_t state;
    uint8_t brightness;
} dim_light;

const uint32_t pin_num[5] = {DLIGHT_RED_PIN, DLIGHT_GREEN_PIN, DLIGHT_BLUE_PIN, DLIGHT_WHITE1_PIN, DLIGHT_WHITE2_PIN};
uint32_t duties[5] = {0, 0, 0, 0, 0};
int16_t phase[5] = {0, 0, 0, 0, 0};
dim_light csro_dlight[2];

void csro_dlight_init(void)
{
    pwm_init(DLIGHT_PWM_PERIOD, duties, 5, pin_num);
    pwm_set_channel_invert(0x00);
    pwm_set_phases(phase);
    pwm_start();
}

void csro_update_dlight_state(void)
{
    cJSON *state_json = cJSON_CreateObject();
    cJSON *state1, *state2;
    cJSON_AddItemToObject(state_json, "state1", state1 = cJSON_CreateObject());
    cJSON_AddNumberToObject(state1, "on", csro_dlight[0].state);
    cJSON_AddNumberToObject(state1, "bright", csro_dlight[0].brightness);
    cJSON_AddItemToObject(state_json, "state2", state2 = cJSON_CreateObject());
    cJSON_AddNumberToObject(state2, "on", csro_dlight[1].state);
    cJSON_AddNumberToObject(state2, "bright", csro_dlight[1].brightness);
    cJSON_AddStringToObject(state_json, "time", sysinfo.time_str);
    cJSON_AddNumberToObject(state_json, "run", (int)(sysinfo.now - sysinfo.start));
    char *out = cJSON_PrintUnformatted(state_json);
    strcpy(mqttinfo.content, out);
    free(out);
    cJSON_Delete(state_json);
    sprintf(mqttinfo.pub_topic, "csro/%s/%s/state", sysinfo.mac_str, sysinfo.dev_type);
}

void csro_dlight_on_connect(esp_mqtt_client_handle_t client)
{
    sprintf(mqttinfo.sub_topic, "csro/%s/%s/set/#", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_subscribe(client, mqttinfo.sub_topic, 1);

    for (size_t i = 1; i < DLIGHT + 1; i++)
    {
        sprintf(mqttinfo.pub_topic, CFG_DLIGHT_TOPIC, sysinfo.mac_str, sysinfo.dev_type, i);
        sprintf(mqttinfo.content, CFG_DLIGHT_PAYLOAD, sysinfo.mac_str, sysinfo.dev_type, sysinfo.dev_type, i, sysinfo.mac_str, i, i, i, i);
        esp_mqtt_client_publish(client, mqttinfo.pub_topic, mqttinfo.content, 0, 1, 1);
    }
    sprintf(mqttinfo.pub_topic, "csro/%s/%s/available", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, "online", 0, 1, 1);

    csro_update_dlight_state();
    esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, mqttinfo.content, 0, 0, 1);
}

void csro_dlight_on_message(esp_mqtt_event_handle_t event)
{
    bool update = false;
    char state_topic[50];
    char bright_topic[50];
    for (size_t i = 1; i < DLIGHT + 1; i++)
    {
        sprintf(state_topic, "csro/%s/%s/set/%d", sysinfo.mac_str, sysinfo.dev_type, i);
        sprintf(bright_topic, "csro/%s/%s/set/bright%d", sysinfo.mac_str, sysinfo.dev_type, i);
        if (strncmp(state_topic, event->topic, event->topic_len) == 0)
        {
            if (strncmp("0", event->data, event->data_len) == 0)
            {
                csro_dlight[i - 1].state = 0;
                pwm_set_duty(i + 2, 0);
                pwm_start();
                update = true;
            }
            else if (strncmp("1", event->data, event->data_len) == 0)
            {
                csro_dlight[i - 1].state = 1;
                if (csro_dlight[i - 1].brightness == 0)
                {
                    csro_dlight[i - 1].brightness == 255;
                }
                pwm_set_duty(i + 2, 10 * csro_dlight[i - 1].brightness);
                pwm_start();
                update = true;
            }
        }
        else if (strncmp(bright_topic, event->topic, event->topic_len) == 0 && event->data_len < 10)
        {
            char data[10];
            uint32_t data_number;
            bzero(data, 10);
            strncpy(data, event->data, event->data_len);
            sscanf(data, "%d", &data_number);
            if (data_number <= 255)
            {
                debug("bright set to = %d\n", data_number);
                csro_dlight[i - 1].brightness = data_number;
                csro_dlight[i - 1].state = 1;
                pwm_set_duty(i + 2, 10 * csro_dlight[i - 1].brightness);
                pwm_start();
                update = true;
            }
        }
    }
    if (update)
    {
        csro_update_dlight_state();
        esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, mqttinfo.content, 0, 0, 1);
    }
}

#endif