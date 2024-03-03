#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "display/disp1color.h"
#include "memorylcd/MemoryLCD.h"
#include "images/images.h"
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

void DrawImage(uint8_t *imgBuff) {
    disp1color_DrawImageFast(imgBuff);
    disp1color_UpdateFromBuff();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void app_main()
{
    //configure_led();

    ESP_LOGI(TAG, "Init display");
    disp1color_Init();

    disp1color_DrawLine(0, 200, 400, 200);
    disp1color_DrawLine(0, 210, 400, 210);
    disp1color_DrawLine(0, 220, 400, 220);
    disp1color_DrawLine(0, 230, 400, 230);
    disp1color_DrawRectangle(10, 50, 200, 100); 
    disp1color_DrawCircle(200, 120, 7);

    disp1color_DrawString(10, 10, FONTID_10X16F, (uint8_t*) "TEST");
    disp1color_printf(10, 30, 0, "test %d", 555);

    while (1) {
        //ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        //blink_led();
        //vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        
        DrawImage(img001);
        DrawImage(img002);
        DrawImage(img003);
        DrawImage(img004);
        DrawImage(img005);
        DrawImage(img006);
        DrawImage(img007);
        DrawImage(img008);
        DrawImage(img009);
        DrawImage(img010);
        DrawImage(img011);
        DrawImage(img012);
        DrawImage(img013);
        DrawImage(img014);
        DrawImage(img015);
        DrawImage(img016);
        DrawImage(img017);
        DrawImage(img018);
        DrawImage(img019);
        DrawImage(img020);
        DrawImage(img021);
        DrawImage(img022);
    }
}
