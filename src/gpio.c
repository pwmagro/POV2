/**
 * @file gpio.c
 * @author Peter Magro
 * @date July 19th, 2021
 * @brief gpio.c enables and configures pin outs.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"

#include "em_cmu.h"
#include "em_assert.h"

#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t gpio_even_irq_cb;
static uint32_t gpio_odd_irq_cb;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Enable and configure the GPIO pins.
 *
 * @details
 * 	 This function sets the drive strength and pin mode of the GPIO pins.
 *
 * @note
 *   Settings are defined in brd_config.h
 *
 ******************************************************************************/
void gpio_open(uint32_t gpio_even_callback, uint32_t gpio_odd_callback){

	CMU_ClockEnable(cmuClock_GPIO, true);

// ADC pin
	GPIO_PinModeSet(ADC_PORT, ADC_PIN, ADC_GPIOMODE, ADC_DEFAULT);
	GPIO->P[ADC_PORT].OVTDIS |= 1u << ADC_PIN;

// I2C pins
	GPIO_PinModeSet(I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_GPIOMODE, I2C_SCL_DEFAULT);
	GPIO_PinModeSet(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SDA_GPIOMODE, I2C_SDA_DEFAULT);

// SPI pins
	GPIO_DriveStrengthSet(WS2812B_SPI_MOSI_PORT, WS2812B_SPI_MOSI_DSTRENGTH);
	GPIO_PinModeSet(WS2812B_SPI_MOSI_PORT, WS2812B_SPI_MOSI_PIN, WS2812B_SPI_MOSI_GPIOMODE, WS2812B_SPI_MOSI_DEFAULT);

// Hall Effect Sensor
	GPIO_PinModeSet(HALL_EFFECT_PORT, HALL_EFFECT_PIN, HALL_EFFECT_GPIOMODE, HALL_EFFECT_DEFAULT);
	GPIO_ExtIntConfig(HALL_EFFECT_PORT, HALL_EFFECT_PIN, HALL_EFFECT_INT_NUM, HALL_EFFECT_INT_RISING, HALL_EFFECT_INT_FALLING, HALL_EFFECT_INT_EN);

// Si7021 pins
	GPIO_DriveStrengthSet(ENVSENSE_EN_PORT, ENVSENSE_EN_DRIVE_STRENGTH);
	GPIO_PinModeSet(ENVSENSE_EN_PORT, ENVSENSE_EN_PIN, ENVSENSE_EN_GPIOMODE, ENVSENSE_EN_DEFAULT);

	GPIO_PinModeSet(ENVSENSE_SCL_PORT, ENVSENSE_SCL_PIN, ENVSENSE_SCL_GPIOMODE, ENVSENSE_SCL_DEFAULT);
	GPIO_PinModeSet(ENVSENSE_SDA_PORT, ENVSENSE_SDA_PIN, ENVSENSE_SDA_GPIOMODE, ENVSENSE_SDA_DEFAULT);

// Button pins
	/*
	*   I think that what I want to do here is have the buttons set up what sort of information is displayed. BTN1 can scroll "left" through the list of
	*   actions, and BTN0 can scroll "right." The actions can maybe be a pattern, RPM, temperature, and text?
	*/
	// even
	GPIO_PinModeSet(BUTTON_0_PORT, BUTTON_0_PIN, BUTTON_0_CONFIG, BUTTON_DEFAULT);
	GPIO_ExtIntConfig(BUTTON_0_PORT, BUTTON_0_PIN, BUTTON_0_INT_NUM, BUTTON_0_INT_RISING, BUTTON_0_INT_FALLING, BUTTON_0_INT_ENABLE);

	// odd
	GPIO_PinModeSet(BUTTON_1_PORT, BUTTON_1_PIN, BUTTON_1_CONFIG, BUTTON_DEFAULT);
	GPIO_ExtIntConfig(BUTTON_1_PORT, BUTTON_1_PIN, BUTTON_1_INT_NUM, BUTTON_1_INT_RISING, BUTTON_1_INT_FALLING, BUTTON_1_INT_ENABLE);

// interrupt configuration
	gpio_even_irq_cb = gpio_even_callback;
	gpio_odd_irq_cb = gpio_odd_callback;
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}
