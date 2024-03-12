#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display/disp1color.h"
#include "demo/demo.h"
#include "sdkconfig.h"

void app_main()
{
    disp1color_Init();
    
    demo_text();
    //demo_clock();
    //demo_game(DISP1COLOR_Width, DISP1COLOR_Height);
    //demo_images();

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
