/**
 * @file i2c.c
 * @author Peter Magro
 * @date July 22nd, 2021
 * @brief i2c.c contains all functions for the I2C peripheral.
 * @note This file replaces traditional "master/slave" terminology with "controller/receiver", however, the Silicon Labs
 * 			platform still uses the older terminology, so it is still present in some of the code.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c.h"

/* System include statements */
#include <stdio.h>

/* Silicon Labs include statements */

/* The developer's include statements */
#include "sleep_routines.h"
#include "scheduler.h"
#include "HW_Delay.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************

static I2C_STATE_MACHINE state_machine_0;
static I2C_STATE_MACHINE state_machine_1;


//***********************************************************************************
// Private functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		Handles ACKs sent by either the controller or receiver.
 *
 * @details
 *		Valid states are:
 *			- Check_If_Device_Available: Will advance i2c_sm's state to Device_Available_Send_Read or Device_Available_Send_Write (depending on the i2c_sm mode) and send the command stored in i2c_sm.
 *			- Device_Available_Send_Read: Will send a start command, read the address stored in i2c_sm, and advance i2c_sm's state to Check_If_Conversion_Ready.
 *			- Device_Available_Send_Write: Will advance i2c_sm's state to Write_First_Byte, and send the least significant byte.
 *			- Check_If_Conversion_Ready: Will advance i2c_sm's state to Read_Data.
 *			- Write_First_Byte: If i2c_sm is in W_Single_Byte mode, will advance state machine to Write_Complete and send a stop command. Otherwise, it will advance to Write_Second_Byte and transmit the MSB.
 *			- Write_Second_Byte: Advances i2c_sm's state to Write_Complete and sends a stop command.
 *			- First_Byte_Received: This ACK was sent by the controller. Will do nothing.
 *
 * @note
 *		Will stall if an invalid state is sent.
 *
 * @param[in] *i2c_sm
 * 		The I2C state machine which currently controls the I2C peripheral.
 *
 ******************************************************************************/
static inline void service_ack(I2C_STATE_MACHINE *i2c_sm) {
	switch(i2c_sm->state) {

	// Valid
	case Check_If_Device_Available:
		i2c_sm->state = Set_Register;
		i2c_sm->i2c->TXDATA = i2c_sm->receiver_register;
		break;
	case Set_Register:
		if ((i2c_sm->mode == R_MSB_First) || (i2c_sm->mode == R_LSB_First)) {
			*(i2c_sm->i2c_data) = 0;
			i2c_sm->state = Send_Read_Command;
			i2c_sm->i2c->CMD = I2C_CMD_START;
			i2c_sm->i2c->TXDATA = (i2c_sm->receiver_address << 1) | READ_BIT;
		} else {
			i2c_sm->state = Write_Data;
			i2c_sm->i2c->TXDATA = *(i2c_sm->i2c_data);
		}
		break;
	case Send_Read_Command:
		i2c_sm->state = Receive_Data;
		break;
	case Write_Data:
		i2c_sm->state = Write_Complete;
		i2c_sm->i2c->CMD = I2C_CMD_STOP;
		break;

	// Valid, but does nothing
	case Receive_Data:
		break;

	// Illegal states
	case All_Data_Received:
		EFM_ASSERT(false);
		break;
	case Write_Complete:
		EFM_ASSERT(false);
		break;
	default:
		EFM_ASSERT(false);		// using this instead of EFM_Assert so I can see which state got us here
	}
}

/***************************************************************************//**
 * @brief
 *		Handles NACKs sent by the receiver.
 *
 * @details
 *		Valid states are:
 *			- Check_If_Device_Available: The receiver was busy when the request was sent. Starts the request over.
 *			- Check_If_Conversion_Ready: The receiver has not finished calculations. Attempts another read.
 *
 * @note
 *		Will stall if an invalid state is sent.
 *
 * @param[in] *i2c_sm
 * 		The I2C state machine which currently controls the I2C peripheral.
 *
 ******************************************************************************/
