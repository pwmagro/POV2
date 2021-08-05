/**
 * @file battery.c
 * @author Peter Magro
 * @date August 4th, 2021
 * @brief Handles all battery measurement-related functions.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "battery.h"
#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define fmin(x, y)	((x < y) ? x : y)

//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static uint32_t consecutive_low_reads;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *		Opens the ADC and LETIMER for battery readings.
 *
 * @details
 *		ADC is opened with 8x oversampling. LETIMER is opened with 5-second poll period
 *		and no callbacks.
 *
 ******************************************************************************/
void battery_open(void) {
	// open ADC
	ADC_OPEN_STRUCT_TypeDef adc_init;

	adc_init.em2ClockConfig = adcEm2Disabled;	// only enables async clock when necessary
	adc_init.ovsRateSel = adcOvsRateSel8;			// pretty arbitrary selection here. we don't need a lot of precision.
	adc_init.tailgate = false;						// tailgating is only necessary when the ADC is running continuously. we're going to poll it with the LETIMER
	adc_init.target_freq = ADC_TARGET_FREQ;
	adc_init.warmUpMode = adcWarmupNormal;			// shut down ADC when not in use
	adc_init.channel = ADC_INPUT_BUS;
	adc_init.overwrite = true;						// overwrite FIFO buffer when full
	adc_init.acqTime = adcAcqTime8;

	adc_open(BATTERY_ADC, &adc_init);

	// open LETIMER to poll ADC
	APP_LETIMER_PWM_TypeDef letimer_init;

	letimer_init.active_period = BATTERY_POLLING_PERIOD / 2;
	letimer_init.comp0_cb = 0;						// not using callbacks
	letimer_init.comp0_irq_enable = false;
	letimer_init.comp1_cb = 0;
	letimer_init.comp1_irq_enable = false;
	letimer_init.debugRun = false;
	letimer_init.enable = false;
	letimer_init.out_pin_0_en = false;
	letimer_init.out_pin_1_en = false;
	letimer_init.out_pin_route0 = PWM_ROUTE_0;
	letimer_init.out_pin_route1 = PWM_ROUTE_1;
	letimer_init.period = BATTERY_POLLING_PERIOD;	// 5 seconds
	letimer_init.uf_cb = 0;
	letimer_init.uf_irq_enable = true;

	letimer_pwm_open(BATTERY_LETIMER, &letimer_init);
}

/***************************************************************************//**
 * @brief
 *		Starts a new conversion, reads the last conversion, and updates
 *		consecutive_low_reads.
 *
 * @details
 *		consecutive_low_reads is incremented for each low read, and reset to 0
 *		for any high read.
 *
 ******************************************************************************/
void battery_poll() {
	adc_start_conversion(BATTERY_ADC);

	uint32_t last_read = adc_get_last_read();

	if (last_read < BATTERY_LOW_THRESH) {
		consecutive_low_reads++;
	} else {
		consecutive_low_reads = 0;
	}
}

/***************************************************************************//**
 * @brief
 *		Returns the battery's "low" state.
 *
 * @details
 *		The battery is considered "low" if consecutive_low_reads is greater than
 *		5.
 *
 * @return
 *		True if the battery level is considered "low".
 *
 ******************************************************************************/
bool battery_check_low() {
	if (consecutive_low_reads > BATTERY_LOW_COUNT_THRESH) {
		return true;
	} else {
		return false;
	}
}

/***************************************************************************//**
 * @brief
 *		Calculates the battery level.
 *
 * @details
 *		Extremely rough estimate - simply a linear percent value between the
 *		battery's dead voltage and full charge.
 *
 * @return
 *		A percent value
 *
 ******************************************************************************/
float battery_get_percent() {

	// Calculate the battery percentage
	float battery_max = BATTERY_MAX_V * 4095;
	float battery_min = BATTERY_MIN_V * 4095;
	float percent = 100 * ((float) adc_get_last_read() - (battery_min)) / (battery_max - battery_min);

	// Return 100 if the battery is reading >100, else return the percentage
	return fmin(percent, 100);
}

/***************************************************************************//**
 * @brief
 *		LETIMER interrupt handler.
 *
 * @details
 *		Begins the battery reading sequence.
 *
 ******************************************************************************/
void LETIMER0_IRQHandler(void) {
	uint32_t int_flag = LETIMER0->IF & LETIMER0->IEN;
	LETIMER0->IFC = int_flag;

	if (int_flag & LETIMER_IF_UF) {
		battery_poll();
	}
}
