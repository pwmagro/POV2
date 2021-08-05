/**
 * @file pov.c
 * @author Peter Magro
 * @date July 23rd, 2021
 * @brief Implements the POV logic and display modes.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "pov.h"

#include <stdio.h>
#include <string.h>

#include "bmp280.h"
#include "timer.h"
#include "font.h"
#include "battery.h"
#include "letimer.h"
#include "math.h"
#include "si7021.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static uint32_t count_one, count_two;
static uint32_t ticks_per_deg;

static enum {
	count_select_one,
	count_select_two
} count_select;

static pov_position current_position;
static GRB_TypeDef display_buffer[DISPLAY_NUM_PIXELS_WIDE][WS2812B_NUM_LEDS];
static volatile uint32_t buffer_index;
static POV_DisplayMode_TypeDef displaymode;

static float temperature;
static float humidity;
//***********************************************************************************
// Private functions
//***********************************************************************************
void pov_core();
void pov_hello_world(POV_Display_TypeDef *display);
void pov_temp_humidity_start();
void pov_credits(POV_Display_TypeDef *display);
void pov_battery_level(POV_Display_TypeDef *display);
void pov_bmp280_start(void);
void pov_filler(POV_Display_TypeDef *display);
void hsv_to_grb(uint8_t H, uint8_t S, uint8_t V, GRB_TypeDef *ret);

/***************************************************************************//**
 * @brief
 *		Calls relevant display function depending on current display mode.
 *
 ******************************************************************************/
void pov_core() {
	POV_Display_TypeDef display;

	// Displays "Hello World"
	switch(displaymode) {
	case HelloWorld:
		pov_hello_world(&display);
		pov_update_display(display);
		break;

	// Displays temperature and humidity
	case TempHumidity:
		pov_temp_humidity_start();
		break;

	// Displays "Keith Graham" and "Peter Magro"
	case Credits:
		pov_credits(&display);
		pov_update_display(display);
		break;

	// Displays the battery level as a percentage
	case BatteryLevel:
		pov_battery_level(&display);
		pov_update_display(display);
		break;

	case PressureAltitude:
		pov_bmp280_start();
		break;

	default:
		pov_filler(&display);
		pov_update_display(display);
		break;
	}
}

/***************************************************************************//**
 * @brief
 *		Displays "Hello World" on the POV display.
 *
 * @details
 *		Slowly rotates through the color spectrum, with top and bottom colors
 *		180 degrees apart.
 *
 * @param[in] display
 * 		The display variable to write to.
 *
 ******************************************************************************/
void pov_hello_world(POV_Display_TypeDef *display) {
	static uint8_t H;
	uint8_t S = 255;
	uint8_t V = 16;

	// Shift hue each call
	H += 5;

	GRB_TypeDef top_color;
	GRB_TypeDef bottom_color;

	// Convert HSV value to GRB
	hsv_to_grb(H, S, V, &top_color);
	hsv_to_grb(H + 128, S, V, &bottom_color);

	// Write to return variable
	display->top_string = "      HELLO     ";
	display->bottom_string = "      WORLD     ";
	for (int i = 0; i < 12; i++) {
		display->top_colors[i] = top_color;
		display->bottom_colors[i] = bottom_color;
	}
}

/***************************************************************************//**
 * @brief
 *		Begins the SI7021 humidity read sequence. Will result in calls to
 *		pov_update_humidity() and pov_update_si7021_temp().
 *
 ******************************************************************************/
void pov_temp_humidity_start() {
	si7021_read(SI7021_RELATIVE_HUMIDITY_NO_HOLD, SI7021_HUMIDITY_CB, R_MSB_First);
}

/***************************************************************************//**
 * @brief
 * 		Displays "Keith Graham" and "Peter Magro" in white and gold.
 *
 * @param[in] display
 * 		The display variable to write to.
 *
 ******************************************************************************/
void pov_credits(POV_Display_TypeDef *display) {
	GRB_TypeDef top_color = { 6, 6, 6 };
	GRB_TypeDef bottom_color = { 10, 8, 0 };

	display->top_string = "  Keith Graham  ";
	display->bottom_string = "   Peter Magro  ";

	for (uint32_t i = 0; i < DISPLAY_NUM_CHARS; i++) {
		display->top_colors[i] = top_color;
		display->bottom_colors[i] = bottom_color;
	}
}

