#include "csro_device.h"

#ifdef AIR_MONITOR

#define WIFI_LED_R 5
#define WIFI_LED_G 4
#define WIFI_LED_B 10
#define AQI_LED_R 12
#define AQI_LED_G 14
#define AQI_LED_B 13
#define FAN_PIN 15
#define BTN_PIN 0

#define GPIO_OUTPUT_IO_0 10
#define GPIO_OUTPUT_PIN_SEL 1ULL << GPIO_OUTPUT_IO_0

const uint32_t pin_num[7] = {WIFI_LED_R, WIFI_LED_G, WIFI_LED_B, AQI_LED_R, AQI_LED_G, AQI_LED_B, FAN_PIN};
uint32_t duties[7] = {0, 0, 0, 0, 0, 0, 300};
int16_t phase[7] = {0, 0, 0, 0, 0, 0, 0};

static void gpio_task_example(void *arg)
{
    static int count = 0;
    while (true)
    {
        count = (count + 1) % 500;
        vTaskDelay(10 / portTICK_RATE_MS);
        pwm_set_duty(0, count);
        pwm_set_duty(3, count);
        pwm_start();
    }
}

void csro_airmon_init(void)
{
    pwm_init(500, duties, 7, pin_num);
    pwm_set_phases(phase);
    pwm_start();
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
}

void csro_airmon_on_connect(esp_mqtt_client_handle_t client)
{
}

void csro_airmon_on_message(esp_mqtt_event_handle_t event)
{
}

#endif