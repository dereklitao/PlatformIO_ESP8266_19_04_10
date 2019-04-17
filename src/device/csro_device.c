#include "csro_device.h"

SemaphoreHandle_t state_msg_semaphore;

void csro_device_on_connect(esp_mqtt_client_handle_t client)
{
#ifdef NLIGHT
    csro_nlight_on_connect(client);
#elif defined DLIGHT
    csro_dlight_prepare_status_message();
#elif defined MOTOR
    csro_motor_prepare_status_message();
#elif defined AQI_MONITOR
    csro_air_monitor_prepare_status_message();
#elif defined AIR_SYSTEM
    csro_air_system_prepare_status_message();
#endif
}

void csro_device_on_message(esp_mqtt_event_handle_t event)
{
#ifdef NLIGHT
    csro_nlight_on_message(event);
#elif defined DLIGHT
    csro_dlight_prepare_status_message();
#elif defined MOTOR
    csro_motor_prepare_status_message();
#elif defined AQI_MONITOR
    csro_air_monitor_prepare_status_message();
#elif defined AIR_SYSTEM
    csro_air_system_prepare_status_message();
#endif
}