static inline void service_nack(I2C_STATE_MACHINE *i2c_sm) {
	switch(i2c_sm->state) {

	case Send_Read_Command:
		if ((i2c_sm->mode == R_MSB_First) || (i2c_sm->mode == R_LSB_First)) {
			i2c_sm->state = Send_Read_Command;
			i2c_sm->i2c->CMD = I2C_CMD_START;
			i2c_sm->i2c->TXDATA = (i2c_sm->receiver_address << 1) | READ_BIT;
		}
		break;

	// Illegal states
	case Check_If_Device_Available:
		EFM_ASSERT(false);
		break;
	case Set_Register:
		EFM_ASSERT(false);
		break;
	case Receive_Data:
		EFM_ASSERT(false);
		break;
	case All_Data_Received:
		EFM_ASSERT(false);
		break;
	case Write_Complete:
		EFM_ASSERT(false);
		break;
	default:
		EFM_ASSERT(false);									// using this instead of EFM_Assert so I can see which state got us here
	}
}

/***************************************************************************//**
 * @brief
 *		Handles the RXDATAV flag.
 *
 * @details
 *		Valid states are:
 *			- Read_Data: Will copy RXDATA to i2c_sm->i2c_data. If i2c_sm's mode is R_Single_Byte, will advance state to All_Data_Received. Otherwise, will advance state to First_Byte_Received.
 *			- First_Byte_Received: Advances i2c_sm's state to All_Data_Received, copies RXDATA buffer i2c_sm's data field, then sends a NACK and STOP command.
 *
 * @note
 *		Will stall if an invalid state is sent.
 *
 * @param[in] *i2c_sm
 * 		The I2C state machine which currently controls the I2C peripheral.
 *
 ******************************************************************************/
static inline void service_rx_data_valid(I2C_STATE_MACHINE *i2c_sm) {
	switch(i2c_sm->state) {

	case Receive_Data:
		i2c_sm->count--;
		if (i2c_sm->mode == R_MSB_First) {
			*(i2c_sm->i2c_data) |= i2c_sm->i2c->RXDATA << (8 * i2c_sm->count);
		} else {
			*(i2c_sm->i2c_data) |= i2c_sm->i2c->RXDATA << (8 * i2c_sm->i);
		}

		i2c_sm->i++;

		if (i2c_sm->count) {
			i2c_sm->i2c->CMD = I2C_CMD_ACK;
		} else {
			i2c_sm->state = All_Data_Received;
			i2c_sm->i2c->CMD = I2C_CMD_NACK;
			i2c_sm->i2c->CMD = I2C_CMD_STOP;
		}

		break;
	case All_Data_Received:
		break;

	// Illegal states
	case Check_If_Device_Available:
		EFM_ASSERT(false);
		break;
	case Set_Register:
		EFM_ASSERT(false);
		break;
	case Send_Read_Command:
		EFM_ASSERT(false);
		break;
	case Write_Complete:
		EFM_ASSERT(false);
		break;
	default:
		while(1);									// using this instead of EFM_Assert so I can see which state got us here
	}
}

/***************************************************************************//**
 * @brief
 *		Handles MSTOP interrupts.
 *
 * @details
 *		Valid states are:
 *			- All_Data_Received: Advances i2c_sm's state to Read_Complete, lowers its busy flag, unblocks the I2C energy mode, and adds i2c_sm's callback function to the scheduler.
 *			- Write_Complete: Lowers the state machine's busy flag, unblocks the I2C energy mode, and adds i2c_sm's callback function to the scheduler.
 *
 * @note
 *		Will stall if an invalid state is sent.
 *
 * @param[in] *i2c_sm
 * 		The I2C state machine which currently controls the I2C peripheral.
 *
 ******************************************************************************/
static inline void service_mstop(I2C_STATE_MACHINE *i2c_sm) {
	switch(i2c_sm->state) {

	case All_Data_Received:
		add_scheduled_event(i2c_sm->cb);
		sleep_unblock_mode(I2C_EM_BLOCK);
		i2c_sm->busy = false;
		break;
	case Write_Complete:
		add_scheduled_event(i2c_sm->cb);
		sleep_unblock_mode(I2C_EM_BLOCK);
		i2c_sm->busy = false;
		break;

	// Illegal states
	case Check_If_Device_Available:
		break;
	case Set_Register:
		break;
	case Send_Read_Command:
		break;
	case Receive_Data:
		break;
	default:
		while(1);									// using this instead of EFM_Assert so I can see which state got us here
	}
}

