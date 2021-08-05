#ifndef WS2812B_HG
#define WS2812B_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// WS2812B protocol note
//***********************************************************************************
/*
 *	The WS2812 uses an unconventional protocol, where ones and zeroes are determined
 *	by PWM duty cycle rather than clocked highs and lows. Since the ratios are simple
 *	2:1 and 1:2 @ 800kHz, we can achieve this with 3 SPI bits @ 2.4MHz:
 *
 *	Clock
 *		.   .   .   .   .   .   .
 *	Output
 *		________	 ___
 *				|	|	|
 *				|___|	|_______
 *	WS2812B
 *		ONE			 ZERO
 *	SPI
 *		  1   1   0   1   0   0
 */
// 1000 0110
// 1011 1110

#define WS2812B_ONE			0xFC	// 0b 1111 1100
#define WS2812B_ZERO		0xC0	// 0b 1100 0000
#define WS2812B_BUFFER_LEN	(WS2812B_NUM_LEDS * 24) // 24 bits per LED

// USART settings
#define WS2812B_BAUD_RATE	6400000u
#define WS2812B_DATABITS	usartDatabits8
#define	WS2812B_TX_ROUTE	USART_ROUTELOC0_TXLOC_LOC29

// DMA settings
#define WS2812B_DMA_CHANNEL				0u
#define WS2812B_DMA_PERIPHERAL_SIGNAL	dmadrvPeripheralSignal_USART2_TXBL

typedef struct {
	char g;
	char r;
	char b;
} GRB_TypeDef;

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void ws2812b_open();
void ws2812b_write(const GRB_TypeDef values[WS2812B_NUM_LEDS]);

#endif
