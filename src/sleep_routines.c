/****************************************************************//**
 * @file sleep_routines.c
 * @author Peter Magro
 * @date May 28th, 2021
 * @brief Contains all the sleep mode handling functions.
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 * **************************************************************
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 */


//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files
#include "em_emu.h"
#include "em_assert.h"
#include "em_core.h"

//** User/developer include files
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static int lowest_energy_mode[MAX_ENERGY_MODES];

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		Unblocks all energy modes.
 *
 * @details
 *		Sets every value of the lowest_energy_mode array to 0.
 *
 * @note
 *		This function is atomic.
 *
 ******************************************************************************/
void sleep_open(void) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	for (int i = 0; i < MAX_ENERGY_MODES; i++) {
		lowest_energy_mode[i] = 0;
	}

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *		Prevents CPU from entering energy mode EM or lower.
 *
 * @details
 *		Increments lowest_energy_mode[EM], and crashes the app if there are more than 4 blocks on energy mode EM.
 *
 * @note
 *		This function is atomic. If this function is called multiple times on a single energy mode, sleep_unblock_mode(EM) must also be called multiple times.
 *
 * @param[in] EM
 * 		The energy mode to block.
 *
 ******************************************************************************/
void sleep_block_mode(uint32_t EM) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	lowest_energy_mode[EM]++;
	EFM_ASSERT(lowest_energy_mode[EM] < 5);

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *		Removes a block on CPU entering energy mode EM.
 *
 * @details
 *		Decrements lowest_energy_mode[EM], and crashes the app if there are a negative number of blocks on energy mode EM.
 *
 * @note
 *		This function is atomic.
 *
 * @param[in] EM
 * 		The energy mode to unblock.
 *
 ******************************************************************************/
void sleep_unblock_mode(uint32_t EM) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	lowest_energy_mode[EM]--;
	EFM_ASSERT(lowest_energy_mode[EM] >= 0);

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *		Enters the lowest allowed energy mode.
 *
 * @details
 *		Steps from high to low energy modes until a block is encountered.
 *
 * @note
 *		This function is atomic. This function will never enter EM4 -- EM3 is the lowest allowed mode.
 *
 ******************************************************************************/
void enter_sleep(void) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	if (lowest_energy_mode[EM0] > 0) {
		CORE_EXIT_CRITICAL();
		return;
	}
	else if (lowest_energy_mode[EM1] > 0) {
		CORE_EXIT_CRITICAL();
		return;
	}
	else if (lowest_energy_mode[EM2] > 0) {
		EMU_EnterEM1();
		CORE_EXIT_CRITICAL();
		return;
	}
	else if (lowest_energy_mode[EM3] > 0) {
		EMU_EnterEM2(true);
		CORE_EXIT_CRITICAL();
		return;
	}
	else {
		EMU_EnterEM3(true);
		CORE_EXIT_CRITICAL();
		return;
	}
}

/***************************************************************************//**
 * @brief
 *		Returns the highest blocked energy mode.
 *
 * @details
 *		Iterates through lowest_energy_mode[] until a block is encountered. Returns lowest energy mode if no blocks are found.
 *
 *
 ******************************************************************************/
uint32_t current_block_energy_mode(void) {
	for (int i = 0; i < MAX_ENERGY_MODES; i++) {
		if (lowest_energy_mode[i] != 0) {
			return i;
		}
	}
	return (MAX_ENERGY_MODES - 1);
}



