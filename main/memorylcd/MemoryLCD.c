/**
* @brief  	Arduino driver for Sharp Memory LCD. 5 LCD models are supported with ESP32 or Arduino M0 PRO.
*			Wiring details is described in MemoryLCD.h under @note pin number definition, hardware dependent
*/
//#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/semphr.h"
#include <esp_timer.h>
#include <sys/time.h>
#include "driver/gpio.h"
#include "HalGpio.h"
#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
//#include "SpiMasterDevice.h"
#include "esp_log.h"

#include "MemoryLCD.h"

#define NOP() asm volatile ("nop")
#define UPD_CMD     0x01    //0x80
#define VCOM_CMD    0x02    //0x40
#define CLEAR_CMD   0x03    //0x20

static bool ExtPinLast = false;
static spi_host_device_t SpiHost = SPI_HOST;
static spi_device_handle_t SpiDevice = 0;
static TimerHandle_t Timer;
uint8_t frameBuffer[GFX_FB_CANVAS_H][GFX_FB_CANVAS_W];

void hal_extcom_start(uint16_t hz);
void hal_extcom_stop();
void hal_extcom_toggle(TimerHandle_t xTimer);

void memorylcd_DrawPixel(uint8_t *buff, uint16_t dispWidth, uint16_t x, uint16_t y, uint8_t color)
{
	//if(y > (GFX_FB_CANVAS_H - 1) || ((x >> 3) > (GFX_FB_CANVAS_W - 1)))
    //    return;
	
	//uint8_t maskBit = 0x80 >> (x & 0x07);	//SPI data sent with MSB first
	uint8_t maskBit = 0x01 << (x & 0x07);	//SPI data sent with LSB first
	uint16_t byteIdx = y * (dispWidth >> 3) + (x >> 3);

	if(!color)
        buff[byteIdx] |= maskBit;
    else
        buff[byteIdx] &= ~maskBit;
}

void GFXDisplayPutPixel_FB(uint16_t x, uint16_t y, COLOR color)
{
	if(y > (GFX_FB_CANVAS_H - 1) || ((x >> 3) > (GFX_FB_CANVAS_W - 1)))
        return;
		
	uint8_t maskBit;
	
	//maskBit = 0x80 >> (x & 0x07);	//SPI data sent with MSB first
	maskBit = 0x01 << (x & 0x07);	//SPI data sent with LSB first
	
	if(color == WHITE)
        frameBuffer[y][(x >> 3)] |= maskBit;    //frameBuffer[y][(x>>1)] &= (maskBit^0xFF); frameBuffer[y][(x>>1)] |= color;
    else
        frameBuffer[y][(x >> 3)] &= (maskBit ^ 0xFF);
}

static void GFXDisplayUpdateLine(uint16_t line, uint8_t *buf);
static void GFXDisplayUpdateBlock(uint16_t start_line, uint16_t end_line, uint8_t *buf);
static uint16_t bfc_DrawChar_RowRowUnpacked(uint16_t x0, uint16_t y0, const BFC_FONT *pFont, uint16_t ch, COLOR color, COLOR bg);

void memorylcd_Clear()
{
    digitalWrite(GFX_DISPLAY_SCS, HIGH);  
    hal_delayUs(3); //SCS setup time of tsSCS (refer to datasheet for timing details)

    uint8_t buff[2] = {CLEAR_CMD, 0x00};

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 2 * 8;
    t.tx_buffer = buff;
    spi_device_polling_transmit(SpiDevice, &t);

    hal_delayUs(1); //SCS hold time of thSCS (refer to datasheet for timing details)
    digitalWrite(GFX_DISPLAY_SCS, LOW);  

    memset((void *)&frameBuffer, 0xFF, sizeof(frameBuffer));  //clear SRAM of the MCU
}

/**
 * @brief Power-on sequence with 5V0 enabled, clear pixel memory (x2), DISP pin set high, and EXTCOMIN pulse started<br>
 *        Normal operation after this function executed. EXTCOMIN_FREQ indicates the pulse frequency defined in MemoryLCD.h
 */
