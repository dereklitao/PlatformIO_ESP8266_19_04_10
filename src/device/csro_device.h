#ifndef CSRO_DEVICE_H_
#define CSRO_DEVICE_H_

#include "./common/csro_common.h"
#include "cJSON.h"

extern SemaphoreHandle_t state_msg_semaphore;

//csro_device.c
void csro_device_on_connect(esp_mqtt_client_handle_t client);
void csro_device_on_message(esp_mqtt_event_handle_t event);

//csro_nlight.c
void csro_nlight_on_connect(esp_mqtt_client_handle_t client);
void csro_nlight_on_message(esp_mqtt_event_handle_t event);

#endif