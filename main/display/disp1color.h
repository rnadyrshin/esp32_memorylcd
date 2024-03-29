//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Author: Nadyrshin Ruslan
//------------------------------------------------------------------------------
#pragma once

#include "fonts/font.h"

#define DISPTYPE_memorylcd      0

#define DISP1COLOR_type         DISPTYPE_memorylcd

#define DISP1COLOR_Width        400
#define DISP1COLOR_Height       240

void disp1color_Init();
void disp1color_TestMode(uint8_t TestOn);
void disp1color_SetBrightness(uint8_t Value);
void disp1color_FillScreenbuff(uint8_t FillValue);
void disp1color_UpdateFromBuff();

void disp1color_DrawPixel(int16_t X, int16_t Y, uint8_t State);
void disp1color_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void disp1color_DrawLine2(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t thick);
void disp1color_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void disp1color_FillRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void disp1color_DrawCircle(int16_t x0, int16_t y0, int16_t radius);
void disp1color_FillCircle(int16_t x0, int16_t y0, int16_t radius);
void disp1color_DrawArc(int16_t x0, int16_t y0, int16_t radius, int16_t startAngle, int16_t endAngle);
void disp1color_DrawArc_Wu(int16_t x0, int16_t y0, int16_t radius, int16_t startAngle, int16_t endAngle);
void disp1color_DrawImage(uint8_t *imgBuff);

uint8_t disp1color_DrawChar(int16_t X, int16_t Y, uint8_t FontID, char Char);
int16_t disp1color_DrawString(int16_t X, int16_t Y, uint8_t FontID, char *Str);
int16_t disp1color_printf(int16_t X, int16_t Y, uint8_t FontID, const char *args, ...);

int16_t dispcolor_getStrWidth(uint8_t FontID, char *Str);
int16_t dispcolor_getFormatStrWidth(uint8_t FontID, const char *args, ...);
