#ifndef APP_HG
#define APP_HG

//***********************************************************************************
// Include files
//***********************************************************************************
// SiLabs files
#include "em_cmu.h"
#include "em_assert.h"
#include "em_core.h"

// User files
#include "adc.h"
#include "brd_config.h"
#include "battery.h"
#include "bmp280.h"
#include "cmu.h"
#include "gpio.h"
#include "HW_delay.h"
#include "i2c.h"
#include "letimer.h"
#include "timer.h"
#include "pov.h"
#include "scheduler.h"
#include "sleep_routines.h"
#include "usart.h"
#include "ws2812b.h"
#include "font.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define		SYSTEM_BLOCK_EM		EM3

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void run_scheduled_events(uint32_t scheduled_events);

#endif
