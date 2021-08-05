/**
 * @file cmu.c
 * @author Peter Magro
 * @date June 21st, 2021
 * @brief cmu.c enables oscillators and configures clock tree.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

#include "em_assert.h"

#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Enable and configure oscillators and route to clock.
 *
 * @details
 * 	 This function enables the HFPER and CORELE clocks, enables the LFRCO and LFXO oscillators,
 * 	 and routes the ULFRCO to the LFA clock tree.
 *
 * @note
 *
 *
 ******************************************************************************/
void cmu_open(void){

		CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, Low Frequency Resistor Capacitor Oscillator, LFRCO, is enabled,
		// Disable the LFRCO oscillator
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// What is the enumeration required for LFRCO?

		// Enable the Low Frequency Crystal Oscillator, LFXO, and route to LFB clock tree
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

		// Route LF clock to LETIMER0 clock tree
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// What clock tree does the LETIMER0 reside on?

		// Now, you must ensure that the global Low Frequency is enabled
		CMU_ClockEnable(cmuClock_CORELE, true);	//This enumeration is found in the Lab 2 assignment

		// Switch HFCLK to HFRCO and disable HFXO
		CMU_HFRCOBandSet(MCU_HFRCO_FREQ); 					// define in brd_config.h
		CMU_OscillatorEnable(cmuOsc_HFRCO, true, true); 	// enable HF RC Osc
		CMU_OscillatorEnable(cmuOsc_HFXO, true, true);		// enable HF crystal osc
		CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);	// Route HF X Osc to HF clock tree

}

