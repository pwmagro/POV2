/**
 * @file adc.c
 * @author Peter Magro
 * @date August 4th, 2021
 * @brief Includes functions related to the ADC.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "adc.h"
#include "em_cmu.h"
#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static uint32_t last_read;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		Initializes the ADC.
 *
 * @details
 *		Opens the ADC using caller-specified settings, and enables interrupts
 *		for single reads.
 *
 * @note
 *   	The channel used for single reads is specified in brd_config.h.
 *
 * @param[in] *adc
 * 		The address of the ADC.
 *
 * @param[in] *open_struct
 * 		The settings with which to open the ADC.
 *
 ******************************************************************************/
void adc_open(ADC_TypeDef *adc, ADC_OPEN_STRUCT_TypeDef *open_struct) {

	// Initial setup
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	EFM_ASSERT(adc == ADC0);
	sleep_block_mode(ADC_EM);
	CMU_ClockEnable(cmuClock_ADC0, true);

	// Opening the ADC
	init.ovsRateSel = open_struct->ovsRateSel;
	init.warmUpMode = open_struct->warmUpMode;
	init.timebase = ADC_TimebaseCalc(MCU_HFRCO_FREQ);
	init.prescale = ADC_PrescaleCalc(open_struct->target_freq, MCU_HFRCO_FREQ);
	init.tailgate = open_struct->tailgate;
	init.em2ClockConfig = open_struct->em2ClockConfig;
	ADC_Init(adc, &init);

	// Programming the ADC's single channel mode
	ADC_InitSingle_TypeDef init_single = ADC_INITSINGLE_DEFAULT;
	init_single.reference = adcRef5V;
	init_single.posSel = open_struct->channel;
	init_single.fifoOverwrite = open_struct->overwrite;
	init_single.acqTime = open_struct->acqTime;
	ADC_InitSingle(adc, &init_single);

	uint32_t ien_flags = ADC_IEN_SINGLE;
	adc->IEN = ien_flags;

	// Cleanup
	NVIC_EnableIRQ(ADC0_IRQn);
	sleep_unblock_mode(ADC_EM);
}

/***************************************************************************//**
 * @brief
 *		Starts a single read of the active channel.
 *
 * @param[in] *adc
 *		The address of the ADC peripheral.
 *
 ******************************************************************************/
void adc_start_conversion(ADC_TypeDef *adc) {
	sleep_block_mode(ADC_EM);
	adc->CMD = ADC_CMD_SINGLESTART;
}

/***************************************************************************//**
 * @brief
 *		Returns the most recent reading from the ADC.
 *
 ******************************************************************************/
uint32_t adc_get_last_read() {
	return last_read;
}

/***************************************************************************//**
 * @brief
 *		Interrupt handler for the ADC.
 *
 * @details
 *		Called when the ADC finishes a conversion; updates last_read.
 *
 ******************************************************************************/
void ADC0_IRQHandler(void) {
	uint32_t int_flag = ADC0->IF & ADC0->IEN;
	ADC0->IFC = int_flag;

	if (int_flag & ADC_IF_SINGLE) {
		last_read = ADC0->SINGLEDATA;
		sleep_unblock_mode(ADC_EM);
	}
}
