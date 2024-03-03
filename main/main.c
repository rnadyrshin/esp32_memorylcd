#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "memorylcd/MemoryLCD.h"
#include "sdkconfig.h"

static const char *TAG = "example";

#define BLINK_GPIO CONFIG_BLINK_GPIO

static uint8_t s_led_state = 0;
static led_strip_handle_t led_strip;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led_strip, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
    s_led_state = !s_led_state;
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

void app_main()
{
    //configure_led();

    ESP_LOGI(TAG, "Init display");
    memorylcd_init();
    memorylcd_PowerOn();


    for(uint16_t x = 0; x < 200; x++) {
        //frameBuffer[5][x] = 
        GFXDisplayPutPixel_FB(x, 0, BLACK);
        GFXDisplayPutPixel_FB(x, 10, BLACK);
        GFXDisplayPutPixel_FB(x, 20, BLACK);
        GFXDisplayPutPixel_FB(x, 30, BLACK);
        GFXDisplayPutPixel_FB(x, 40, BLACK);
        GFXDisplayPutPixel_FB(x, 50, BLACK);
        //GFXDisplayPutPixel_FB(x, 10, WHITE);
    }

    //uint16_t cntr = 0;

    while (1) {
        //ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        //blink_led();

        ESP_LOGI(TAG, "Update");
        
        memorylcd_update(0, 0);
        //GFXDisplayTestPattern(0x55, 0);

        //GFXDisplayPutPixel(cntr, cntr, BLACK);
        //if(++cntr >= 240)
        //    cntr = 0;

        vTaskDelay(200 /*CONFIG_BLINK_PERIOD*/ / portTICK_PERIOD_MS);
    }
}