void memorylcd_PowerOn()
{
    hal_delayUs(30);
    memorylcd_Clear();

    hal_delayUs(30);
    GFXDisplayOn();
    hal_delayUs(30);
    hal_extcom_start(EXTCOMIN_FREQ); //turn on EXTCOMIN pulse
    hal_delayUs(30);
}

/**
 * @brief	Display OFF with DISP pin set '1'
 */
void GFXDisplayOn()
{
  digitalWrite(GFX_DISPLAY_DISP, HIGH); //DISP = '1'
}

/**
 * @brief Power-off sequence with all pixels set white, DISP pin set low, EXTCOMIN pulse stopped, and 5V0 disabled.<br>
 *        Need to run GFXDisplayPowerOn() after this function if we need to write to Memory LCD again.
 */
void GFXDisplayPowerOff()
{
    memorylcd_Clear();
    GFXDisplayOff(); 	//DISP = '0'
    hal_extcom_stop();  	//stop EXTCOMIN pulse
    hal_delayUs(30);
}

/**
 * @brief	Display OFF with DISP pin set '0'
 */
void GFXDisplayOff()
{
  digitalWrite(GFX_DISPLAY_DISP, LOW); //DISP = '0'
}

/**
 * @brief	Print a single pixel
 * @param	(x,y) indicate the position
 * @param	color is BLACK/WHITE
 */
void GFXDisplayPutPixel(uint16_t x, uint16_t y, COLOR color)
{
  GFXDisplayPutPixel_FB(x, y, color);
  GFXDisplayUpdateLine(y, (uint8_t *)&frameBuffer[y]);	//Update on screen. Line counts from 1 thats why y+1
}

void SendDummyBytes() {
  uint8_t buff[2] = {0, 0};

  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length = 2 * 8;
  t.tx_buffer = buff;
  spi_device_polling_transmit(SpiDevice, &t);
}

/**
 * @brief	Display a test pattern of vertical strip with horizontal byte defined
 * @param	pattern in 8-bit to define the byte pattern
 * @param	*pfcn a pointer to function when SCS in high state, mainly for energy measurement.
 * @return	time in millisec to update the display with a vertical strip pattern
 * @note	No frame buffer is involved. This function displays a vertical strip pattern defined by the argument 'pattern' in LSB format.<br>
 *			Example to use<br>
 *				GFXDisplayTestPattern(0xF0, NULL);
 */
uint32_t GFXDisplayTestPattern(uint8_t pattern, void (*pfcn)(void))
{
    uint32_t timing = 0;
    uint32_t sMillis = xTaskGetTickCount();

    digitalWrite(GFX_DISPLAY_SCS, HIGH);  
    hal_delayUs(3); //SCS setup time of tsSCS (refer to datasheet for timing details)

    uint16_t oneLineLen = (DISP_HOR_RESOLUTION >> 3) + 2;
    uint8_t buff[oneLineLen];

    for(uint16_t line = 0; line < DISP_VER_RESOLUTION; line++)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = oneLineLen * 8;
        t.tx_buffer = buff;

#ifdef LS032B7DD02
        buff[0] = ((line + 1) << 6) | UPD_CMD; //update one specified line with M0=H,M2=L & AG0:AG1 concatenate to Bit[1:0] sending with LSB first
        buff[1] = (line + 1) >> 2;          //AG2~AG9 in LSB first
#else
        buff[0] = UPD_CMD;                     //update one specified line with M0=H,M2=L sending with LSB first
        buff[1] = line + 1;                 //AG0~AG7 in LSB first for gate line address
#endif

    for(uint16_t dot = 0; dot < DISP_HOR_RESOLUTION >> 3; dot++) {
        buff[dot + 2] = pattern;
    }

    spi_device_polling_transmit(SpiDevice, &t);

    if(line == DISP_VER_RESOLUTION / 2)
    {
        if(pfcn) {
            pfcn();//run pfcn() only once sample in the middle, pls make sure sampling time is long enough
        }
    }
  }
  
  SendDummyBytes();
  
  hal_delayUs(1); //SCS hold time of thSCS (refer to datasheet for timing details)
  digitalWrite(GFX_DISPLAY_SCS, LOW);  

  timing = xTaskGetTickCount() - sMillis;  
  return timing;
}

