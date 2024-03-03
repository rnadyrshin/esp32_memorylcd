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

void disp1color_DrawPixel(uint8_t X, uint8_t Y, uint8_t State);
void disp1color_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void disp1color_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void disp1color_DrawRectangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void disp1color_DrawCircle(int16_t x0, int16_t y0, int16_t radius);
void disp1color_DrawImageFast(uint8_t *imgBuff);

uint8_t disp1color_DrawChar(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t Char);
int16_t disp1color_DrawString(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t *Str);
int16_t disp1color_printf(uint8_t X, uint8_t Y, uint8_t FontID, const char *args, ...);

int16_t dispcolor_getStrWidth(uint8_t FontID, char *Str);
int16_t dispcolor_getFormatStrWidth(uint8_t FontID, const char *args, ...);
