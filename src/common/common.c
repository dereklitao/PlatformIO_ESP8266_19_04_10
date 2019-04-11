#include "common.h"

void csro_smart_config_task(void *pvParameters)
{
    static uint16_t count = 0;

    for (;;)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
        printf("This is the %d smartconfig!\r\n", count);
        count++;
    }
    vTaskDelete(NULL);
}

void test_task(void *pvParameters)
{
    static uint16_t count = 0;

    for (;;)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
        printf("This is the %d print!\r\n", count);
        count++;
    }
    vTaskDelete(NULL);
}