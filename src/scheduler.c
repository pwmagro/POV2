/**
 * @file scheduler.c
 * @author Peter Magro
 * @date June 21st, 2021
 * @brief Contains all scheduler functions.
 */

/*
 * includes
 */
#include "scheduler.h"

#include "em_core.h"

/*
 * defines
 */
#define CLEAR_EVENTS		0x00


/*
 * private variables
 */
static unsigned int event_scheduled;

/***************************************************************************//**
 * @brief
 *		Prepares the scheduler to be used.
 *
 * @details
 *		Opens the scheduler functionality by resetting the static/private variable event_scheduled to 0.
 *
 * @note
 *		This function is atomic.
 *
 ******************************************************************************/
void scheduler_open(void) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	event_scheduled = CLEAR_EVENTS;

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *		Adds a new event to the scheduler.
 *
 * @details
 *		ORs a new event, the input argument, with the existing state of the static variable event_scheduled.
 *
 * @note
 *		This function is atomic.
 *
 * @param[in] event
 * 		The event to add to the scheduler.
 *
 ******************************************************************************/
void add_scheduled_event(uint32_t event) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	event_scheduled |= event;

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *		Removes an event from the scheduler.
 *
 * @details
 *		Inverts then ANDs an event, the input argument, with the existing state of the static variable event_scheduled
 *
 * @note
 *		This function is atomic.
 *
 * @param[in] event
 * 		The event to remove from the scheduler.
 *
 ******************************************************************************/
void remove_scheduled_event(uint32_t event) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	event_scheduled &= ~event;

	CORE_EXIT_CRITICAL();
}

// Returns the current state of the static variable event_scheduled
/***************************************************************************//**
 * @brief
 *		Returns the event_scheduled variable.
 *
 ******************************************************************************/
uint32_t get_scheduled_events(void) {
	return event_scheduled;
}
