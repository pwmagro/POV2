//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	I2C_HG
#define	I2C_HG

#include <stdint.h>
#include <stdbool.h>
#include "em_i2c.h"

#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define I2C_EM_BLOCK	EM1
#define READ_BIT		1u
#define WRITE_BIT		0u

//***********************************************************************************
// global variables
//***********************************************************************************

typedef enum {
	// Shared states
	Check_If_Device_Available,
	Set_Register,

	// Read states
	Send_Read_Command,
	Receive_Data,
	All_Data_Received,

	// Write states
	Write_Data,
	Write_Complete
} I2C_States;

typedef enum {
	R_MSB_First,
	R_LSB_First,
	W_MSB_First,
	W_LSB_First
} I2C_Mode;

typedef struct {
	// Arguments required for I2C_Init()
	I2C_TypeDef *i2c;
	bool enable;
	uint32_t freq;
	uint32_t refFreq;
	bool controller;


	// Arguments for SCL, SDA ports and pins
	uint32_t SCL_Port;
	uint32_t SCL_Pin;
	uint32_t SDA_Port;
	uint32_t SDA_Pin;

	uint32_t SCL_route;
	uint32_t SDA_route;

	// CTRL register
	I2C_ClockHLR_TypeDef ClockHLR;


} I2C_OPEN_STRUCT;

typedef struct {
	uint32_t receiver_address;
	uint32_t receiver_register;
	uint32_t *i2c_data;
	I2C_States state;
	bool busy;
	I2C_TypeDef *i2c;
	uint32_t cb;
	I2C_Mode mode;
	uint32_t count;
	uint32_t i;
} I2C_STATE_MACHINE;

typedef struct {
	uint32_t receiver_address;
	uint32_t receiver_register;
	I2C_TypeDef *i2c_peripheral;
	uint32_t cb;
	I2C_Mode mode;
	uint32_t byte_count;
} I2C_PAYLOAD;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void i2c_open(I2C_TypeDef *i2c_peripheral, I2C_OPEN_STRUCT *i2c_settings);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void i2c_start(I2C_PAYLOAD *payload, uint32_t *i2c_data);
bool i2c_getBusy(I2C_TypeDef *i2c);

#endif

