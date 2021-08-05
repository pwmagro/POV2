/**
 * @file app.c
 * @author Peter Magro
 * @date August 4th, 2021
 * @brief app.c contains functions for enabling the LETIMER0 and peripherals.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_assert.h"
#include "em_core.h"

#include "app.h"
#include "adc.h"
#include "brd_config.h"
#include "battery.h"
#include "bmp280.h"
#include "cmu.h"
#include "gpio.h"
#include "HW_delay.h"
#include "i2c.h"
#include "letimer.h"
#include "si7021.h"
#include "timer.h"
#include "pov.h"
#include "usart.h"
#include "ws2812b.h"
#include "font.h"

//***********************************************************************************
// defined files
//***********************************************************************************

/*
 * Uncomment to enable TDD function calls
 */
//#define SI7021_TEST_ENABLED
//#define BMP280_TEST_ENABLED

//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//********************//
// Callback Functions //
//********************//
/***************************************************************************//**
 * @brief
 *		Boot up callback function.
 *
 * @details
 *		Starts the POV measure timers and battery polling timer. Also calls SI7021
 *		and BMP280 TDD functions, if enabled.
 *
 ******************************************************************************/
void scheduled_boot_up_cb(void) {

#ifdef SI7021_TEST_ENABLED
	si7021_i2c_test(0);
#endif
#ifdef BMP280_TEST_ENABLED
	bmp280_i2c_test(0);
#endif
	remove_scheduled_event(BOOT_UP_CB);
	timer_start(POV_MEASURE_TIMER, TWO_SECONDS, UINT32_MAX);
	letimer_start(BATTERY_LETIMER, true);
}

/***************************************************************************//**
 * @brief
 *		SI7021 humidity callback function.
 *
 * @details
 *		Updates the humidity variable in pov.c and begins a temperature read.
 *
 ******************************************************************************/
void scheduled_si7021_humidity_cb(void) {
	remove_scheduled_event(SI7021_HUMIDITY_CB);
	pov_update_humidity();
	si7021_read(SI7021_TEMPERATURE_NO_HOLD, SI7021_TEMP_CB, R_MSB_First);
}

/***************************************************************************//**
 * @brief
 *		SI7021 temperature callback function.
 *
 * @details
 *		Updates the temperature variable in pov.c.
 *
 ******************************************************************************/
void scheduled_si7021_temp_cb(void) {
	remove_scheduled_event(SI7021_TEMP_CB);
	pov_update_si7021_temp();
}

/***************************************************************************//**
 * @brief
 *		BMP280 temperature callback function.
 *
 * @details
 *		Begins a pressure read.
 *
 ******************************************************************************/
void scheduled_bmp280_temp_cb(void) {
	remove_scheduled_event(BMP280_TEMP_CB);
	bmp280_read_pressure();
}

/***************************************************************************//**
 * @brief
 *		BMP280 pressure callback function.
 *
 * @details
 *		Calls pov_update_bmp280().
 *
 ******************************************************************************/
void scheduled_bmp280_pressure_cb(void) {
	remove_scheduled_event(BMP280_PRESSURE_CB);
	pov_update_bmp280();
}

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		Opens all peripherals.
 *
 * @details
 *		Opens CMU, GPIO, sleep mode manager, scheduler, battery, and POV app.
 *
 ******************************************************************************/
void app_peripheral_setup(void) {

	// System preparation
	sleep_block_mode(SYSTEM_BLOCK_EM);

	// Open peripherals
	cmu_open();
	gpio_open(GPIO_EVEN_CB, GPIO_ODD_CB);
	sleep_open();
	scheduler_open();
	battery_open();
	pov_open();

	// Cleanup
	add_scheduled_event(BOOT_UP_CB);
}

/***************************************************************************//**
 * @brief
 *		Runs all events from the scheduler.
 *
 * @details
 *		Gets scheduled events from the scheduler, then calls each CB function as needed.
 *
 * @note
 *		Callback macros are defined in app.h.
 *
 ******************************************************************************/
void run_scheduled_events(uint32_t scheduled_events) {

	if (scheduled_events & BOOT_UP_CB) {
		scheduled_boot_up_cb();
	}

	if (scheduled_events & SI7021_HUMIDITY_CB) {
		scheduled_si7021_humidity_cb();
	}

	if (scheduled_events & SI7021_TEMP_CB) {
		scheduled_si7021_temp_cb();
	}

	if (scheduled_events & BMP280_TEMP_CB) {
		scheduled_bmp280_temp_cb();
	}
}

/*
 * Interrupt Functions
 */
/***************************************************************************//**
 * @brief
 *		Handles interrupts from even GPIO.
 *
 * @details
 *		Interrupts raised by buttons will change what's shown on the display.
 *		Interrupts raised by the hall effect sensor will call the POV calibration
 *		function.
 *
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void) {
	uint32_t int_flag = GPIO->IF & GPIO->IEN;
	GPIO->IFC = int_flag;

	// Interrupts from buttons
	if (int_flag & (1u << BUTTON_0_INT_NUM)) {
		pov_change_mode(true);
	}

	// Interrupts from Hall effect sensor
	if (int_flag & (1u << HALL_EFFECT_INT_NUM)) {
		uint32_t count = timer_measure_restart(POV_MEASURE_TIMER);
		pov_handle_measure(count);
	}
}

/***************************************************************************//**
 * @brief
 *		Handles interrupts from odd GPIO.
 *
 * @details
 *		Interrupts raised by buttons will change what's shown on the display.
 *
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void) {
	uint32_t int_flag = GPIO->IF & GPIO->IEN;
	GPIO->IFC = int_flag;

	// Interrupts from buttons
	if (int_flag & (1u << BUTTON_1_INT_NUM)) {
		pov_change_mode(false);
	}
}
