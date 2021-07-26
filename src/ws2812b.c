/**
 * @file ws2812b.c
 * @author Peter Magro
 * @date June 10th, 2021
 * @brief Implements the WS2812B RGB LED protocol.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "ws2812b.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static uint8_t txbuffer[WS2812B_BUFFER_LEN];

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *		Opens the WS2812B driver.
 *
 * @details
 *		Enables USART and DMADRV.
 *
 ******************************************************************************/
void ws2812b_open() {
	USART_BITBANG_OPEN_TypeDef open_struct = {
			usartEnableTx,
			0,
			WS2812B_BAUD_RATE,
			WS2812B_DATABITS,
			usartClockMode0,
			true,
			WS2812B_TX_ROUTE,
			true
		};
	usart_bitbang_open(WS2812B_USART, &open_struct);
	DMADRV_Init();
}

/***************************************************************************//**
 * @brief
 *		Displays GRB data on LEDs.
 *
 * @details
 *		Converts GRB data to bits to be sent to USART through DMA.
 *
 * @note
 *		DMA and the USART are being used in conjunction to send a high-frequency
 *		PWM signal. Each location in txbuffer[] holds one byte to be sent to the
 *		USART, each representing one bit in the WS2812B protocol.
 *
 * @param[in] values
 * 		An array containing GRB data for each LED available.
 *
 ******************************************************************************/
void ws2812b_write(const GRB_TypeDef values[WS2812B_NUM_LEDS]) {

	sleep_block_mode(USART_SLEEP_BLOCK_MODE);
	timer_delay(1);

	// converts each bit of each GRB value to a byte to send to SPI
	for (int i = WS2812B_NUM_LEDS - 1; i >= 0; i--) {
		uint32_t j = i * 24;

		txbuffer[j + 23] = ((values[i].b)      & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 22] = ((values[i].b >> 1) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 21] = ((values[i].b >> 2) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 20] = ((values[i].b >> 3) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 19] = ((values[i].b >> 4) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 18] = ((values[i].b >> 5) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 17] = ((values[i].b >> 6) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 16] = ((values[i].b >> 7) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 15] = ((values[i].r)      & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 14] = ((values[i].r >> 1) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 13] = ((values[i].r >> 2) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 12] = ((values[i].r >> 3) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 11] = ((values[i].r >> 4) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 10] = ((values[i].r >> 5) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 9]  = ((values[i].r >> 6) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 8]  = ((values[i].r >> 7) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 7]  = ((values[i].g)      & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 6]  = ((values[i].g >> 1) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 5]  = ((values[i].g >> 2) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 4]  = ((values[i].g >> 3) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 3]  = ((values[i].g >> 4) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 2]  = ((values[i].g >> 5) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j + 1]  = ((values[i].g >> 6) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
		txbuffer[j]      = ((values[i].g >> 7) & 1u) ? WS2812B_ONE : WS2812B_ZERO;
	}

	unsigned int channel;
	EFM_ASSERT(DMADRV_AllocateChannel(&channel, NULL) == ECODE_EMDRV_DMADRV_OK);	// allocate a DMA channel and ensure it was successful

	DMADRV_MemoryPeripheral(
								WS2812B_DMA_CHANNEL,
								WS2812B_DMA_PERIPHERAL_SIGNAL,
								(void*)&USART2->TXDATA,
								txbuffer,
								true,
								WS2812B_BUFFER_LEN,
								dmadrvDataSize1,
								NULL,
								NULL
			);

	timer_delay(1);

	DMADRV_FreeChannel(WS2812B_DMA_CHANNEL);											// free the DMA channel
	sleep_unblock_mode(USART_SLEEP_BLOCK_MODE);
}
