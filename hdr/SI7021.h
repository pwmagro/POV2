//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	SI7021_HG
#define	SI7021_HG

/* System include statements */


/* Silicon Labs include statements */


/* The developer's include statements */
#include "i2c.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define SI7021_RECEIVER_ADDRESS					0x40
#define SI7021_RELATIVE_HUMIDITY_NO_HOLD		0xF5
#define	SI7021_TEMPERATURE_NO_HOLD				0xF3
#define SI7021_WRITE_USER_REG_1					0xE6
#define SI7021_READ_USER_REG_1					0xE7
#define SI7021_8_BIT_HUMIDITY					0x3B
#define SI7021_12_BIT_HUMIDITY					0x3A

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void si7021_i2c_open(I2C_TypeDef *i2c, bool enable);
void si7021_read(uint32_t command, uint32_t cb, uint32_t byte_count);
float si7021_calculate_humidity();
float si7021_calculate_temperature();
bool si7021_i2c_test(uint32_t cb);
void si7021_write(uint32_t user_register, uint32_t *payload, uint32_t cb);

#endif
