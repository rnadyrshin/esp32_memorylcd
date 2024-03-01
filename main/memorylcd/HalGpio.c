#include "HalGpio.h"

void gpioReset(uint8_t pinNum)
{
    gpio_reset_pin((gpio_num_t) pinNum);
}

esp_err_t pinMode(uint8_t pinNum, uint8_t mode)
{
	switch (mode)
	{
	case OUTPUT:
		return gpio_set_direction((gpio_num_t) pinNum, GPIO_MODE_OUTPUT);
	case OUTPUT_OD:
		return gpio_set_direction((gpio_num_t) pinNum, GPIO_MODE_OUTPUT_OD);
	case INPUT:
	{
		esp_err_t err = gpio_set_direction((gpio_num_t) pinNum, GPIO_MODE_INPUT);
		if (err != ESP_OK)
			return err;
		return gpio_set_pull_mode((gpio_num_t) pinNum, GPIO_FLOATING);
	}
	case INPUT_PULLUP:
	{
		esp_err_t err = gpio_set_direction((gpio_num_t) pinNum, GPIO_MODE_INPUT);
		if (err != ESP_OK)
			return err;
		return gpio_set_pull_mode((gpio_num_t) pinNum, GPIO_PULLUP_ONLY);
	}
	default:
		return ESP_ERR_INVALID_ARG;
	}
}
