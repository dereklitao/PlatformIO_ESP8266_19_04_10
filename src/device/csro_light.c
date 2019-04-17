#include "csro_device.h"

#ifdef NLIGHT

#define CFG_TOPIC "csro/light/%s_%s_%d/config"
#define CFG_PAYLOAD "{\"~\":\"csro/%s/%s\",\"name\":\"%s_%d_%s\",\"avty_t\":\"~/available\",\"pl_avail\":\"online\",\"pl_not_avail\":\"offline\",\"stat_t\":\"~/state\",\"stat_val_tpl\":\"{{value_json.state[%d]}}\",\"cmd_t\":\"~/set/%d\",\"pl_on\":1,\"pl_off\":0, \"opt\":\"false\",\"qos\":1}"

int light_state[3];

static void update_nlight_state(void)
{
    cJSON *state_json = cJSON_CreateObject();
    cJSON_AddItemToObject(state_json, "state", cJSON_CreateIntArray(light_state, 3));
    char *out = cJSON_PrintUnformatted(state_json);
    strcpy(mqttinfo.content, out);
    free(out);
    cJSON_Delete(state_json);

    sprintf(mqttinfo.pub_topic, "csro/%s/%s/state", sysinfo.mac_str, sysinfo.dev_type);
    xSemaphoreGive(state_msg_semaphore);
}

void csro_nlight_on_connect(esp_mqtt_client_handle_t client)
{
    sprintf(mqttinfo.sub_topic, "csro/%s/%s/set/#", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_subscribe(client, mqttinfo.sub_topic, 1);

    for (size_t i = 1; i < NLIGHT + 1; i++)
    {
        sprintf(mqttinfo.pub_topic, CFG_TOPIC, sysinfo.mac_str, sysinfo.dev_type, i);
        sprintf(mqttinfo.content, CFG_PAYLOAD, sysinfo.mac_str, sysinfo.dev_type, sysinfo.dev_type, i, sysinfo.mac_str, i - 1, i);
        esp_mqtt_client_publish(client, mqttinfo.pub_topic, mqttinfo.content, 0, 1, 1);
    }
    sprintf(mqttinfo.pub_topic, "csro/%s/%s/available", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_publish(client, mqttinfo.pub_topic, "online", 0, 1, 1);
    update_nlight_state();
}

void csro_nlight_on_message(esp_mqtt_event_handle_t event)
{
    bool update = false;
    char topic[100];
    for (size_t i = 1; i < NLIGHT + 1; i++)
    {
        sprintf(topic, "csro/%s/%s/set/%d", sysinfo.mac_str, sysinfo.dev_type, i);
        if (strncmp(topic, event->topic, event->topic_len) == 0)
        {
            if (strncmp("0", event->data, event->data_len) == 0)
            {
                light_state[i - 1] = 0;
                update = true;
            }
            else if (strncmp("1", event->data, event->data_len) == 0)
            {
                light_state[i - 1] = 1;
                update = true;
            }
        }
    }
    if (update)
    {
        update_nlight_state();
    }
}

#endif