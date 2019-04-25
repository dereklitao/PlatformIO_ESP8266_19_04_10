#include "csro_device.h"

#ifdef RGBLIGHT

#define RGBLIGHT_RED_PIN 15
#define RGBLIGHT_GREEN_PIN 13
#define RGBLIGHT_BLUE_PIN 12
#define RGBLIGHT_PWM_PERIOD 2550

#define CFG_RGBLIGHT_TOPIC "csro/light/%s_%s_%d/config"
#define CFG_RGBLIGHT_PAYLOAD "{\"~\":\"csro/%s/%s\",\"name\":\"%s_%d_%s\",\"avty_t\":\"~/available\",\"pl_avail\":\"online\",\"pl_not_avail\":\"offline\",\"stat_t\":\"~/state\",\"stat_val_tpl\":\"{{value_json.state%d.on}}\",\"bri_stat_t\":\"~/state\",\"bri_val_tpl\":\"{{value_json.state%d.bright}}\",\"cmd_t\":\"~/set/%d\",\"pl_on\":1,\"pl_off\":0, \"bri_cmd_t\":\"~/set/bright%d\",\"opt\":\"false\",\"qos\":1}"

typedef struct
{
    uint8_t state;
    uint8_t bright;
    uint8_t bright_set;
    uint8_t bright_last;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} csro_rgb_light;

csro_rgb_light rgb_light;
const uint32_t rgb_pin[3] = {RGBLIGHT_RED_PIN, RGBLIGHT_GREEN_PIN, RGBLIGHT_BLUE_PIN};
uint32_t rgb_duties[3] = {0, 0, 0};
int16_t rgb_phases[3] = {0, 0, 0};

void csro_rgblight_init(void)
{
    pwm_init(RGBLIGHT_PWM_PERIOD, rgb_duties, 3, rgb_pin);
    pwm_set_channel_invert(0x00);
    pwm_set_phases(rgb_phases);
    pwm_start();
}

void csro_rgblight_on_connect(esp_mqtt_client_handle_t client)
{
}

void csro_rgblight_on_message(esp_mqtt_event_handle_t event)
{
}

#endif