/***************************************************************************//**
 * @brief
 *		Resets all communication on the I2C bus.
 *
 * @details
 *		Aborts any transmissions, clears the TX buffer, and resets the bus by sending simultaneous START and STOP commands.
 *
 * @param[in] *i2c_peripheral
 * 		The I2C peripheral in use by the application.
 *
 *
 ******************************************************************************/
static void i2c_bus_reset(I2C_TypeDef *i2c_peripheral) {
	i2c_peripheral->CMD = I2C_CMD_ABORT;			// abort any current transmissions

	// disabling and clearing interrupts
	uint32_t ien = i2c_peripheral->IEN;
	i2c_peripheral->IEN = 0x0;
	i2c_peripheral->IFC = 0xFFFFFFFF;

	// clearing the contents of the peripheral registers and resetting
	i2c_peripheral->CMD = I2C_CMD_CLEARTX;

	i2c_peripheral->IFC |= I2C_IFC_MSTOP;
	i2c_peripheral->CMD |= I2C_CMD_START | I2C_CMD_STOP;

	while(!(i2c_peripheral->IF & I2C_IF_MSTOP));
	i2c_peripheral->IFC |= I2C_IFC_MSTOP;

	i2c_peripheral->CMD = I2C_CMD_CLEARTX;

	// put everybody back where they belong
	i2c_peripheral->IEN = ien;
	i2c_peripheral->CMD = I2C_CMD_ABORT;
}




//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		Configures the I2C peripheral.
 *
 * @details
 *		Enables the appropriate clock, asserts the functionality of the I2C peripheral's interrupt vector, opens the peripheral with the settings passed by the *i2c_settings struct, and enables I2C interrupts. Also sends a bus reset.
 *
 * @param[in] *i2c_peripheral
 * 		The I2C peripheral to be enabled.
 *
 * @param[in] *i2c_settings
 * 		A struct containing an enable bool, frequency, reference frequency, Hi/Lo Ratio, and controller setting.
 *
 ******************************************************************************/
void i2c_open(I2C_TypeDef *i2c_peripheral, I2C_OPEN_STRUCT *i2c_settings) {
	I2C_Init_TypeDef i2c_init_config;

	if (i2c_peripheral == I2C0) {
		CMU_ClockEnable(cmuClock_I2C0, true);
	} else if (i2c_peripheral == I2C1) {
		CMU_ClockEnable(cmuClock_I2C1, true);
	} else EFM_ASSERT(false);

	if ((i2c_peripheral->IF & 0x01) == 0) {
		i2c_peripheral->IFS = 0x01;
		EFM_ASSERT(i2c_peripheral->IF & 0x01);
		i2c_peripheral->IFC = 0x01;
	} else {
		i2c_peripheral->IFC = 0x01;
		EFM_ASSERT(!(i2c_peripheral->IF & 0x01));
	}

	i2c_init_config.enable = i2c_settings->enable;
	i2c_init_config.freq = i2c_settings->freq;
	i2c_init_config.refFreq = i2c_settings->refFreq;
	i2c_init_config.clhr = i2c_settings->ClockHLR;
	i2c_init_config.master = i2c_settings->controller;

	I2C_Init(i2c_peripheral, &i2c_init_config);

	i2c_peripheral->ROUTELOC0 |= i2c_settings->SCL_route | i2c_settings->SDA_route;

	i2c_peripheral->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN * i2c_settings->enable)
							 | (I2C_ROUTEPEN_SDAPEN * i2c_settings->enable);


	i2c_peripheral->IFC = I2C_IFC_ACK | I2C_IFC_NACK | I2C_IFC_MSTOP;

	i2c_peripheral->IEN |= I2C_IEN_ACK;
	i2c_peripheral->IEN |= I2C_IEN_NACK;
	i2c_peripheral->IEN |= I2C_IEN_RXDATAV;
	i2c_peripheral->IEN |= I2C_IEN_MSTOP;

	i2c_bus_reset(i2c_peripheral);

	if (i2c_peripheral == I2C0) {
		NVIC_SetPriority(I2C0_IRQn, 0);
		NVIC_EnableIRQ(I2C0_IRQn);
	} else if (i2c_peripheral == I2C1) {
		NVIC_SetPriority(I2C1_IRQn, 0);
		NVIC_EnableIRQ(I2C1_IRQn);
	} else EFM_ASSERT(false);
}

