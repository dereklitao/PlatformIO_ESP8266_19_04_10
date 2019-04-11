#include <stdio.h>

#include "esp_system.h"
#include "common/common.h"

void app_main(void)
{
    printf("\r\nHello PlatformIO\n");

    nvs_flash_init();

    printf("\r\nHello github\r\n");

    xTaskCreate(test_task, "test_task", 4096, NULL, 10, NULL);
    xTaskCreate(csro_smart_config_task, "csro_smart_config_task", 4096, NULL, 10, NULL);
}
