/**
 * @file app.c
 * @author Peter Magro
 * @date June 21st, 2021
 * @brief app.c contains functions for enabling the LETIMER0 and peripherals.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"


//***********************************************************************************
// defined files
//***********************************************************************************

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

void scheduled_letimer_uf_cb(void) {
	remove_scheduled_event(LETIMER_UF_CB);
	battery_poll();
}

void scheduled_si7021_humidity_cb(void) {
	remove_scheduled_event(SI7021_HUMIDITY_CB);
	pov_update_humidity();
	si7021_read(SI7021_TEMPERATURE_NO_HOLD, SI7021_TEMP_CB, R_MSB_First);
}

void scheduled_si7021_temp_cb(void) {
	remove_scheduled_event(SI7021_TEMP_CB);
	pov_update_si7021_temp();
}

void scheduled_bmp280_temp_cb(void) {
	remove_scheduled_event(BMP280_TEMP_CB);
	bmp280_read_pressure();
}

void scheduled_bmp280_pressure_cb(void) {
	remove_scheduled_event(BMP280_PRESSURE_CB);
	pov_update_bmp280();
}

void scheduled_bmp280_open_cb(void) {
	remove_scheduled_event(BMP280_OPEN_CB);

}
//***********************************************************************************
// Global functions
//***********************************************************************************



/***************************************************************************//**
 * @brief
 *		Opens all peripherals.
 *
 *
 * @details
 *		Opens CMU, GPIO, sets LETIMER0 to PWM mode, and starts LETIMER0.
 *
 * @note
 *		PWM parameters are defined in app.h.
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
	battery_open(LETIMER_UF_CB);
	pov_open();

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
	if (scheduled_events & LETIMER_UF_CB) {
		scheduled_letimer_uf_cb();
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

	if (scheduled_events & BMP280_PRESSURE_CB) {
		scheduled_bmp280_pressure_cb();
	}

}

/*
 * Interrupt Functions
 */
/***************************************************************************//**
 * @brief
 *		Handles interrupts from GPIO.
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

void GPIO_ODD_IRQHandler(void) {
	uint32_t int_flag = GPIO->IF & GPIO->IEN;
	GPIO->IFC = int_flag;

	// Interrupts from buttons
	if (int_flag & (1u << BUTTON_1_INT_NUM)) {
		pov_change_mode(false);
	}
}

//************************************
// These need to be moved to pov.c!
//************************************

/***************************************************************************//**
 * @brief
 *		Interrupt handler for TIMER1.
 *
 * @details
 *		If the timer has reached the end of a one-shot, will start or stop the
 *		display cycle as appropriate. If the timer has reached the compare value,
 *		the display will be advanced.
 *
 ******************************************************************************/
void WTIMER1_IRQHandler(void) {
	uint32_t int_flag = WTIMER1->IF & WTIMER1->IEN;
	WTIMER1->IFC = int_flag;

	// Interrupts from overflow
	if (int_flag & TIMER_IF_OF) {
		if (pov_get_position() == dead_one) {
			pov_start_display();
		}
		if (pov_get_position() == display) {
			pov_end_display();
		}
	}

	// Interrupts from CC0
	if (int_flag & TIMER_IF_CC0) {
		pov_tick();
	}
}

/***************************************************************************//**
 * @brief
 *		Interrupt handler for TIMER0.
 *
 * @details
 *		If TIMER0 (the timer used for calibration) overflows, it is assumed that
 *		the display has stopped spinning, and the "menu" mode is activated.
 *
 ******************************************************************************/
void WTIMER0_IRQHandler(void) {
	static bool blah;
	uint32_t int_flag = WTIMER0->IF & WTIMER0->IEN;
	WTIMER0->IFC = int_flag;

	// Interrupts from overflow
	if (int_flag & TIMER_IF_OF) {
		pov_show_menu();
		blah = !blah;
//		GRB_TypeDef bleh[12] = {
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah},
//				{0, 0, 8*blah}
//		};
//		ws2812b_write(bleh);

		timer_measure_restart(POV_MEASURE_TIMER);
	}
}