/***************************************************************************//**
 * @brief
 *		Sends an I2C command.
 *
 * @details
 *		Ensures that the I2C peripheral is available, then blocks the I2C sleep mode, configures the I2C state machine, and sends an initial request to the receiver.
 *
 * @param[in] receiver_address
 * 		The address of the receiver to communicate with.
 *
 * @param[in] command
 * 		The command to send the receiver.
 *
 * @param[in] *i2c_data
 * 		The variable in which to store the receiver's response.
 *
 * @param[in] *i2c_peripheral
 * 		The I2C peripheral to be used.
 *
 * @param[in] cb
 * 		The ID of the function to be called upon completion.
 *
 * @param[in] mode
 * 		The I2C transmission sequence to follow.
 *
 ******************************************************************************/
void i2c_start(I2C_PAYLOAD *payload, uint32_t *i2c_data) {

	EFM_ASSERT((payload->i2c_peripheral->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);
	sleep_block_mode(I2C_EM_BLOCK);

	if (payload->i2c_peripheral == I2C0) {
		state_machine_0.receiver_address = payload->receiver_address;
		state_machine_0.busy = true;
		state_machine_0.receiver_register = payload->receiver_register;
		state_machine_0.i2c = payload->i2c_peripheral;
		state_machine_0.i2c_data = i2c_data;
		state_machine_0.state = Check_If_Device_Available;
		state_machine_0.cb = payload->cb;
		state_machine_0.mode = payload->mode;
		state_machine_0.count = payload->byte_count;
		state_machine_0.i = 0;
	} else {
		state_machine_1.receiver_address = payload->receiver_address;
		state_machine_1.busy = true;
		state_machine_1.receiver_register = payload->receiver_register;
		state_machine_1.i2c = payload->i2c_peripheral;
		state_machine_1.i2c_data = i2c_data;
		state_machine_1.state = Check_If_Device_Available;
		state_machine_1.cb = payload->cb;
		state_machine_1.mode = payload->mode;
		state_machine_1.count = payload->byte_count;
		state_machine_1.i = 0;
	}

	payload->i2c_peripheral->CMD |= I2C_CMD_START;
	payload->i2c_peripheral->TXDATA = (payload->receiver_address << 1) | WRITE_BIT;
	;
}

/***************************************************************************//**
 * @brief
 *		Returns the "busy" field of the static I2C state machine.
 ******************************************************************************/
bool i2c_getBusy(I2C_TypeDef *i2c) {
	EFM_ASSERT((i2c == I2C0) | (i2c == I2C1));
	return (i2c == I2C0) ? state_machine_0.busy : state_machine_1.busy;
}

/***************************************************************************//**
 * @brief
 *		Handles interrupts from the I2C0 peripheral.
 *
 * @details
 *		Clears any raised flags then calls service_ack(), service_nack(), service_rx_data_valid(), or service_mstop() as needed.
 *

 ******************************************************************************/
void I2C0_IRQHandler(void) {
	volatile uint32_t int_flag;
	int_flag = I2C0->IF & I2C0->IEN;
	I2C0->IFC = int_flag;

	if (int_flag & I2C_IF_ACK) {
		service_ack(&state_machine_0);
	}
	if (int_flag & I2C_IF_NACK) {
		service_nack(&state_machine_0);
	}
	if (int_flag & I2C_IF_RXDATAV) {
		service_rx_data_valid(&state_machine_0);
	}
	if (int_flag & I2C_IF_MSTOP) {
		service_mstop(&state_machine_0);
	}

}

/***************************************************************************//**
 * @brief
 *		Handles interrupts from the I2C1 peripheral.
 *
 * @details
 *		Clears any raised flags then calls service_ack(), service_nack(), service_rx_data_valid(), or service_mstop() as needed.
 *

 ******************************************************************************/
void I2C1_IRQHandler(void) {

	volatile uint32_t int_flag;
	int_flag = I2C1->IF & I2C1->IEN;
	I2C1->IFC = int_flag;

	if (int_flag & I2C_IF_ACK) {
		service_ack(&state_machine_1);
	}
	if (int_flag & I2C_IF_NACK) {
		service_nack(&state_machine_1);
	}
	if (int_flag & I2C_IF_RXDATAV) {
		service_rx_data_valid(&state_machine_1);
	}
	if (int_flag & I2C_IF_MSTOP) {
		service_mstop(&state_machine_1);
	}

}
