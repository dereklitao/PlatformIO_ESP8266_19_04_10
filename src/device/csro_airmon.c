#include "csro_device.h"

#ifdef AIR_MONITOR

#define GPIO_OUTPUT_IO_0 10
#define GPIO_OUTPUT_PIN_SEL 1ULL << GPIO_OUTPUT_IO_0

static void gpio_task_example(void *arg)
{
    static int count = 0;
    while (true)
    {
        gpio_set_level(GPIO_OUTPUT_IO_0, count % 2);
        debug("pin10 = %d\n", count % 2);
        count++;
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void csro_airmon_init(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
}

void csro_airmon_on_connect(esp_mqtt_client_handle_t client)
{
}

void csro_airmon_on_message(esp_mqtt_event_handle_t event)
{
}

#endif