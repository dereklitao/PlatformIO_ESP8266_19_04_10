#include "common.h"

#define MACSTR_FORMAT "%02x%02x%02x%02x%02x%02x"
#define PASSSTR_FORMAT "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"

csro_system sysinfo;
csro_mqtt mqttinfo;

static void system_print_device_type(void)
{
#ifdef NLIGHT
    sprintf(sysinfo.dev_type, "nlight%d", NLIGHT);
#elif defined DLIGHT
    sprintf(sysinfo.dev_type, "dlight%d", DLIGHT);
#elif defined MOTOR
    sprintf(sysinfo.dev_type, "motor%d", MOTOR);
#elif defined AQI_MONITOR
    sprintf(sysinfo.dev_type, "airmon");
#elif defined AIR_SYSTEM
    sprintf(sysinfo.dev_type, "airsys");
#endif
}

void csro_system_get_info(void)
{
    size_t len = 0;
    nvs_handle handle;
    nvs_open("system", NVS_READWRITE, &handle);
    nvs_get_str(handle, "ssid", NULL, &len);
    nvs_get_str(handle, "ssid", sysinfo.router_ssid, &len);
    nvs_get_str(handle, "pass", NULL, &len);
    nvs_get_str(handle, "pass", sysinfo.router_pass, &len);
    nvs_get_u16(handle, "interval", &mqttinfo.interval);
    if (mqttinfo.interval < 2 || mqttinfo.interval > 120)
    {
        mqttinfo.interval = 5;
        nvs_set_u16(handle, "interval", mqttinfo.interval);
    }
    nvs_commit(handle);
    nvs_close(handle);
    esp_wifi_get_mac(WIFI_MODE_STA, sysinfo.mac);
    sprintf(sysinfo.mac_str, MACSTR_FORMAT, sysinfo.mac[0], sysinfo.mac[1], sysinfo.mac[2], sysinfo.mac[3], sysinfo.mac[4], sysinfo.mac[5]);
    sprintf(sysinfo.host_name, "CSRO_%s", sysinfo.mac_str);

    sprintf(mqttinfo.id, "csro/%s", sysinfo.mac_str);
    system_print_device_type();
    sprintf(mqttinfo.name, "csro/%s/%s", sysinfo.mac_str, sysinfo.dev_type);
    sprintf(mqttinfo.pass, PASSSTR_FORMAT, sysinfo.mac[1], sysinfo.mac[3], sysinfo.mac[5], sysinfo.mac[0], sysinfo.mac[2], sysinfo.mac[4], sysinfo.mac[5], sysinfo.mac[3], sysinfo.mac[1], sysinfo.mac[4], sysinfo.mac[2], sysinfo.mac[0]);
    debug("id = %s.\nname = %s.\npass = %s.\n", mqttinfo.id, mqttinfo.name, mqttinfo.pass);
}