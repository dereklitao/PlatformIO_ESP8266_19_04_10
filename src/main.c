#include <stdio.h>

#include "esp_system.h"
#include "common/csro_common.h"
#include "device/csro_device.h"

void app_main(void)
{
    nvs_handle handle;
    uint8_t flag = 0;
    nvs_flash_init();
    nvs_open("system", NVS_READWRITE, &handle);
    nvs_get_u32(handle, "power_count", &sysinfo.power_on_count);
    nvs_set_u32(handle, "power_count", (sysinfo.power_on_count + 1));
    nvs_get_u8(handle, "router", &flag);
    nvs_commit(handle);
    nvs_close(handle);

    csro_device_init();

    debug("Power Count = %d\n", sysinfo.power_on_count);

    if (flag == 1)
    {
        csro_start_mqtt();
    }
    else
    {
        csro_start_smart_config();
    }
}
