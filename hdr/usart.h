#ifndef USART_HG
#define USART_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_usart.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_dma.h"

#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define USART_SLEEP_BLOCK_MODE		EM2

typedef struct {
	USART_Enable_TypeDef enable;
	uint32_t refFreq;
	uint32_t baudrate;
	USART_Databits_TypeDef databits;
	USART_ClockMode_TypeDef clockMode;
	bool autoTx;
	uint32_t tx_loc;
	bool tx_pin_en;
} USART_BITBANG_OPEN_TypeDef;


typedef struct {
	USART_TypeDef *usart;
	char *tx_str;
	uint32_t str_length;
} SPI_BITBANG_PAYLOAD_TypeDef;

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void usart_bitbang_open(USART_TypeDef *usart, USART_BITBANG_OPEN_TypeDef *usart_open_struct);

#endif
