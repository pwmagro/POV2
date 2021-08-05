#ifndef ADC_HG
#define ADC_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>

#include "em_adc.h"

#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define	ADC_EM				EM2

typedef struct {
	ADC_OvsRateSel_TypeDef 		ovsRateSel;
	ADC_Warmup_TypeDef 			warmUpMode;
	uint32_t					target_freq;
	bool						tailgate;
	ADC_EM2ClockConfig_TypeDef	em2ClockConfig;
	ADC_PosSel_TypeDef			channel;
	bool						overwrite;
	ADC_AcqTime_TypeDef			acqTime;
} ADC_OPEN_STRUCT_TypeDef;

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void adc_open(ADC_TypeDef *adc, ADC_OPEN_STRUCT_TypeDef *open_struct);
void adc_start_conversion(ADC_TypeDef *adc);
uint32_t adc_get_last_read();

#endif
