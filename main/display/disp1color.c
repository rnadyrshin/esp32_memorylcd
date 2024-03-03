//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Author: Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "fonts/font.h"
#include "disp1color.h"

#if (DISP1COLOR_type == DISPTYPE_memorylcd)
    #include "../memorylcd/memorylcd.h"
#else
    #error Display type not supported
#endif

static uint8_t buff[(DISP1COLOR_Width * DISP1COLOR_Height) / 8];


static void SwapInt16Values(int16_t *pValue1, int16_t *pValue2)
{
  int16_t TempValue = *pValue1;
  *pValue1 = *pValue2;
  *pValue2 = TempValue;
}

void disp1color_Init()
{
#if (DISP1COLOR_type == DISPTYPE_memorylcd)
    memorylcd_Init();
    memorylcd_PowerOn();
    disp1color_FillScreenbuff(0xFF);
    //memorylcd_Update(buff, sizeof(buff));
#endif
}

void disp1color_TestMode(uint8_t TestOn)
{
#if (DISP1COLOR_type == DISPTYPE_memorylcd)
#endif
}

void disp1color_SetBrightness(uint8_t Value)
{
#if (DISP1COLOR_type == DISPTYPE_memorylcd)
#endif
}

void disp1color_FillScreenbuff(uint8_t FillValue)
{
    memset(buff, FillValue, sizeof(buff));
}

void disp1color_UpdateFromBuff()
{
#if (DISP1COLOR_type == DISPTYPE_ssd1306)
    memorylcd_Update(buff, sizeof(buff));
#endif
}

int16_t disp1color_printf(uint8_t X, uint8_t Y, uint8_t FontID, const char *args, ...)
{
  char StrBuff[100];
  
  va_list ap;
  va_start(ap, args);
  char len = vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
  va_end(ap);
  
  return disp1color_DrawString(X, Y, FontID, (uint8_t *)StrBuff);
}

void disp1color_DrawPixel(uint8_t X, uint8_t Y, uint8_t State)
{
  if ((X >= DISP1COLOR_Width) || (Y >= DISP1COLOR_Height))
    return;
#if (DISP1COLOR_type == DISPTYPE_ssd1306)
    memorylcd_DrawPixel(buff, DISP1COLOR_Width, X, Y, State);
#else  
  uint16_t ByteIdx = Y >> 3;
  uint8_t BitIdx = Y - (ByteIdx << 3);
  ByteIdx *= DISP1COLOR_Width;  
  ByteIdx += X;
  
  if (State)
    buff[ByteIdx] |= (1 << BitIdx);
  else
    buff[ByteIdx] &= ~(1 << BitIdx);
#endif
}

void disp1color_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2) 
{
  const int16_t deltaX = abs(x2 - x1);
  const int16_t deltaY = abs(y2 - y1);
  const int16_t signX = x1 < x2 ? 1 : -1;
  const int16_t signY = y1 < y2 ? 1 : -1;

  int16_t error = deltaX - deltaY;

  disp1color_DrawPixel(x2, y2, 1);

  while (x1 != x2 || y1 != y2) 
  {
    disp1color_DrawPixel(x1, y1, 1);
    const int16_t error2 = error * 2;
 
    if (error2 > -deltaY) 
    {
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX)
    {
      error += deltaX;
      y1 += signY;
    }
  }
}

void disp1color_FillRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
  int16_t y_start = y;
  
  while (y <= y_start + h)
  {
    disp1color_DrawLine(x, y, x + w, y);
    y++;
  }
}

void disp1color_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2) 
{
  disp1color_DrawLine(x1, y1, x1, y2);
  disp1color_DrawLine(x2, y1, x2, y2);
  disp1color_DrawLine(x1, y1, x2, y1);
  disp1color_DrawLine(x1, y2, x2, y2);
}

void disp1color_DrawRectangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  if (x1 > x2)
    SwapInt16Values(&x1, &x2);
  if (y1 > y2)
    SwapInt16Values(&y1, &y2);

  disp1color_FillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

void disp1color_DrawCircle(int16_t x0, int16_t y0, int16_t radius) 
{
  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0)
  {
    disp1color_DrawPixel(x0 + x, y0 + y, 1);
    disp1color_DrawPixel(x0 + x, y0 - y, 1);
    disp1color_DrawPixel(x0 - x, y0 + y, 1);
    disp1color_DrawPixel(x0 - x, y0 - y, 1);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) 
    {
      ++x;
      delta += 2 * x + 1;
      continue;
    }
	
    error = 2 * (delta - x) - 1;
		
    if (delta > 0 && error > 0) 
    {
      --y;
      delta += 1 - 2 * y;
      continue;
    }
	
    ++x;
    delta += 2 * (x - y);
    --y;
  }
}

uint8_t disp1color_DrawChar(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t Char)
{
  uint8_t *pCharTable = font_GetFontStruct(FontID, Char);
  uint8_t CharWidth = font_GetCharWidth(pCharTable);    // ������ �������
  uint8_t CharHeight = font_GetCharHeight(pCharTable);  // ������ �������
  pCharTable += 2;

  if (FontID == FONTID_6X8M)
  {
    for (uint8_t row = 0; row < CharHeight; row++)
    {
      for (uint8_t col = 0; col < CharWidth; col++) {
        disp1color_DrawPixel(X + col, Y + row, pCharTable[row] & (1 << (7 - col)));
      }
    }
  }
  else
  {
    for (uint8_t row = 0; row < CharHeight; row++)
    {
      for (uint8_t col = 0; col < CharWidth; col++)
      {
        if (col < 8)
          disp1color_DrawPixel(X + col, Y + row, pCharTable[row * 2] & (1 << (7 - col)));
        else
          disp1color_DrawPixel(X + col, Y + row, pCharTable[(row * 2) + 1] & (1 << (15 - col)));
      }
    }
  }
  
  return CharWidth;
}

int16_t disp1color_DrawString(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t *Str)
{
  uint8_t done = 0;             // ���� ��������� ������
  uint8_t Xstart = X;           // ���������� ���� ����� ���������� ������� ��� �������� �� ����� ������
  uint8_t StrHeight = 8;        // ������ �������� � �������� ��� �������� �� ��������� ������

  // ����� ������
  while (!done)
  {
    switch (*Str)
    {
    case '\0':  // ����� ������
      done = 1;
      break;
    case '\n':  // ������� �� ��������� ������
      Y += StrHeight;
      break;
    case '\r':  // ������� � ������ ������
      X = Xstart;
      break;
    default:    // ������������ ������
      X += disp1color_DrawChar(X, Y, FontID, *Str);
      StrHeight = font_GetCharHeight(font_GetFontStruct(FontID, *Str));
      break;
    }
    Str++;
  }
  
  return X;
}

int16_t dispcolor_getStrWidth(uint8_t FontID, char *Str)
{
  uint8_t done = 0;       // ���� ��������� ������
  int16_t StrWidth = 0;  	// ������ ������ � ��������

  // ����� ������
  while (!done)
  {
    switch (*Str)
    {
    case '\0':  // ����� ������
      done = 1;
      break;
    case '\n':  // ������� �� ��������� ������
    case '\r':  // ������� � ������ ������
      break;
    default:    // ������������ ������
      StrWidth += font_GetCharWidth(font_GetFontStruct(FontID, *Str));
      break;
    }
    Str++;
  }

  return StrWidth;
}

int16_t dispcolor_getFormatStrWidth(uint8_t FontID, const char *args, ...)
{
  char StrBuff[256];

  va_list ap;
  va_start(ap, args);
  vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
  va_end(ap);

  return dispcolor_getStrWidth(FontID, StrBuff);
}
