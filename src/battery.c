/**
 * @file battery.c
 * @author Peter Magro
 * @date June 22nd, 2021
 * @brief Initializes the ADC and LETIMER for battery measurements.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "battery.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static uint32_t consecutive_low_reads;
static uint32_t letimer_callback;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
void battery_open(uint32_t cb) {
	letimer_callback = cb;
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
	letimer_init.comp0_cb = 0;						// not using
	letimer_init.comp0_irq_enable = false;
	letimer_init.comp1_cb = 0;
	letimer_init.comp1_irq_enable = false;
	letimer_init.debugRun = false;
	letimer_init.enable = false;
	letimer_init.out_pin_0_en = false;
	letimer_init.out_pin_1_en = false;
	letimer_init.out_pin_route0 = PWM_ROUTE_0;
	letimer_init.out_pin_route1 = PWM_ROUTE_1;
	letimer_init.period = BATTERY_POLLING_PERIOD;
	letimer_init.uf_cb = cb;
	letimer_init.uf_irq_enable = true;

	letimer_pwm_open(BATTERY_LETIMER, &letimer_init);
}

void battery_poll() {
	adc_start_conversion(BATTERY_ADC);

	uint32_t last_read = adc_get_last_read();

	if (last_read < BATTERY_LOW_THRESH) {
		consecutive_low_reads++;
	} else {
		consecutive_low_reads = 0;
	}
}

bool battery_check_low() {
	if (consecutive_low_reads > BATTERY_LOW_COUNT_THRESH) {
		return true;
	} else {
		return false;
	}
}

float battery_get_percent() {
	return 100 * ((float) adc_get_last_read()) / 4095;
}

void LETIMER0_IRQHandler(void) {
	uint32_t int_flag = LETIMER0->IF & LETIMER0->IEN;
	LETIMER0->IFC = int_flag;

	if (int_flag & LETIMER_IF_UF) {
		add_scheduled_event(letimer_callback);
	}
}
