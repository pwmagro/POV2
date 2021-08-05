/**
 * @file timer.c
 * @author Peter Magro
 * @date June 25th, 2021
 * @brief Enables and configures the Timer module.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "timer.h"

#include "em_cmu.h"
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *		Opens the TIMER module.
 *
 * @param[in] *timer
 *		The address of the timer module to open.
 *
 * @param[in] *open_struct
 * 		Struct containing parameters with which to open the timer.
 *
 ******************************************************************************/
void timer_open(TIMER_TypeDef *timer, TIMER_MEASURE_TypeDef *open_struct) {

	TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;

	if (timer == TIMER0) {
		CMU_ClockEnable(cmuClock_TIMER0, true);
	} else if (timer == TIMER1) {
		CMU_ClockEnable(cmuClock_TIMER1, true);
	} else if (timer == WTIMER0){
		CMU_ClockEnable(cmuClock_WTIMER0, true);
	} else if (timer == WTIMER1) {
		CMU_ClockEnable(cmuClock_WTIMER1, true);
	} else {
		EFM_ASSERT(false);
	}

	sleep_block_mode(TIMER_EM);
	timer->IEN = 0x00;

	timer->CMD = TIMER_CMD_START;
	while(!(timer->STATUS & TIMER_STATUS_RUNNING));			// we'll get stuck here if the timer doesn't start
	timer->CMD = TIMER_CMD_STOP;
	while(timer->STATUS & TIMER_STATUS_RUNNING);			// we'll get stuck here if the timer doesn't stop

	init.enable = open_struct->enable;
	init.debugRun = open_struct->debugRun;
	init.prescale = open_struct->prescale;
	init.clkSel = open_struct->clkSel;
	init.riseAction = open_struct->riseAction;
	init.fallAction = open_struct->fallAction;
	init.oneShot = open_struct->oneShot;

	TIMER_Init(timer, &init);

	timer->CC[0].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;

	timer->LOCK = TIMER_LOCK_TIMERLOCKKEY_UNLOCK;			// unlock timer registers

	if (timer == TIMER0) {
		NVIC_EnableIRQ(TIMER0_IRQn);
	} else if (timer == TIMER1) {
		NVIC_EnableIRQ(TIMER1_IRQn);
	} else if (timer == WTIMER0) {
		NVIC_EnableIRQ(WTIMER0_IRQn);
	} else if (timer == WTIMER1) {
		NVIC_EnableIRQ(WTIMER1_IRQn);
	}
}

/***************************************************************************//**
 * @brief
 *		Resets the timer and returns the timer->CNT value at time of reset.
 *
 * @param[in] *timer
 *		The address of the timer to reset.
 *
 ******************************************************************************/
uint32_t timer_measure_restart(TIMER_TypeDef *timer) {
	EFM_ASSERT(timer == TIMER0
			|| timer == TIMER1
			|| timer == WTIMER0
			|| timer == WTIMER1);
	timer_stop(timer);
	uint32_t out = timer->CNT;
	timer_start(timer, timer->TOP, timer->CC[0].CCV);
	return out;
}

/***************************************************************************//**
 * @brief
 *		Begins a timer one-shot.
 *
 * @details
 *		Sets timer's TOP and CC[0] values. Enables interrupts from CC[0] and overflow.
 *
 * @param[in] *timer
 *		The address of the timer to start.
 *
 * @param[in] ticks
 * 		The TOP value of the timer.
 *
 * @param[in] compare_reg
 * 		The value to set CC[0] to. When the timer's CNT register reaches this value,
 * 		an interrupt will be generated.
 *
 ******************************************************************************/
void timer_start(TIMER_TypeDef *timer, uint32_t ticks, uint32_t compare_reg) {
	EFM_ASSERT(timer == TIMER0
			|| timer == TIMER1
			|| timer == WTIMER0
			|| timer == WTIMER1);
	timer->CMD = TIMER_CMD_STOP;
	timer->CNT = 0;
	timer->TOP = ticks;
	timer->CC[0].CCV = compare_reg;
	timer->IEN |= TIMER_IEN_CC0 | TIMER_IEN_OF;
	timer->CMD = TIMER_CMD_START;
}

/***************************************************************************//**
 * @brief
 *		Stops a timer.
 *
 * @details
 *		Disables interrupts from the timer, then sends a STOP command.
 *
 * @param[in] *timer
 *		The address of the timer to stop.
 *
 ******************************************************************************/
void timer_stop(TIMER_TypeDef *timer) {
	EFM_ASSERT(timer == TIMER0
			|| timer == TIMER1
			|| timer == WTIMER0
			|| timer == WTIMER1);
	timer->IEN &= ~(TIMER_IEN_CC0 | TIMER_IEN_OF);
	timer->CMD = TIMER_CMD_STOP;
}
