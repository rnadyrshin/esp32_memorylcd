#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "display/disp1color.h"
#include "images/images.h"
#include "game.h"
#include "ledstrip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

void DrawImage(uint8_t *imgBuff) {
    disp1color_DrawImageFast(imgBuff);
    disp1color_UpdateFromBuff();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}

void app_main()
{
    //configure_led();

    ESP_LOGI(TAG, "Init display");
    disp1color_Init();
    //game(DISP1COLOR_Width, DISP1COLOR_Height);

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
        /*
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
        DrawImage(img023);
        DrawImage(img024);
        DrawImage(img025);
        DrawImage(img026);
        DrawImage(img027);
        DrawImage(img028);
        DrawImage(img029);
        DrawImage(img030);
        DrawImage(img031);
        DrawImage(img032);
        DrawImage(img033);
        DrawImage(img034);
        DrawImage(img035);
        DrawImage(img036);
        DrawImage(img037);
        DrawImage(img038);
        DrawImage(img039);
        DrawImage(img040);
        DrawImage(img041);
        DrawImage(img042);
        DrawImage(img043);
        DrawImage(img044);
        DrawImage(img045);
        DrawImage(img046);
        DrawImage(img047);
        DrawImage(img048);
        DrawImage(img049);
        DrawImage(img050);
        DrawImage(img051);
        DrawImage(img052);
        DrawImage(img053);
        DrawImage(img054);
        DrawImage(img055);
        DrawImage(img056);
        DrawImage(img057);
        DrawImage(img058);
        DrawImage(img059);
        DrawImage(img060);
        DrawImage(img061);
        */
        //DrawImage(img062);

        DrawImage(interface);
        DrawImage(selfie1_diff75);
        DrawImage(selfie1_diff80);
        DrawImage(selfie1_my);
        DrawImage(selfie2_diff75);
        DrawImage(selfie2_diff80);
        DrawImage(selfie2_my);
        DrawImage(selfie3_big_diff50);
        DrawImage(selfie3_big_diff75);
        DrawImage(selfie3_diff60);
        DrawImage(selfie3_diff75);
        DrawImage(selfie3_my);
        DrawImage(selfie4_diff50);
        DrawImage(selfie4_diff75);
        DrawImage(selfie4_my);
        DrawImage(selfie5_diff50);
        DrawImage(selfie5_diff75);
        DrawImage(selfie6_big_diff60);
        DrawImage(selfie6_big_diff75);
        DrawImage(selfie6_diff50);
        DrawImage(selfie6_diff75);
        DrawImage(selfie7_big_diff75);
        DrawImage(selfie7_big_diff80);
        DrawImage(selfie7_diff60);
        DrawImage(selfie7_diff75);
    }
}
