#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
static const char *TAG = "ADC EXAMPLE";
static esp_adc_cal_characteristics_t adc1_chars;

#define TEST_PIN 2

void app_main(void)
{
    int value;

    gpio_pad_select_gpio(TEST_PIN);
    gpio_set_direction (TEST_PIN,GPIO_MODE_OUTPUT);

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11));
    while (1) 
    {
        gpio_set_level(TEST_PIN, 1);
        value = adc1_get_raw(ADC1_CHANNEL_6); //esp_adc_cal_raw_to_voltage(
        //ESP_LOGI(TAG, "ADC1_CHANNEL_6: %d mV", value);
        gpio_set_level(TEST_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}