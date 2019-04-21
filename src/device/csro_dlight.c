#include "csro_device.h"

#ifdef DLIGHT

#define DLIGHT_RED_PIN 15
#define DLIGHT_GREEN_PIN 13
#define DLIGHT_BLUE_PIN 12
#define DLIGHT_WHITE1_PIN 14
#define DLIGHT_WHITE2_PIN 4
#define DLIGHT_PWM_PERIOD 2550

const uint32_t pin_num[7] = {DLIGHT_RED_PIN, DLIGHT_GREEN_PIN, DLIGHT_BLUE_PIN, DLIGHT_WHITE1_PIN, DLIGHT_WHITE2_PIN};
uint32_t duties[7] = {0, 0, 0, 0, 0};
int16_t phase[7] = {0, 0, 0, 0, 0};

void csro_dlight_init(void)
{
    pwm_init(DLIGHT_PWM_PERIOD, duties, 7, pin_num);
    pwm_set_channel_invert(0x00);
    pwm_set_phases(phase);
    pwm_start();
}

void csro_dlight_on_connect(esp_mqtt_client_handle_t client)
{
}

void csro_dlight_on_message(esp_mqtt_event_handle_t event)
{
}

#endif