/**
 * @brief Function to update one line
 * @note  The minimum payload to write to a Memory LCD is a horizontal line
 * @param line is the line number start from 0 to DISP_VER_RESOLUTION - 1
 * @param *buf is a pointer to data
 */
static void GFXDisplayUpdateLine(uint16_t line, uint8_t *buf)
{
  if(line >= DISP_VER_RESOLUTION)
    return;

  digitalWrite(GFX_DISPLAY_SCS, HIGH);  
  hal_delayUs(3); //SCS setup time of tsSCS (refer to datasheet for timing details)

  uint16_t oneLineLen = (DISP_HOR_RESOLUTION >> 3) + 2;
  uint8_t buff[oneLineLen];

  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length = oneLineLen * 8;
  t.tx_buffer = buff;

#ifdef LS032B7DD02
  buff[0] = ((line + 1) << 6) | UPD_CMD;  //update one specified line with M0=H,M2=L & AG0:AG1 concatenate to Bit[1:0] sending with LSB first
  buff[1] = (line + 1) >> 2;           //AG2~AG9 in LSB first
#else
  buff[0] = UPD_CMD;                               //update one specified line with M0=H,M2=L sending with LSB first
  buff[1] = line + 1;                           //AG0~AG7 in LSB first for gate line address
#endif

  for(uint16_t dot = 0; dot < DISP_HOR_RESOLUTION >> 3; dot++) {
    buff[dot + 2] = *buf;
    buf++;
  }

  spi_device_polling_transmit(SpiDevice, &t);
  SendDummyBytes();

  hal_delayUs(1); //SCS hold time of thSCS (refer to datasheet for timing details)
  digitalWrite(GFX_DISPLAY_SCS, LOW);  
}

/**
 * @brief Function to update multiple lines
 * @param start_line indicates the starting line number ranges 1~DISP_VER_RESOLUTION
 * @param end_line indicates the ending line number ranges 1~DISP_VER_RESOLUTION
 * @param *buf is a pointer to data
 */
static void GFXDisplayUpdateBlock(uint16_t start_line, uint16_t end_line, uint8_t *buf)
{
  if((start_line > end_line) || (start_line >= DISP_VER_RESOLUTION))
    return;

  int16_t _end_line = MIN(end_line, DISP_VER_RESOLUTION - 1);	//clip the ending gate line address

  for(uint16_t line = start_line; line <= _end_line; line++) {
    GFXDisplayUpdateLine(line, buf);
    buf += DISP_HOR_RESOLUTION >> 3;
  }
}

/**
 * @brief	Print a character from MCU's Flash with data created by BitFontCreator
 * @param	(x,y) is the top left corner coordinates
 * @param	BFC_FONT* pFont is a pointer to font data from MCU's Flash. Font data created by BitFontCreator (http://www.iseasoft.com)
 * @param	ch in 2-byte width represents the character
 * @param	color is the font color BLACK/WHITE
 * @param	bg is the background color BLACK/WHITE/TRANSPARENT. TRANSPARENT means the background is not changed.
 * @return	width of character printed
 * @note	Example <br>
 *			extern const BFC_FONT fontArial_Rounded_MT_Bold37h;	//font map Arial_Rounded_MT_Bold37.c created by BitFontCreator
 *			//...
 *			GFXDisplayPutChar(50,50,&fontArial_Rounded_MT_Bold37h, 0x0042, BLACK, WHITE);	//print the ASCII character 'B' with black stroke on white background
 */
uint16_t GFXDisplayPutChar(uint16_t x, uint16_t y, const BFC_FONT* pFont, const uint16_t ch, COLOR color, COLOR bg)
{
  return (uint16_t)bfc_DrawChar_RowRowUnpacked(x,y,pFont,ch,color, bg);
}

