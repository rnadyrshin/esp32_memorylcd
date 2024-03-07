#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../display/disp1color.h"
#include "demo.h"

#define xC	120
#define yC	120

static uint8_t hour;
static uint8_t min;
static uint8_t sec;

static void DrawArrow(int16_t angle, uint8_t lineLen, uint8_t thick) {
	angle -= 90;
	float angleRad = (float) angle * M_PI / 180;
	int x = cos(angleRad) * lineLen + xC;
	int y = sin(angleRad) * lineLen + yC;
	disp1color_DrawLine2(xC, yC, x, y, thick);
}

static void DrawClock(uint8_t hour, uint8_t min, uint8_t sec, uint8_t secBubbles) {
	uint8_t radius1 = 115;
	for (uint16_t angle = 0; angle <= 360; angle += 6) {
		uint8_t riskSize;
		if (!(angle % 90))
			riskSize = 13;
		else if (!(angle % 30))
			riskSize = 10;
		else
			riskSize = 6;

		uint8_t radius2 = radius1 - riskSize;
		float angleRad = (float) angle * M_PI / 180;
		int x1 = cos(angleRad) * radius1 + xC;
		int y1 = sin(angleRad) * radius1 + yC;
		int x2 = cos(angleRad) * radius2 + xC;
		int y2 = sin(angleRad) * radius2 + yC;

        disp1color_DrawLine2(x1, y1, x2, y2, 2);
	}

	disp1color_DrawString(161, 30,  FONTID_24F, "1");
	disp1color_DrawString(195, 62,  FONTID_24F, "2");
	disp1color_DrawString(202, 106, FONTID_32F, "3");
	disp1color_DrawString(194, 157, FONTID_24F, "4");
	disp1color_DrawString(165, 190, FONTID_24F, "5");
	disp1color_DrawString(112, 193, FONTID_32F, "6");
	disp1color_DrawString(66,  190, FONTID_24F, "7");
	disp1color_DrawString(35,  157, FONTID_24F, "8");
	disp1color_DrawString(22,  106, FONTID_32F, "9");
	disp1color_DrawString(33,  62,  FONTID_24F, "10");
	disp1color_DrawString(65,  30,  FONTID_24F, "11");
	disp1color_DrawString(106, 16,  FONTID_32F, "12");

	DrawArrow(min * 6 + sec / 10, 95, 3);
	DrawArrow(hour * 30 + min / 2, 50, 6);

	uint8_t radius3 = 112;
	if (!sec)
		sec = 60;
	if (secBubbles) {
		int16_t startAngle = -90;
		int16_t endAngle = sec * 6 - 90;

		for (int16_t angle = startAngle; angle <= endAngle; angle += 6) {
			float angleRad = (float) angle * M_PI / 180;
			int x = cos(angleRad) * radius3 + xC;
			int y = sin(angleRad) * radius3 + yC;

			if (angle == endAngle)
				disp1color_FillCircle(x, y, 5);
			else
				disp1color_FillCircle(x, y, 3);
		}
	} else
		disp1color_DrawArc(xC, yC, 119, 0, sec * 6);
}

static void DrawProgressbar(int16_t x, int16_t y, int16_t width, char *title, int16_t value, int16_t max) {
    // Draw title
    int16_t textWidth = dispcolor_getStrWidth(FONTID_10X16F, title);
    int16_t X = x + (width - textWidth) / 2;
    disp1color_DrawString(X, y + 2, FONTID_10X16F, title);

    //Draw progressbar
    int16_t yBase = y + 20;
    int16_t barHeight = 18;
    int16_t barValueWidth = value * width / max;
    disp1color_DrawRectangle(x, yBase, x + width, yBase + barHeight);
    disp1color_FillRectangle(x, yBase + 1, x + barValueWidth, yBase + barHeight - 2);
}

void clock_task(void *params)
{
    while (1) {
        if (++sec == 60) {
            sec = 0;
            if (++min == 60) {
                min = 0;
                if (++hour == 24) {
                    hour = 0;
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void demo_clock() {
    hour = 12;
    min = 17;
    sec = 18;
    uint8_t secBubbles = 1;
    xTaskCreatePinnedToCore(clock_task, "clock", 4 * 1024, NULL, 1, NULL, 1);

    while (1) {
        disp1color_FillScreenbuff(0xFF);

        DrawClock(hour, min, sec, secBubbles);

        disp1color_DrawRectangle(250, 10, 390, 65); 
        disp1color_printf(255, 14, FONTID_32F, "%02d : %02d : %02d", hour, min, sec);
        disp1color_DrawString(262, 42,  FONTID_10X16F, "10 ìàðòà 2024");

        DrawProgressbar(250, 80, 140, "×ÀÑÛ", hour, 24);
        DrawProgressbar(250, 125, 140, "ÌÈÍÓÒÛ", min, 60);
        DrawProgressbar(250, 170, 140, "ÑÅÊÓÍÄÛ", sec, 60);

        disp1color_DrawString(197, 230, FONTID_6X8M, "Ýëåêòðîíèêà â îáúåêòèâå 2024");

    	disp1color_UpdateFromBuff();

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
