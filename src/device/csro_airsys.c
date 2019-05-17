#include "csro_device.h"
#include "./modbus/modbus_master.h"

struct air_sys_register
{
    uint8_t coil[100];
    bool coil_flag[100];
    uint16_t holding[100];
    bool holding_flag[100];
} airsys_reg;

static SemaphoreHandle_t master_mutex;
static SemaphoreHandle_t write_semaphore;

static void write_coil_with_flag_and_range_check(uint8_t index)
{
    if (airsys_reg.coil_flag[index] == false)
    {
        return;
    }
    if (airsys_reg.coil[index] == 1 || airsys_reg.coil[index] == 0)
    {
        modbus_master_write_single_coil(&Master, 1, index, airsys_reg.coil[index]);
    }
    airsys_reg.coil_flag[index] = false;
}

static void write_holding_with_flag_and_range_check(uint8_t index, uint16_t min, uint16_t max)
{
    if (airsys_reg.holding_flag[index] == false)
    {
        return;
    }
    if (airsys_reg.holding[index] >= min && airsys_reg.holding[index] <= max)
    {
        modbus_master_Write_single_holding_reg(&Master, 1, index, airsys_reg.holding[index]);
    }
    airsys_reg.holding_flag[index] = false;
}

static void modbus_master_write_task(void *pvParameters)
{
    while (true)
    {
        if (xSemaphoreTake(write_semaphore, portMAX_DELAY) == pdTRUE)
        {
            if (xSemaphoreTake(master_mutex, portMAX_DELAY) == pdTRUE)
            {
                write_coil_with_flag_and_range_check(40);
                write_coil_with_flag_and_range_check(41);
                write_holding_with_flag_and_range_check(21, 1, 2);
                xSemaphoreGive(master_mutex);
            }
        }
    }
    vTaskDelete(NULL);
}

static void modbus_master_read_task(void *pvParameters)
{
    while (true)
    {
        if (xSemaphoreTake(master_mutex, portMAX_DELAY) == pdTRUE)
        {
            uint8_t temp_coil[100];
            if (modbus_master_read_coils(&Master, 1, 1, 79, &temp_coil[1]))
            {
                for (size_t i = 1; i < 80; i++)
                {
                    if (airsys_reg.coil_flag[i] == false)
                    {
                        airsys_reg.coil[i] = temp_coil[i];
                    }
                    debug("%d|", airsys_reg.coil[i]);
                }
                debug("\r\n");
            }
            debug("read coil timeout!\n");
            xSemaphoreGive(master_mutex);
        }

        if (xSemaphoreTake(master_mutex, portMAX_DELAY) == pdTRUE)
        {
            uint16_t temp_holding[100];
            if (modbus_master_read_holding_regs(&Master, 1, 1, 42, &temp_holding[1]))
            {
                for (size_t i = 1; i < 43; i++)
                {
                    if (airsys_reg.holding_flag[i] == false)
                    {
                        airsys_reg.holding[i] = temp_holding[i];
                    }
                    debug("%d|", airsys_reg.holding[i]);
                }
                debug("\r\n");
            }
            debug("read holding timeout!\n");
            xSemaphoreGive(master_mutex);
        }
        vTaskDelay(200 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

static void gpio_read(void *pvParameters)
{
    while (true)
    {
        printf("gpio 16 level = %d", gpio_get_level(GPIO_NUM_16));
        vTaskDelay(200 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

static void gpio_init(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << 16;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
}

void csro_airsys_init(void)
{
    master_mutex = xSemaphoreCreateMutex();
    write_semaphore = xSemaphoreCreateBinary();
    gpio_init();
    modbus_master_init();
    xTaskCreate(modbus_master_read_task, "modbus_master_read_task", 2048, NULL, 5, NULL);
    xTaskCreate(modbus_master_write_task, "modbus_master_write_task", 2048, NULL, 8, NULL);
    xTaskCreate(gpio_read, "gpio_read", 2048, NULL, 8, NULL);
}
void csro_airsys_on_connect(esp_mqtt_client_handle_t client)
{
}
void csro_airsys_on_message(esp_mqtt_event_handle_t event)
{
}