/**
 * @brief	Print string of ASCII code of 1 byte width
 * @param	(x,y) is the top left corner coordinates
 * @param	BFC_FONT* pFont is a pointer to font data from MCU's Flash. Font data created by BitFontCreator (http://www.iseasoft.com)
 * @param	*str is a pointer to character array from Flash space
 * @param	color is the font color BLACK/WHITE
 * @param	bg is the background color BLACK/WHITE/TRANSPARENT. TRANSPARENT means the background is not changed.
 * @return	width of string printed
 */
uint16_t GFXDisplayPutString(uint16_t x, uint16_t y, const BFC_FONT* pFont, const char *str, COLOR color, COLOR bg)
{
	uint16_t _x = x;
	uint16_t _y = y;
	uint16_t width = 0;
	char ch = 0;
	
	if( pFont == 0 || str == 0 )
		return 0;

	while(*str != '\0')
	{
		ch = *str;
		width = GFXDisplayPutChar(_x, _y, pFont, ch, color, bg);
		str++;
		_x += width;
	}  	
	
	return (uint16_t)(_x-x);
}

/**
 * @brief	Print string of ASCII code of 2 bytes width that means Unicode can be printed (Chinese, Japanese, etc)
 * @param	(x,y) is the top left corner coordinates
 * @param	BFC_FONT* pFont is a pointer to font data from MCU's Flash. Font data created by BitFontCreator (http://www.iseasoft.com)
 * @param	*str is a pointer to character array from Flash space
 * @param	color is the font color BLACK/WHITE
 * @param	bg is the background color BLACK/WHITE/TRANSPARENT. TRANSPARENT means the background is not changed.
 * @return	width of string printed
 * @note	Example<br>
 *			const uint16_t hello_japanese[]={0x3053, 0x3093, 0x306B, 0x3061, 0x306F, '\0'};
 *			//....
 *			GFXDisplayPutWString(100,150,&fontSimHei_35h, hello_japanese, BLACK, WHITE);	//SimHei_35h.c created by BitFontCreator
 */
uint16_t GFXDisplayPutWString(uint16_t x, uint16_t y, const BFC_FONT* pFont, const uint16_t *str, COLOR color, COLOR bg)
{
	uint16_t _x = x;
	uint16_t _y = y;
	uint16_t width = 0;
	uint16_t ch = 0;
	
	if( pFont == 0 || str == 0 )
		return 0;

	while(*str != '\0')
	{
		ch = *str;
		width = GFXDisplayPutChar(_x, _y, pFont, ch, color, bg);
		str++;
		_x += width;
	}  	
	
	return (uint16_t)(_x-x);	
}

/**
 * @brief	Decode BFC font
 */
static uint16_t bfc_DrawChar_RowRowUnpacked(uint16_t x0, uint16_t y0, const BFC_FONT *pFont, uint16_t ch, COLOR color, COLOR bg)
{
  // 1. find the character information first
  const BFC_CHARINFO *pCharInfo = GetCharInfo(pFont, (unsigned short)ch);
  
  if( pCharInfo != 0 )
  {
    int height = pFont->FontHeight;
    //USE_SERIAL.print("Font height = "); USE_SERIAL.println(height);
    int width = pCharInfo->Width;
    //USE_SERIAL.print("Font width = "); USE_SERIAL.println(width);
    const unsigned char *pData = pCharInfo->p.pData8;   // pointer to data array

    int bpp = GetFontBpp(pFont->FontType);              // how many bits per pixel
    int bytesPerLine = (width * bpp + 7) / 8;           // # bytes in a row
    int bLittleEndian = (GetFontEndian(pFont->FontType)==1);

    uint16_t x, y, _x, _y, col;
    unsigned char data, pixel, bit;
    
    // 2. draw all the pixels in this character
    for(y=0; y<height; y++)
    {
      for(x=0; x<width; x++)
      {
        col = (x * bpp) / 8;       // byte index in the line
        data = pData[y * bytesPerLine + col];

        // every BYTE (8 bits) data includes 8/bpp pixels,
        // we need to get each pixel color index (0,1,2,3... based on bpp) from the BYTE data
        pixel = data;
        
        // bit index in the BYTE
        // For 1-bpp: bit =  x % 8 (Big Endian),   7 -  x % 8 (Little Endian)
        // For 2-bpp: bit = 2x % 8 (Big Endian),   6 - 2x % 8 (Little Endian)
        // For 4-bpp: bit = 4x % 8 (Big Endian),   4 - 4x % 8 (Little Endian)
        bit = bLittleEndian ? (8-bpp)-(x*bpp)%8 : (x*bpp)%8;

        pixel = pixel<<bit;               // clear left pixels
        pixel = pixel>>(8/bpp-1)*bpp;     // clear right pixels
        
        _x = x0+x, 
        _y = y0+y;
          
        if(pixel) 
        {
		  GFXDisplayPutPixel_FB(_x, _y, color);	//update frame buffer, no update on screen yet.
        }
		else
		{
			if(bg!=TRANSPARENT)
				GFXDisplayPutPixel_FB(_x, _y, bg);
		}
      }
    } 
	//update framebuffer for the block area
	GFXDisplayUpdateBlock(y0+1, y0+height, (uint8_t *)&frameBuffer[y0]);
	
    return (uint16_t)width;
  }
  return 0;
} 