/***************************************************************************//**
 * @brief
 *		Displays the battery level on the POV display.
 *
 * @note
 * 		Not yet implemented.
 *
 * @param[in]
 * 		The display variable to write to.
 *
 ******************************************************************************/
void pov_battery_level(POV_Display_TypeDef *display) {
	display->top_string = "    Battery:    ";

	char bottom[17];
	uint32_t len = sprintf(bottom, "      %.2f", battery_get_percent());
	for (uint32_t i = len; i < DISPLAY_NUM_CHARS; i++) {
		bottom[i] = ' ';
	}
	strcpy(display->bottom_string, bottom);

	GRB_TypeDef color = { 0, 8, 0 };

	for (uint32_t i = 0; i < DISPLAY_NUM_CHARS; i++) {
		display->top_colors[i] = color;
		display->bottom_colors[i] = color;
	}
}

/***************************************************************************//**
 * @brief
 *		Begins the BMP280 read sequence.
 *
 ******************************************************************************/
void pov_bmp280_start(void) {
	bmp280_read_temp();
}

/***************************************************************************//**
 * @brief
 *		Opens POV and required peripherals.
 *
 * @details
 *		Opens TIMER0 for calibration measurements, and TIMER1 for display clocking.
 *
 ******************************************************************************/
void pov_filler(POV_Display_TypeDef *display) {
	GRB_TypeDef top_color = { 6, 6, 6 };
	GRB_TypeDef bottom_color = { 6, 6, 6 };

	display->top_string = "     Filler     ";
	display->bottom_string = "     Filler     ";

	for (uint32_t i = 0; i < DISPLAY_NUM_CHARS; i++) {
		display->top_colors[i] = top_color;
		display->bottom_colors[i] = bottom_color;
	}
}

/***************************************************************************//**
 * @brief
 *		Converts an HSV value to a GRB value.
 *
 * @details
 *		Implements formulas from https://www.rapidtables.com/convert/color/hsv-to-rgb.html
 *
 * @param[in] hue
 * 		The H value from 0 to 255.
 *
 * @param[in] saturation
 *		The S value from 0 to 255.
 *
 * @param[in] value
 * 		The V value from 0 to 255.
 *
 * @param[in] ret
 * 		The GRB_TypeDef variable to write the converted results to.
 ******************************************************************************/
void hsv_to_grb(uint8_t hue, uint8_t saturation, uint8_t value, GRB_TypeDef *ret) {

	// Convert hue to 360-degree scale and S/V to percent values
	double H = ((double)hue) * 360 / 255;
	double S = ((double)saturation) / 255;
	double V = ((double)value) / 255;

	// Formulas from rapidtables
	double C, X, m, rp, gp, bp;
	uint8_t g, r, b;

	C = S * V;
	X = C * (1 - fabs(fmod((H / 60), 2.0) - 1));
	m = V - C;
	if (0 <= H && H < 60) {
		rp = C;
		gp = X;
		bp = 0;
	} else if (60 <= H && H < 120) {
		rp = X;
		gp = C;
		bp = 0;
	} else if (120 <= H && H < 180) {
		rp = 0;
		gp = C;
		bp = X;
	} else if (180 <= H && H < 240) {
		rp = 0;
		gp = X;
		bp = C;
	} else if (240 <= H && H < 300) {
		rp = X;
		gp = 0;
		bp = C;
	} else if (300 <= H && H < 360) {
		rp = C;
		gp = 0;
		bp = X;
	}
	g = (uint8_t)((gp + m) * 255);
	r = (uint8_t)((rp + m) * 255);
	b = (uint8_t)((bp + m) * 255);

	// Update return variable
	ret->g = g;
	ret->r = r;
	ret->b = b;
}

//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *		Opens POV and required peripherals.
 *
 * @details
 *		Opens TIMER0 for calibration measurements, and TIMER1 for display clocking.
 *
 ******************************************************************************/
void pov_open(void) {
	// Reset all values
	count_select = count_select_one;
	count_one = 0;
	count_two = 0;
	humidity = 0;
	temperature = 0;

	// Timer settings
	TIMER_MEASURE_TypeDef timer_struct;
	timer_struct.enable = false;						// Don't run timer
	timer_struct.debugRun = false;						// Pause timer when debug paused
	timer_struct.prescale = timerPrescale1;				// 38 MHz
	timer_struct.clkSel = timerClkSelHFPerClk;			// HFPERCLOCK
	timer_struct.fallAction = timerInputActionNone;		// Do nothing on falling edge input
	timer_struct.riseAction = timerInputActionNone;		// Do nothing on rising edge input
	timer_struct.oneShot = true;						// Do not loop

	displaymode = TempHumidity;

	// Open peripherals
	timer_open(POV_MEASURE_TIMER, &timer_struct);
	timer_open(POV_TICK_TIMER, &timer_struct);
	ws2812b_open();
	si7021_i2c_open(ENVSENSE_I2C_PERIPHERAL, true);
	bmp280_open(BMP280_TEMP_CB, BMP280_PRESSURE_CB);
}

