#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../display/disp1color.h"
#include "../images/images.h"
#include "sdkconfig.h"
#include "demo.h"

void DrawImage(uint8_t *imgBuff) {
    disp1color_DrawImage(imgBuff);
    disp1color_UpdateFromBuff();
    vTaskDelay(1500 / portTICK_PERIOD_MS);
}

void demo_images() {
    while (1) {
        DrawImage(img001);
        //DrawImage(img002);
        //DrawImage(img003);
        //DrawImage(img004);
        //DrawImage(img005);
        DrawImage(img006);
        //DrawImage(img007);
        //DrawImage(img008);
        DrawImage(img009);
        //DrawImage(img010);
        //DrawImage(img011);
        DrawImage(img012);
        //DrawImage(img013);
        //DrawImage(img014);
        //DrawImage(img015);
        //DrawImage(img016);
        //DrawImage(img017);
        DrawImage(img018);
        DrawImage(img019);
        DrawImage(img020);
        DrawImage(img021);
        DrawImage(img022);
        //DrawImage(img023);
        DrawImage(img024);
        DrawImage(img025);
        //DrawImage(img026);
        //DrawImage(img027);
        //DrawImage(img028);
        //DrawImage(img029);
        //DrawImage(img030);
        DrawImage(img031);
        //DrawImage(img032);
        DrawImage(img033);
        //DrawImage(img034);
        //DrawImage(img035);
        //DrawImage(img036);
        //DrawImage(img037);
        DrawImage(img038);
        //DrawImage(img039);
        //DrawImage(img040);
        //DrawImage(img041);
        DrawImage(img042);
        //DrawImage(img043);
        DrawImage(img044);
        //DrawImage(img045);
        //DrawImage(img046);
        //DrawImage(img047);
        DrawImage(img048);
        //DrawImage(img049);
        //DrawImage(img050);
        //DrawImage(img051);
        //DrawImage(img052);
        //DrawImage(img053);
        //DrawImage(img054);
        //DrawImage(img055);
        //DrawImage(img056);
        //DrawImage(img057);
        DrawImage(img058);
        //DrawImage(img059);
        //DrawImage(img060);
        //DrawImage(img061);
    }
}
