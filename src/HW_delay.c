//***********************************************************************************
// Include files
//***********************************************************************************

//** Silicon Labs Include Files

//** User Include Files
#include "HW_delay.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************


//***********************************************************************************
// Private functions Prototypes
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

void timer_delay(uint32_t ms_delay){

	uint32_t old_ctrl = TIMER0->CTRL;
	uint32_t old_ien = TIMER0->IEN;
	uint32_t old_top = TIMER0->TOP;
	uint32_t old_cc0_ctrl = TIMER0->CC[0].CTRL;
	uint32_t old_cc0_ccv = TIMER0->CC[0].CCV;

	uint32_t timer_clk_freq = CMU_ClockFreqGet(cmuClock_HFPER);
	uint32_t delay_count = ms_delay *(timer_clk_freq/1000) / 1024;
	TIMER_Init_TypeDef delay_counter_init = TIMER_INIT_DEFAULT;
		delay_counter_init.oneShot = true;
		delay_counter_init.enable = false;
		delay_counter_init.mode = timerModeDown;
		delay_counter_init.prescale = timerPrescale1024;
		delay_counter_init.debugRun = false;
	TIMER_Init(TIMER0, &delay_counter_init);
	TIMER0->CNT = delay_count;
	TIMER_Enable(TIMER0, true);
	while (TIMER0->CNT != 00);
	TIMER_Enable(TIMER0, false);

	TIMER0->CTRL = old_ctrl;
	TIMER0->IEN = old_ien;
	TIMER0->TOP = old_top;
	TIMER0->CC[0].CTRL = old_cc0_ctrl;
	TIMER0->CC[0].CCV = old_cc0_ccv;

	TIMER_Enable(TIMER0, true);
}

