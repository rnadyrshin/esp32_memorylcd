/**
* @brief  	Arduino driver for Sharp Memory LCD. 5 LCD models are supported with ESP32 or Arduino M0 PRO.
*			Wiring details is described in MemoryLCD.h under @note pin number definition, hardware dependent
*/
//#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_timer.h>
#include <sys/time.h>
#include "driver/gpio.h"
#include "HalGpio.h"
#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "MemoryLCD.h"

#define NOP() asm volatile ("nop")
#define UPD_CMD     0x01    //0x80
#define VCOM_CMD    0x02    //0x40
#define CLEAR_CMD   0x03    //0x20

#define DISP_HIGH()     digitalWrite(MEMORYLCD_DISP, HIGH)
#define DISP_LOW()      digitalWrite(MEMORYLCD_DISP, LOW)
#define SCS_HIGH()      digitalWrite(MEMORYLCD_SCS, HIGH)
#define SCS_LOW()       digitalWrite(MEMORYLCD_SCS, LOW)



static bool ExtPinLast = false;
static spi_host_device_t SpiHost = SPI_HOST;
static spi_device_handle_t SpiDevice = 0;
static TimerHandle_t Timer;

static void hal_extcom_start(uint16_t hz);
static void hal_extcom_stop();
static void hal_extcom_toggle(TimerHandle_t xTimer);
static void hal_delayUs(uint32_t us);
static void memorylcd_UpdateLine(uint16_t line, uint8_t *buf);
static void memorylcd_UpdateBlock(uint16_t start_line, uint16_t end_line, uint8_t *buf);


void memorylcd_DrawPixel(uint8_t *buff, uint16_t dispWidth, uint16_t x, uint16_t y, uint8_t color)
{
    //uint8_t maskBit = 0x80 >> (x & 0x07);	//SPI data sent with MSB first
    uint8_t maskBit = 0x01 << (x & 0x07);	//SPI data sent with LSB first
    uint16_t byteIdx = y * (dispWidth >> 3) + (x >> 3);

    if(!color)
        buff[byteIdx] |= maskBit;
    else
        buff[byteIdx] &= ~maskBit;
}

void memorylcd_Clear()
{
    SCS_HIGH();
    hal_delayUs(3); //SCS setup time of tsSCS (refer to datasheet for timing details)

    uint8_t buff[2] = {CLEAR_CMD, 0x00};

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 2 * 8;
    t.tx_buffer = buff;
    spi_device_polling_transmit(SpiDevice, &t);

    hal_delayUs(1); //SCS hold time of thSCS (refer to datasheet for timing details)
    SCS_LOW();
}

void memorylcd_PowerOn()
{
    hal_delayUs(30);
    memorylcd_Clear();

    hal_delayUs(30);
    DISP_HIGH();
    hal_delayUs(30);
    hal_extcom_start(EXTCOMIN_FREQ); //turn on EXTCOMIN pulse
    hal_delayUs(30);
}

void memorylcd_PowerOff()
{
    memorylcd_Clear();
    DISP_LOW();
    hal_extcom_stop();  	//stop EXTCOMIN pulse
    hal_delayUs(30);
}

void SendDummyBytes() {
  uint8_t buff[2] = {0, 0};

  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length = 2 * 8;
  t.tx_buffer = buff;
  spi_device_polling_transmit(SpiDevice, &t);
}

static void memorylcd_UpdateLine(uint16_t line, uint8_t *buf)
{
  if(line >= DISP_VER_RESOLUTION)
    return;

  SCS_HIGH();
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
  SCS_LOW();
}

static void memorylcd_UpdateBlock(uint16_t start_line, uint16_t end_line, uint8_t *buf)
{
  if((start_line > end_line) || (start_line >= DISP_VER_RESOLUTION))
    return;

  int16_t _end_line = MIN(end_line, DISP_VER_RESOLUTION - 1);	//clip the ending gate line address

  for(uint16_t line = start_line; line <= _end_line; line++) {
    memorylcd_UpdateLine(line, buf);
    buf += DISP_HOR_RESOLUTION >> 3;
  }
}

unsigned long IRAM_ATTR micros()
{
  return (unsigned long) (esp_timer_get_time());
}

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
  pinMode(MEMORYLCD_SCS, OUTPUT);
  digitalWrite(MEMORYLCD_SCS, LOW);
  //Set DISP pin an output default low
  pinMode(MEMORYLCD_DISP, OUTPUT);
  digitalWrite(MEMORYLCD_DISP, LOW); 
  //Set EXTCOMIN pin an output default low
  pinMode(MEMORYLCD_EXTCOMIN, OUTPUT);
  digitalWrite(MEMORYLCD_EXTCOMIN, LOW);

  spi_bus_config_t bus_config;
  memset(&bus_config, 0, sizeof(spi_bus_config_t));
  bus_config.mosi_io_num = MEMORYLCD_MOSI;
  bus_config.sclk_io_num = MEMORYLCD_SCK;
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
  dev_config.mode = 0;
  dev_config.clock_speed_hz = 14 * 1000 * 1000;
  dev_config.spics_io_num = -1;
  dev_config.duty_cycle_pos = 128;
  dev_config.queue_size = 3;
    dev_config.flags = SPI_DEVICE_BIT_LSBFIRST;
    spi_bus_add_device(SpiHost, &dev_config, &SpiDevice);
}

void memorylcd_Update(uint8_t *buff, uint16_t len) {
  memorylcd_UpdateBlock(0, 239, buff);
} 

void hal_extcom_start(uint16_t hz)
{
    Timer = xTimerCreate("Timer", 1000 / hz, pdTRUE, (void *) 0, hal_extcom_toggle);

    if(Timer) {
        xTimerStart(Timer, 0 );
    }
}

void hal_extcom_stop()
{
    if(Timer)
    {
        xTimerStop(Timer, 10);
        xTimerDelete(Timer, 10);
        Timer = 0;
    }
}

void IRAM_ATTR hal_extcom_toggle(TimerHandle_t xTimer)
{
    digitalWrite(MEMORYLCD_EXTCOMIN, ExtPinLast);
    ExtPinLast = !ExtPinLast;
}
