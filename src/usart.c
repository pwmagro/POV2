/**
 * @file usart.c
 * @author Peter Magro
 * @date June 17th, 2021
 * @brief Enables the USART for SPI operation.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "usart.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static volatile bool busy;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *		Opens the USART for synchronous (SPI) operation.
 *
 * @details
 *		Enables clock to USART, configures USART, and sets pin locations defined
 *		in brd_config.h.
 *
 * @note
 *		Intended to be controlled through DMA.
 *
 * @param[in] *usart
 * 		The address of the USART to open. For this purpose, only USART2 is valid.
 *
 * @param[in] *usart_open_struct
 * 		A struct containing settings with which to open the USART.
 *
 ******************************************************************************/
void usart_bitbang_open(USART_TypeDef *usart, USART_BITBANG_OPEN_TypeDef *usart_open_struct) {

	EFM_ASSERT(usart == USART2); // Only USART2 is capable of SPI operation on EFR32xG12 chips

	CMU_ClockEnable(cmuClock_USART2, true);

	USART_InitSync_TypeDef initSync;
	initSync.enable = false;
	initSync.refFreq = usart_open_struct->refFreq;
	initSync.baudrate = usart_open_struct->baudrate;
	initSync.databits = usart_open_struct->databits;
	initSync.master = true;
	initSync.msbf = true;
	initSync.clockMode = usart_open_struct->clockMode;
	initSync.prsRxEnable = false;						// TX only
	initSync.prsRxCh = usartPrsRxCh0;					// null
	initSync.autoTx = usart_open_struct->autoTx;
	initSync.autoCsEnable = true;
	initSync.autoCsHold = 0;
	initSync.autoCsSetup = 0;

	USART_InitSync(usart, &initSync);

	usart->ROUTELOC0 = usart_open_struct->tx_loc;
	usart->ROUTEPEN = usart_open_struct->tx_pin_en * USART_ROUTEPEN_TXPEN;

	usart->CMD = USART_CMD_CLEARTX | USART_CMD_CLEARRX;

	usart->IEN = 0x00;
	__NVIC_EnableIRQ(USART2_TX_IRQn);

	USART_Enable(usart, usartEnable);
}

/***************************************************************************//**
 * @brief
 *		Interrupt handler for USART2.
 *
 * @details
 *		Lowers busy flag and disables TXBL interrupts.
 *
 ******************************************************************************/
void USART2_TX_IRQHandler(void) {
	volatile uint32_t int_flag = USART2->IF;
	int_flag &= USART2->IEN;
	USART2->IFC = int_flag;

	if (int_flag & USART_IF_TXBL) {
		busy = false;
		USART2->IEN &= ~USART_IEN_TXBL;
	}
}
