#pragma once

#include <stdio.h>
#include "driver/gpio.h"

// Digital pin states
#define HIGH						1
#define LOW							0

// Pin mode
#define INPUT						1
#define INPUT_PULLUP				2
#define OUTPUT						3
#define OUTPUT_OD					4

#define digitalWrite(pin, val)		gpio_set_level((gpio_num_t) pin, val)
#define digitalRead(pin)			gpio_get_level((gpio_num_t) pin)

void gpioReset(uint8_t pinNum);
esp_err_t pinMode(uint8_t pinNum, uint8_t mode);
