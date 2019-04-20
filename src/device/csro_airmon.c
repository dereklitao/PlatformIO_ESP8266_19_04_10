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
#define BRIGHTNESS 5
#define PWM_PERIOD 2550

typedef enum
{
    PURPLE = 0,  //red=128, green=0; 	blue=128;
    RED,         //red=255, green=0; 	blue=0;
    ORANGE,      //red=255, green=102; 	blue=0;
    YELLOW,      //red=255, green=255; 	blue=0;
    GREENYELLOW, //red=173, green=255; 	blue=47;
    GREEN,       //red=0,   green=255; 	blue=0;
    WHITE        //red=128, green=128; 	blue=128;
} rgb_color;

typedef struct
{
    rgb_color color;
    uint8_t rgb[3];
} rgb_led;

typedef struct
{
    uint32_t pm1_cf, pm1_atm;
    uint32_t pm2_cf, pm2_atm;
    uint32_t pm10_cf, pm10_atm;
    uint32_t cnt_03, cnt_05, cnt_1, cnt_2, cnt_5, cnt_10;
    float hcho, temp, humi;
} pms_data;

static QueueHandle_t uart0_queue;
rgb_led wifi_led, aqi_led;
pms_data pms;
const uint32_t pin_num[7] = {WIFI_LED_R, WIFI_LED_G, WIFI_LED_B, AQI_LED_R, AQI_LED_G, AQI_LED_B, FAN_PIN};
uint32_t duties[7] = {0, 0, 0, 0, 0, 0, (int)(0.6 * PWM_PERIOD)};
int16_t phase[7] = {0, 0, 0, 0, 0, 0, 0};

static void load_color(rgb_led *led)
{
    if (led->color == PURPLE)
    {
        led->rgb[0] = 128;
        led->rgb[1] = 0;
        led->rgb[2] = 128;
    }
    else if (led->color == RED)
    {
        led->rgb[0] = 255;
        led->rgb[1] = 0;
        led->rgb[2] = 0;
    }
    else if (led->color == ORANGE)
    {
        led->rgb[0] = 255;
        led->rgb[1] = 102;
        led->rgb[2] = 0;
    }
    else if (led->color == YELLOW)
    {
        led->rgb[0] = 255;
        led->rgb[1] = 255;
        led->rgb[2] = 0;
    }
    else if (led->color == GREENYELLOW)
    {
        led->rgb[0] = 173;
        led->rgb[1] = 255;
        led->rgb[2] = 47;
    }
    else if (led->color == GREEN)
    {
        led->rgb[0] = 0;
        led->rgb[1] = 255;
        led->rgb[2] = 0;
    }
    else
    {
        led->rgb[0] = 128;
        led->rgb[1] = 128;
        led->rgb[2] = 128;
    }
}

static void update_led_status(void)
{
    rgb_color pm_index, hcho_index;
    if (sysinfo.router_flag == 1)
    {
        if (esp_mqtt_client_connected(mqtt_client))
        {
            wifi_led.color = GREEN;
        }
        else
        {
            wifi_led.color = YELLOW;
        }
    }
    else
    {
        wifi_led.color = RED;
    }
    load_color(&wifi_led);

    if (pms.pm2_atm < 35)
    {
        pm_index = GREEN;
    }
    else if (pms.pm2_atm >= 35 && pms.pm2_atm < 75)
    {
        pm_index = GREENYELLOW;
    }
    else if (pms.pm2_atm >= 75 && pms.pm2_atm < 115)
    {
        pm_index = YELLOW;
    }
    else if (pms.pm2_atm >= 115 && pms.pm2_atm < 150)
    {
        pm_index = ORANGE;
    }
    else if (pms.pm2_atm >= 150 && pms.pm2_atm < 250)
    {
        pm_index = RED;
    }
    else
    {
        pm_index = PURPLE;
    }
    aqi_led.color = pm_index;

    if (pms.hcho < 0.05)
    {
        hcho_index = GREEN;
    }
    else if (pms.hcho >= 0.05 && pms.hcho < 0.1)
    {
        hcho_index = GREENYELLOW;
    }
    else if (pms.hcho >= 0.1 && pms.hcho < 0.2)
    {
        hcho_index = YELLOW;
    }
    else if (pms.hcho >= 0.2 && pms.hcho < 0.5)
    {
        hcho_index = ORANGE;
    }
    else if (pms.hcho >= 0.5 && pms.hcho < 0.8)
    {
        hcho_index = RED;
    }
    else
    {
        hcho_index = PURPLE;
    }

    if (hcho_index < pm_index)
    {
        aqi_led.color = hcho_index;
    }
    load_color(&aqi_led);
}

static void led_task(void *arg)
{
    static bool wifi_on = false;
    while (true)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
        update_led_status();
        wifi_on = !wifi_on;
        if (wifi_on)
        {
            pwm_set_duty(0, wifi_led.rgb[0] * BRIGHTNESS);
            pwm_set_duty(1, wifi_led.rgb[1] * BRIGHTNESS);
            pwm_set_duty(2, wifi_led.rgb[2] * BRIGHTNESS);
        }
        else
        {
            pwm_set_duty(0, 0);
            pwm_set_duty(1, 0);
            pwm_set_duty(2, 0);
        }
        pwm_set_duty(3, aqi_led.rgb[0] * BRIGHTNESS);
        pwm_set_duty(4, aqi_led.rgb[1] * BRIGHTNESS);
        pwm_set_duty(5, aqi_led.rgb[2] * BRIGHTNESS);
        pwm_start();
    }
    vTaskDelete(NULL);
}

