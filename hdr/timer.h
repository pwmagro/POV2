//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	TIMER_HG
#define	TIMER_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_timer.h"
#include "em_cmu.h"
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define TIMER_2X_COUNT_MODE			false
#define	TIMER_ATI_ENABLE			false
#define	TIMER_MODE					timerModeUp

#define	TIMER_EM					EM3

typedef struct {
	bool enable;
	bool debugRun;
	TIMER_Prescale_TypeDef prescale;
	TIMER_ClkSel_TypeDef clkSel;
	TIMER_InputAction_TypeDef fallAction;
	TIMER_InputAction_TypeDef riseAction;
	bool oneShot;
} TIMER_MEASURE_TypeDef;

//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// function prototypes
//***********************************************************************************
void timer_open(TIMER_TypeDef *timer, TIMER_MEASURE_TypeDef *open_struct);
uint32_t timer_measure_restart(TIMER_TypeDef *timer);
void timer_start(TIMER_TypeDef *timer, uint32_t ticks, uint32_t capture_reg);
void timer_stop(TIMER_TypeDef *timer);

#endif