/**
 * @brief	Get physical width of the Memory LCD
 * @return	Width of Memory LCD
 */
uint16_t GFXDisplayGetLCDWidth()
{
  return DISP_HOR_RESOLUTION;
}

/**
 * @brief	Get physical height of the Memory LCD
 * @return	Height of Memory LCD
 */
uint16_t GFXDisplayGetLCDHeight()
{
  return DISP_VER_RESOLUTION;
}

/**
 * @brief	Return character width from a BFC font
 * @param	BFC_FONT* pFont is a pointer to font data from MCU's Flash. Font data created by BitFontCreator (http://www.iseasoft.com)
 * @param	ch in 2-byte width represents the character
 * @return	character width
 */
uint16_t GFXDisplayGetCharWidth(const BFC_FONT *pFont, const uint16_t ch)
{
  const BFC_CHARINFO *pCharInfo = GetCharInfo(pFont, (unsigned short)ch);
  uint16_t _width = 0;
  if( pCharInfo != 0 )
  {
    _width = (uint16_t)pCharInfo->Width;
  }
  
  return _width;
}

/**
 * @brief	Return BFC font height
 * @param	BFC_FONT* pFont is a pointer to font data from MCU's Flash. Font data created by BitFontCreator (http://www.iseasoft.com)
 * @return	Font height
 */
uint16_t GFXDisplayGetFontHeight(const BFC_FONT *pFont)
{
	uint16_t _height = 0;
	
    _height = (uint16_t)(pFont->FontHeight);

	return _height;
}

/**
 * @brief	Return a string width from a BFC font
 * @param	*pFont is a pointer to font data from MCU's Flash. Font data created by BitFontCreator (http://www.iseasoft.com)
 * @param	*str is a pointer to character array from Flash space of 1-byte character length
 * @return	string width
 */
uint16_t GFXDisplayGetStringWidth(const BFC_FONT *pFont, const char *str)
{
	uint16_t _x = 0;
	
	if( pFont == 0 || str == 0 )
		return 0;

	while(*str != '\0')
	{
		_x += GFXDisplayGetCharWidth(pFont, (const uint16_t )*str++);
	}
	
	return _x;	
}

/**
 * @brief	Return a wide string width from a BFC font
 * @param	*pFont is a pointer to font data from MCU's Flash. Font data created by BitFontCreator (http://www.iseasoft.com)
 * @param	*str is a pointer to character array from Flash space of 2-byte character length
 * @return	string width
 */
uint16_t GFXDisplayGetWStringWidth(const BFC_FONT *pFont, const uint16_t *str)
{
  uint16_t _x = 0;

  if( pFont == 0 || str == 0 )
    return 0;

  while(*str != '\0')
  {
    _x += GFXDisplayGetCharWidth(pFont, *str++);
  }

  return _x;
}

/**
 * @brief Hardware Abstraction Layer (HAL) for a software delay in millisec
 * @param ms is the delay in millisec
 */
