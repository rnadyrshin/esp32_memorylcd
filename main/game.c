#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display/disp1color.h"

static int16_t scrWidth, scrHeight;
static int16_t ballPosX, ballPosY;
static int16_t ballRadius;
static int16_t ballVectorX, ballVectorY;

void game_update() {
    disp1color_FillScreenbuff(0xFF);

    ballPosX += ballVectorX;
    ballPosY += ballVectorY;

    if (ballPosX + ballRadius >= scrWidth) {
        ballVectorX = -ballVectorX;
        ballPosX += ballVectorX;
    }

    if (ballPosY + ballRadius >= scrHeight) {
        ballVectorY = -ballVectorY;
        ballPosY += ballVectorY;
    }

    if (ballPosX - ballRadius < 0) {
        ballVectorX = -ballVectorX;
        ballPosX += ballVectorX;
    }

    if (ballPosY - ballRadius < 0) {
        ballVectorY = -ballVectorY;
        ballPosY += ballVectorY;
    }

    disp1color_FillCircle(ballPosX, ballPosY, ballRadius);
    disp1color_UpdateFromBuff();
}

void game(uint16_t screenWidth, uint16_t screenHeight) {
    scrWidth = screenWidth;
    scrHeight = screenHeight;
    ballPosX = scrWidth / 2;
    ballPosY = scrHeight / 2;
    ballRadius = 15;
    ballVectorX = 3;
    ballVectorY = 3;

    while (1) {
        game_update();
        vTaskDelay(1);
    }
}