/***************************************************************************//**
 * @brief
 *		Handles pulses from the SI7201 and calibrates display.
 *
 * @details
 *		This function has two static counters, which alternate holding values from
 *		TIMER0. The values are then compared to determine which position the display
 *		is in; if the most recent counter is larger than the last, the display is
 *		12.5 degrees CCW of the bottom; if it is smaller, the display is 12.5 degrees
 *		CW of the bottom. This information is then used to find the ratio of the
 *		speed of the measurement timer to the speed of the display.
 *
 *		The timer also begins the tick timer depending on current position.
 *
 * @param[in] count
 * 		The value to update the next counter with.
 *
 ******************************************************************************/
void pov_handle_measure(uint32_t count) {

	// Update the appropriate count value, switch to the other count value, and determine
	// the current position of the display
	if (count_select == count_select_one) {
		count_one = count;
		count_select = count_select_two;

		if (count_one > count_two) {
			current_position = measure;
		} else {
			current_position = dead_one;
		}
	} else if (count_select == count_select_two) {
		count_two = count;
		count_select = count_select_one;

		if (count_two > count_one) {
			current_position = measure;
		} else {
			current_position = dead_one;
		}
	} else EFM_ASSERT(false);

	// Start display sequence, if necessary
	if (current_position == dead_one) {
		ticks_per_deg = (float)(count_one + count_two) / DEGREES_360;
		timer_start(POV_TICK_TIMER, ticks_per_deg * DEAD_ZONE_WIDTH, UINT32_MAX);

	}
}

/***************************************************************************//**
 * @brief
 *		Begins the LED sequence.
 *
 * @details
 *		Resets display buffer, starts the tick timer, and sets current_position
 *		to display.
 *
 ******************************************************************************/
void pov_start_display(void) {
	pov_core();
	buffer_index = 0;
	timer_start(POV_TICK_TIMER, ticks_per_deg * DISPLAY_ZONE_WIDTH, ticks_per_deg * DISPLAY_PIXEL_WIDTH);
	current_position = display;
}

/***************************************************************************//**
 * @brief
 *		Ends the LED sequence.
 *
 * @details
 *		Turns off all LEDs and sets current_position to dead_two.
 *
 ******************************************************************************/
void pov_end_display(void) {

	// Create an array of all black (off)
	GRB_TypeDef clear[WS2812B_NUM_LEDS];
	for (uint32_t i = 0; i < WS2812B_NUM_LEDS; i++) {
		clear[i].g = 0;
		clear[i].r = 0;
		clear[i].b = 0;
	}

	// Write blanks to the LEDs
	ws2812b_write(clear);
	current_position = dead_two;
}

/***************************************************************************//**
 * @brief
 *		Turns on the LEDs.
 *
 * @details
 *		Writes from display buffer to LEDs, advances display buffer, and increases
 *		tick timer's compare value to next trigger point.
 *
 ******************************************************************************/
void pov_tick(void) {
	ws2812b_write(display_buffer[buffer_index]);
	POV_TICK_TIMER->CC[0].CCV += (uint32_t)(ticks_per_deg * DISPLAY_PIXEL_WIDTH);
	buffer_index++;
}

/***************************************************************************//**
 * @brief
 *		Formats strings and color data to pixels data and writes to display buffer.
 *
 * @details
 *		Converts strings to POV_CHAR arrays, then writes appropriate pixels with
 *		colors set in display struct.
 *
 * @note
 *		A low battery will always override the written value with "Low Battery
 *		/ Recharge Soon".
 *
 * @param[in] display
 * 		A struct which contains a top string, bottom string, and colors for each
 * 		character.
 *
 ******************************************************************************/