void hal_delayMs(uint32_t ms)
{
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

unsigned long IRAM_ATTR micros()
{
  return (unsigned long) (esp_timer_get_time());
}

/**
 * @brief Hardware Abstraction Layer (HAL) for a software delay in microseconds
 * @param us is the delay in microseconds
 */
void IRAM_ATTR hal_delayUs(uint32_t us)
{
  uint32_t m = micros();
  if (us)
  {
    uint32_t e = (m + us);
    if (m > e) //overflow
    {
      while(micros() > e)
      {
        NOP();
      }
    }
    while(micros() < e)
    {
      NOP();
    }
  }
}

void memorylcd_Init()
{
  //Set SCS pin an output default low
  pinMode(GFX_DISPLAY_SCS, OUTPUT);
  digitalWrite(GFX_DISPLAY_SCS, LOW);
  //Set DISP pin an output default low
  pinMode(GFX_DISPLAY_DISP, OUTPUT);
  digitalWrite(GFX_DISPLAY_DISP, LOW); 
  //Set EXTCOMIN pin an output default low
  pinMode(GFX_DISPLAY_EXTCOMIN, OUTPUT);
  digitalWrite(GFX_DISPLAY_EXTCOMIN, LOW);

  spi_bus_config_t bus_config;
  memset(&bus_config, 0, sizeof(spi_bus_config_t));
  bus_config.mosi_io_num = GFX_DISPLAY_MOSI;
  bus_config.sclk_io_num = GFX_DISPLAY_SCK;
  bus_config.miso_io_num = -1;
  bus_config.quadwp_io_num = -1;
  bus_config.quadhd_io_num = -1;
  //bus_config.data0_io_num = -1;
  //bus_config.data1_io_num = -1;
  bus_config.data2_io_num = -1;
  bus_config.data3_io_num = -1;
  bus_config.data4_io_num = -1;
  bus_config.data5_io_num = -1;
  bus_config.data6_io_num = -1;
  bus_config.data7_io_num = -1;
  bus_config.max_transfer_sz = 256;
  bus_config.intr_flags = 0;
  spi_bus_initialize(SpiHost, &bus_config, 0);

  spi_device_interface_config_t dev_config;
  memset(&dev_config, 0, sizeof(spi_device_interface_config_t));
  dev_config.command_bits = 0;
  dev_config.address_bits = 0;
  dev_config.dummy_bits = 0;
  dev_config.mode = 0;//3;//2;//1;//0;
  dev_config.clock_speed_hz = 1 * 1000 * 1000;
  dev_config.spics_io_num = -1;
  //dev_config.post_cb = spi_end;
  dev_config.duty_cycle_pos = 128;
  dev_config.queue_size = 3;
    dev_config.flags = SPI_DEVICE_BIT_LSBFIRST;
    spi_bus_add_device(SpiHost, &dev_config, &SpiDevice);
}

void memorylcd_update(uint8_t *buff, uint16_t len) {
  //GFXDisplayUpdateBlock(0, 239, buff);

  GFXDisplayUpdateBlock(0, 239, buff);
  
} 

/**
 * @brief HAL function to start EXTCOMIN pulse
 * @param hz is the pulse frequency measured in hertz. 
 * @note  EXTCOMIN frequency should be made lower than frame frequency.
 *        Reference: http://forum.arduino.cc/index.php?topic=425385.0
 */
void hal_extcom_start(uint16_t hz)
{
    Timer = xTimerCreate("Timer", 1000 / hz, pdTRUE, (void *) 0, hal_extcom_toggle);

    if(Timer) {
        xTimerStart(Timer, 0 );
    }
}

/**
 * @brief HAL function to stop EXTCOMIN pulse
 */
void hal_extcom_stop()
{
    if(Timer)
    {
        xTimerStop(Timer, 10);
        xTimerDelete(Timer, 10);
        Timer = 0;
    }
}

/**
 * brief	HAL function to toggle EXTCOMIN pin
 */
void /*IRAM_ATTR*/ hal_extcom_toggle(TimerHandle_t xTimer)
{
    digitalWrite(GFX_DISPLAY_EXTCOMIN, ExtPinLast);
    ExtPinLast = !ExtPinLast;
}