static void calculate_pms(uint8_t *data)
{
    uint16_t sum = 0;
    for (size_t i = 0; i < 38; i++)
    {
        sum = sum + data[i];
    }
    if (data[38] == (sum >> 8) && data[39] == (sum & 0xFF))
    {
        pms.pm1_cf = data[4] * 256 + data[5];
        pms.pm2_cf = data[6] * 256 + data[7];
        pms.pm10_cf = data[8] * 256 + data[9];
        pms.pm1_atm = data[10] * 256 + data[11];
        pms.pm2_atm = data[12] * 256 + data[13];
        pms.pm10_atm = data[14] * 256 + data[15];
        pms.cnt_03 = data[16] * 256 + data[17];
        pms.cnt_05 = data[18] * 256 + data[19];
        pms.cnt_1 = data[20] * 256 + data[21];
        pms.cnt_2 = data[22] * 256 + data[23];
        pms.cnt_5 = data[24] * 256 + data[25];
        pms.cnt_10 = data[26] * 256 + data[27];
        pms.hcho = ((float)(data[28] * 256 + data[29])) / 1000.0;
        pms.temp = ((float)(data[30] * 256 + data[31])) / 10.0;
        pms.humi = ((float)(data[32] * 256 + data[33])) / 10.0;

        cJSON *sensor_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(sensor_json, "pm1dot0", pms.pm1_atm);
        cJSON_AddNumberToObject(sensor_json, "pm2dot5", pms.pm2_atm);
        cJSON_AddNumberToObject(sensor_json, "pm10", pms.pm10_atm);
        cJSON_AddNumberToObject(sensor_json, "temperature", pms.temp);
        cJSON_AddNumberToObject(sensor_json, "humidity", pms.humi);
        cJSON_AddNumberToObject(sensor_json, "hcho", pms.hcho);
        cJSON_AddStringToObject(sensor_json, "time", sysinfo.time_str);
        cJSON_AddNumberToObject(sensor_json, "run", (int)(sysinfo.now - sysinfo.start));
        char *out = cJSON_PrintUnformatted(sensor_json);
        strcpy(mqttinfo.content, out);
        free(out);
        cJSON_Delete(sensor_json);
        sprintf(mqttinfo.pub_topic, "csro/%s/%s/state", sysinfo.mac_str, sysinfo.dev_type);
        esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, mqttinfo.content, 0, 0, 1);
    }
}

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t *dtmp = (uint8_t *)malloc(512);
    for (;;)
    {
        if (xQueueReceive(uart0_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            bzero(dtmp, 512);
            if (event.type == UART_DATA)
            {
                uart_read_bytes(UART_NUM_0, dtmp, event.size, portMAX_DELAY);
                if (event.size == 40)
                {
                    calculate_pms(dtmp);
                }
            }
            else if (event.type == UART_FIFO_OVF || event.type == UART_BUFFER_FULL)
            {
                uart_flush_input(UART_NUM_0);
                xQueueReset(uart0_queue);
            }
        }
    }

    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void csro_airmon_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 512 * 2, 512 * 2, 100, &uart0_queue);
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

    pwm_init(PWM_PERIOD, duties, 7, pin_num);
    pwm_set_channel_invert(0x3F);
    pwm_set_phases(phase);
    pwm_start();
    xTaskCreate(led_task, "led_task", 2048, NULL, 9, NULL);
}

#define CFG_AIRMON_TOPIC "csro/sensor/%s_%s_%s/config" //
#define CFG_AIRMON_PAYLOAD "{\"~\":\"csro/%s/%s\",\"dev_cla\":\"%s\",\"unit_of_meas\":\"%s\",\"name\":\"%s_%s_%s\",\"avty_t\":\"~/available\",\"pl_avail\":\"online\",\"pl_not_avail\":\"offline\",\"stat_t\":\"~/state\",\"val_tpl\":\"{{value_json.%s | round(%d)}}\", \"icon\":\"%s\",\"qos\":1}"

char *classlist[6] = {"temperature", "humidity", "pressure", "pressure", "pressure", "pressure"};
char *itemlist[6] = {"temperature", "humidity", "hcho", "pm1dot0", "pm2dot5", "pm10"};
char *unitlist[6] = {"°C", "\%", "mg/m^3", "ug/m^3", "ug/m^3", "ug/m^3"};
char *iconlist[6] = {"mdi:thermometer-lines", "mdi:water-percent", "mdi:alien", "mdi:blur", "mdi:blur", "mdi:blur"};
uint8_t roundlist[6] = {1, 1, 3, 0, 0, 0};

void csro_airmon_on_connect(esp_mqtt_client_handle_t client)
{
    for (size_t i = 0; i < 6; i++)
    {
        sprintf(mqttinfo.pub_topic, CFG_AIRMON_TOPIC, sysinfo.mac_str, sysinfo.dev_type, itemlist[i]);
        sprintf(mqttinfo.content, CFG_AIRMON_PAYLOAD, sysinfo.mac_str, sysinfo.dev_type, classlist[i], unitlist[i], sysinfo.dev_type, itemlist[i], sysinfo.mac_str, itemlist[i], roundlist[i], iconlist[i]);
        esp_mqtt_client_publish(client, mqttinfo.pub_topic, mqttinfo.content, 0, 1, 1);
    }
    sprintf(mqttinfo.pub_topic, "csro/%s/%s/available", sysinfo.mac_str, sysinfo.dev_type);
    esp_mqtt_client_publish(mqtt_client, mqttinfo.pub_topic, "online", 0, 1, 1);
}

void csro_airmon_on_message(esp_mqtt_event_handle_t event)
{
}

#endif