void pov_update_display(POV_Display_TypeDef display) {

	// If the battery is low, display a low battery message regardless of what's
	// been written to the display
	if (battery_check_low()) {
		display.top_string = "   Low Battery  ";
		display.bottom_string = "  Recharge Soon ";

		for (int i = 0; i < 16; i++) {
			display.top_colors[i].g = 0;
			display.top_colors[i].r = 63;
			display.top_colors[i].b = 0;

			display.bottom_colors[i].g = 31;
			display.bottom_colors[i].r = 0;
			display.bottom_colors[i].b = 31;
		}
	}


	POV_CHAR top_chars[DISPLAY_NUM_CHARS];
	POV_CHAR bottom_chars[DISPLAY_NUM_CHARS];

	// Convert strings to arrays of POV_CHARs
	for (uint32_t i = 0; i < DISPLAY_NUM_CHARS; i++) {
		if (display.top_string[i] != '\0')
			top_chars[i] = convert_to_pov_char(display.top_string[i]);
		if (display.bottom_string[i] != '\0')
			bottom_chars[i] = convert_to_pov_char(display.bottom_string[i]);
	}

	// Convert POV_CHARs into a series of twelve-wide WS2812B GRB values
	for (uint32_t char_pos = 0; char_pos < DISPLAY_NUM_CHARS; char_pos++) {
		for (uint32_t pixel_x = 0; pixel_x < DISPLAY_CHAR_PIXELS_WIDE; pixel_x++) {
			for (uint32_t pixel_y = 0; pixel_y < DISPLAY_CHAR_PIXELS_HIGH; pixel_y++) {

				uint32_t disp_buf_pos = char_pos * (DISPLAY_CHAR_PIXELS_WIDE + 1) + pixel_x;
				uint32_t char_shift = (pixel_x * (DISPLAY_CHAR_PIXELS_WIDE + 1) + pixel_y);

				display_buffer[disp_buf_pos][pixel_y].g = ((bottom_chars[char_pos] >> char_shift) & 1u) * display.bottom_colors[char_pos].g;
				display_buffer[disp_buf_pos][pixel_y].r = ((bottom_chars[char_pos] >> char_shift) & 1u) * display.bottom_colors[char_pos].r;
				display_buffer[disp_buf_pos][pixel_y].b = ((bottom_chars[char_pos] >> char_shift) & 1u) * display.bottom_colors[char_pos].b;

				display_buffer[disp_buf_pos][pixel_y + DISPLAY_CHAR_PIXELS_HIGH].g = ((top_chars[char_pos] >> char_shift) & 1u) * display.top_colors[char_pos].g;
				display_buffer[disp_buf_pos][pixel_y + DISPLAY_CHAR_PIXELS_HIGH].r = ((top_chars[char_pos] >> char_shift) & 1u) * display.top_colors[char_pos].r;
				display_buffer[disp_buf_pos][pixel_y + DISPLAY_CHAR_PIXELS_HIGH].b = ((top_chars[char_pos] >> char_shift) & 1u) * display.top_colors[char_pos].b;
			}
		}
	}
}

/***************************************************************************//**
 * @brief
 *		Updates the humidity local static variable.
 ******************************************************************************/
void pov_update_humidity(void) {
	humidity = si7021_calculate_humidity();
}

/***************************************************************************//**
 * @brief
 *		Updates the humidity local static variable and updates display.
 *
 ******************************************************************************/
void pov_update_si7021_temp(void) {
	/*
	 * Create temperature  and humidity strings & pad leftover characters
	 * with spaces
	 */
	POV_Display_TypeDef display;
	char top[16];
	float temp = si7021_calculate_temperature();
	uint32_t len = sprintf(top, "Humidity: %.2f%%\n", humidity);

	for (uint32_t i = len; i < DISPLAY_NUM_CHARS; i++) {
		top[i] = ' ';
	}

	char bottom[16];
	len = sprintf(bottom, "Temp: %.1fF", temp);

	for (uint32_t i = len; i < DISPLAY_NUM_CHARS; i++) {
		bottom[i] = ' ';
	}


	/* Write strings to display variable */
	strcpy(display.top_string, top);
	strcpy(display.bottom_string, bottom);


	/* Write colors to display variable */
	GRB_TypeDef top_text_color = { 2, 2, 4 };
	GRB_TypeDef top_num_color = { 1, 1, 6};
	GRB_TypeDef bottom_text_color = { 2, 4, 2 };
	GRB_TypeDef bottom_num_color = { 1, 6, 1 };

	for (uint32_t i = 0; i < 9; i++) {
		display.top_colors[i] = top_text_color;
	}
	for (uint32_t i = 9; i < 16; i++) {
		display.top_colors[i] = top_num_color;
	}
	for (uint32_t i = 0; i < 5; i++) {
		display.bottom_colors[i] = bottom_text_color;
	}
	for (uint32_t i = 5; i < 16; i++) {
		display.bottom_colors[i] = bottom_num_color;
	}

	pov_update_display(display);
}

