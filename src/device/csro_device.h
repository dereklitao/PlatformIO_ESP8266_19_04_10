#ifndef CSRO_DEVICE_H_
#define CSRO_DEVICE_H_

#include "./common/csro_common.h"
#include "./device/csro_device.h"
#include "cJSON.h"

//csro_device.c
void csro_device_init(void);
void csro_device_on_connect(esp_mqtt_client_handle_t client);
void csro_device_on_message(esp_mqtt_event_handle_t event);

//csro_nlight.c
void csro_nlight_init(void);
void csro_nlight_on_connect(esp_mqtt_client_handle_t client);
void csro_nlight_on_message(esp_mqtt_event_handle_t event);

//csro_dlight.c
void csro_dlight_init(void);
void csro_dlight_on_connect(esp_mqtt_client_handle_t client);
void csro_dlight_on_message(esp_mqtt_event_handle_t event);

//csro_airmon.c
void csro_airmon_init(void);
void csro_airmon_on_connect(esp_mqtt_client_handle_t client);
void csro_airmon_on_message(esp_mqtt_event_handle_t event);

#endif