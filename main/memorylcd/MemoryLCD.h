/**
* @brief 		Header file for Sharp Memory LCD
* @processor	ESP32 / Arduino M0 PRO
*/

/**
 * Revision	: 	Add suport for two new modules 
 * 				- LS006B7DH03 (64x64) & LS011B7DH03 (160x68)
 * Date :		29-11-2018
 * */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define   LS027B7DH01
//#define	LS032B7DD02
//#define 	LS044Q7DH01
//#define 	LS006B7DH03
//#define 	LS011B7DH03

//@note pin number definition, hardware dependent
#define SPI_HOST            SPI2_HOST

#define MEMORYLCD_SCK       10
#define MEMORYLCD_MOSI      11
#define MEMORYLCD_SCS       12
#define MEMORYLCD_EXTCOMIN  13
#define MEMORYLCD_DISP      14

//@note EXTCOMIN pulse frequency in hal_extcom_start(hz) fcn. -> GFXDisplayOn()
#define EXTCOMIN_FREQ       2


#ifndef MIN
#define MIN(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#define MAX(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

#ifdef LS027B7DH01
	#define DISP_HOR_RESOLUTION	400
	#define DISP_VER_RESOLUTION	240
#elif defined LS032B7DD02
	#define DISP_HOR_RESOLUTION	336
	#define DISP_VER_RESOLUTION	536
#elif defined LS044Q7DH01
	#define DISP_HOR_RESOLUTION	320
	#define DISP_VER_RESOLUTION	240
#elif defined LS006B7DH03
	#define DISP_HOR_RESOLUTION	64
	#define DISP_VER_RESOLUTION	64
#elif defined LS011B7DH03
	#define DISP_HOR_RESOLUTION	160
	#define DISP_VER_RESOLUTION	68
#else
	#error You need to define the horizontal and vertical resolution for a new model
#endif

void memorylcd_Init();
void memorylcd_Update(uint8_t *buff, uint16_t len);
void memorylcd_Clear();
void memorylcd_PowerOn();
void memorylcd_PowerOff();
void memorylcd_DrawPixel(uint8_t *buff, uint16_t dispWidth, uint16_t x, uint16_t y, uint8_t color);