/***************************************************************************//**
 * @brief
 *		Shows the current pressure and altitude estimate on the display.
 *
 ******************************************************************************/
void pov_update_bmp280(void) {
	POV_Display_TypeDef display;

	char top[17];
	float pressure = bmp280_get_last_pressure_read() / 100;
	uint32_t len = sprintf(top, "Pressure: %.0fhPa", pressure);

	for (uint32_t i = len; i < DISPLAY_NUM_CHARS; i++) {
		top[i] = ' ';
	}

	char bottom[17];
	float altitude = bmp280_get_altitude();
	len = sprintf(bottom, "Altitude: %.1fm", altitude);

	for (uint32_t i = len; i < DISPLAY_NUM_CHARS; i++) {
		bottom[i] = ' ';
	}

	pov_update_display(display);
}

/***************************************************************************//**
 * @brief
 *		Shows the "menu" to select the current display option.
 *
 * @details
 *		True = Display is spinning, displaying current data.
 *		False = display is stopped, "menu" mode.
 *
 * @param[in] new_state
 * 		The state to set the display to.
 *
 ******************************************************************************/
void pov_show_menu(void) {

	// Create an empty array of GRB values
	GRB_TypeDef display[WS2812B_NUM_LEDS];
	for (uint32_t i = 0; i < WS2812B_NUM_LEDS; i++) {
		display[i].g = 0;
		display[i].r = 0;
		display[i].b = 0;
	}

	// Turn one LED on depending on the mode selected
	switch(displaymode) {
	case HelloWorld:
		display[0].g = 8;
		break;
	case TempHumidity:
		display[1].g = 8;
		break;
	case Credits:
		display[2].g = 8;
		break;
	case BatteryLevel:
		display[3].g = 8;
		break;
	case PressureAltitude:
		display[4].g = 8;
		break;
	case Filler6:
		display[5].g = 8;
		break;
	case Filler7:
		display[6].g = 8;
		break;
	case Filler8:
		display[7].g = 8;
		break;
	case Filler9:
		display[8].g = 8;
		break;
	case Filler10:
		display[9].g = 8;
		break;
	case Filler11:
		display[10].g = 8;
		break;
	case Filler12:
		display[11].g = 8;
		break;
	}

	ws2812b_write(display);
}

/***************************************************************************//**
 * @brief
 *		Scrolls through operation modes.
 *
 * @param[in] right
 * 		Determines the direction to scroll through the modes.
 *
 ******************************************************************************/
void pov_change_mode(bool direction) {

	// Scroll to the next mode depending on the previous.
	switch(displaymode) {
	case HelloWorld:
		displaymode = direction ? Filler12 : TempHumidity;
		break;
	case TempHumidity:
		displaymode = direction ? HelloWorld : Credits;
		break;
	case Credits:
		displaymode = direction ? TempHumidity : BatteryLevel;
		break;
	case BatteryLevel:
		displaymode = direction ? Credits : PressureAltitude;
		break;
	case PressureAltitude:
		displaymode = direction ? BatteryLevel : Filler6;
		break;
	case Filler6:
		displaymode = direction ? PressureAltitude : Filler7;
		break;
	case Filler7:
		displaymode = direction ? Filler6 : Filler8;
		break;
	case Filler8:
		displaymode = direction ? Filler7 : Filler9;
		break;
	case Filler9:
		displaymode = direction ? Filler8 : Filler10;
		break;
	case Filler10:
		displaymode = direction ? Filler9 : Filler11;
		break;
	case Filler11:
		displaymode = direction ? Filler10 : Filler12;
		break;
	case Filler12:
		displaymode = direction ? Filler11 : HelloWorld;
		break;
	}

	pov_show_menu();
}

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
		if (current_position == dead_one) {
			pov_start_display();
		}
		else if (current_position == display) {
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
	uint32_t int_flag = WTIMER0->IF & WTIMER0->IEN;
	WTIMER0->IFC = int_flag;

	if (int_flag & TIMER_IF_OF) {
		pov_show_menu();
		timer_measure_restart(POV_MEASURE_TIMER);
	}
}
