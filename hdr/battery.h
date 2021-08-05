#ifndef BATTERY_HG
#define BATTERY_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "letimer.h"
#include "adc.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define	BATTERY_ADC					ADC0
#define	BATTERY_LETIMER				LETIMER0
#define BATTERY_POLLING_PERIOD		5
#define BATTERY_LOW_THRESH			4095 * (3.2 / 5.0)
#define BATTERY_LOW_COUNT_THRESH	5
#define BATTERY_MAX_V				4.2
#define BATTERY_MIN_V				3.0

//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// function prototypes
//***********************************************************************************
void battery_open(void);
bool battery_check_low();
float battery_get_percent();

#endif
