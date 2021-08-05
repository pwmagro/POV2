/**
 * @file SI7021.c
 * @author Peter Magro
 * @date July 20th, 2021
 * @brief SI7021.c contains all functions for interfacing with the SI7021.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "SI7021.h"
#include "HW_delay.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static uint32_t data;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		Calls i2c_open() with settings to communicate with the Si7021.
 *
 * @details
 *		Si7021 pins, ports, and routes are defined in brd_config.h.
 *
 * @param[in] *i2c
 * 		The I2C peripheral to configure.
 *
 * @param[in] enable
 * 		Whether or not to enable the I2C peripheral.
 *
 *
 ******************************************************************************/
void si7021_i2c_open(I2C_TypeDef *i2c, bool enable) {
	I2C_OPEN_STRUCT settings;

	timer_delay(80);

	settings.ClockHLR = i2cClockHLRAsymetric;
	settings.SCL_Pin = ENVSENSE_SCL_PIN;
	settings.SCL_Port = ENVSENSE_SCL_PORT;
	settings.SCL_route = ENVSENSE_SCL_ROUTE;
	settings.SDA_Pin = ENVSENSE_SDA_PIN;
	settings.SDA_Port = ENVSENSE_SDA_PORT;
	settings.SDA_route = ENVSENSE_SDA_ROUTE;
	settings.enable = enable;
	settings.freq = I2C_FREQ_FAST_MAX;
	settings.refFreq = 0;
	settings.controller = true;

	i2c_open(i2c, &settings);
}

/***************************************************************************//**
 * @brief
 *		Sends and handles a no-hold command with the Si7021 without checksum.
 *
 * @details
 *		Will not return any values, only store the result in the private data variable defined in SI7021.c.
 *
 * @param[in] command
 * 		The command to send to the Si7021.
 *
 * @param[in] cb
 * 		The function to be scheduled when the transaction is complete.
 *
 * @param[in] mode
 * 		The mode in which to operate (R_Single_Byte or R_MSB_First)
 *
 ******************************************************************************/
void si7021_read(uint32_t command, uint32_t cb, uint32_t byte_count) {
	I2C_PAYLOAD payload = {
			.receiver_address = SI7021_RECEIVER_ADDRESS,
			.receiver_register = command,
			.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL,
			.cb = cb,
			.mode = R_MSB_First,
			.byte_count = byte_count
	};

	if ((ENVSENSE_I2C_PERIPHERAL->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE) {
		i2c_start(&payload, &data);
	}
}

/***************************************************************************//**
 * @brief
 *		Writes to an Si7021 user register.
 *
 * @note
 *		Will not modify the value at *payload - this is a pointer only for compatibility with read operations.
 *
 * @param[in] user_register
 * 		The Si7021 register to write to.
 *
 * @param[in] *command
 * 		The value to write to the register.
 *
 * @param[in] cb
 * 		The function to be scheduled when the transaction is complete.
 *
 ******************************************************************************/
void si7021_write(uint32_t user_register, uint32_t *command, uint32_t cb) {
	if ((ENVSENSE_I2C_PERIPHERAL->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE) {
		I2C_PAYLOAD payload = {
				.receiver_address = SI7021_RECEIVER_ADDRESS,
				.receiver_register = user_register,
				.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL,
				.cb = cb,
				.mode = W_MSB_First,
				.byte_count = 1
		};

		i2c_start(&payload, command);
	}
}

/***************************************************************************//**
 * @brief
 *		Converts the private data variable in SI7021.c to a % relative humidity.
 *
 * @details
 *		Implements the relative humidity formula provided in the Si7021 datasheet.
 *
 ******************************************************************************/
float si7021_calculate_humidity() {
	float humidity = ((125.0 * data) / 65536) - 6; // a nice formula given to us by Si Labs
	return humidity;
}

/***************************************************************************//**
 * @brief
 *		Converts the private data variable in SI7021.c to a temperature in Fahrenheit.
 *
 * @details
 *		Implements the temperature formula provided in the Si7021 datasheet.
 *
 ******************************************************************************/
float si7021_calculate_temperature() {
	float temperature = ((175.72 * data) / 65536) - 46.85;	// a nice formula given to us by Si Labs
	temperature = (temperature * 9 / 5) + 32;				// convert C to F
	return temperature;
}

/***************************************************************************//**
 * @brief
 *		TDD routine to validate that the Si7021 and I2C peripheral are configured
 *		and communicating properly.
 *
 * @details
 *		Validates the operation of single byte reads, MSB-first two-byte reads,
 *		and single byte writes.
 *
 * @note
 * 		This function assumes that the Si7021's user registers have not been modified.
 * 		Since humidity values are checked within a range instead of against a known value,
 * 		it is possible (but unlikely) for the test to pass with incorrect readings.
 * 		Similarly, if the environment is particularly dry or humid, the test may fail
 * 		even with accurate readings.
 *
 * @param[in] cb
 * 		The callback function to be passed into i2c_start() calls.
 *
 ******************************************************************************/
bool si7021_i2c_test(uint32_t cb) {

	bool success = false;
	I2C_PAYLOAD payload;

	// This section of code does two things: First, it reads the Si7021 user
	// register 1 and saves the value to original_register_value. Successfully
	// exiting the i2c_start function means that the i2c handled the arguments
	// passed to it. Then, an ASSERT statement validates that the value of
	// original_register_value is the default value for the Si7021 user register 1.
	// This confirms that the response from the Si7021 was received and interpreted
	// properly.

	payload.byte_count = 1;
	payload.cb = cb;
	payload.receiver_register = SI7021_READ_USER_REG_1;
	payload.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL;
	payload.mode = R_MSB_First;
	payload.receiver_address = SI7021_RECEIVER_ADDRESS;

	uint32_t original_register_value;
	i2c_start(&payload, &original_register_value);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));

	EFM_ASSERT((original_register_value == SI7021_12_BIT_HUMIDITY) | (original_register_value == SI7021_8_BIT_HUMIDITY));


	// This section of code validates the MSB-first two-byte read by calling i2c_start
	// with values for a humidity read. By default, the Si7021 should send two bytes when
	// a humidity read is performed. The received value is then converted to a relative
	// humidity, and checked to be a reasonable value (20% < RH < 40%)
	payload.receiver_register = SI7021_RELATIVE_HUMIDITY_NO_HOLD;
	payload.byte_count = 2;

	uint32_t humidity;
	i2c_start(&payload, &humidity);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	humidity = ((125 * humidity) / 65536) - 6;
	EFM_ASSERT(humidity > 20 && humidity < 50);


	i2c_start(&payload, &humidity);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	humidity = ((125 * humidity) / 65536) - 6;
	EFM_ASSERT(humidity > 20 && humidity < 50);

	// This section attempts to write to the Si7021 user register 1, setting the humidity
	// to 8-bit precision. This means that humidity reads should now be one byte. Since we
	// have already validated that single-byte reads work, if the humidity is within our
	// range of "reasonable values," then we've correctly received and interpreted a single
	// byte humidity read.
	uint32_t command = SI7021_8_BIT_HUMIDITY;
	payload.receiver_register = SI7021_WRITE_USER_REG_1;
	payload.mode = W_LSB_First;
	payload.byte_count = 1;

	i2c_start(&payload, &command);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	timer_delay(15);

	payload.receiver_register = SI7021_READ_USER_REG_1;
	payload.mode = R_MSB_First;
	uint32_t test_value;
	i2c_start(&payload, &test_value);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	EFM_ASSERT(test_value == SI7021_8_BIT_HUMIDITY);

	payload.receiver_register = SI7021_RELATIVE_HUMIDITY_NO_HOLD;
	payload.byte_count = 2;
	i2c_start(&payload, &humidity);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	humidity = ((125 * humidity) / 65536) - 6;
	EFM_ASSERT(humidity > 20 && humidity < 50);

	// This section of code restores the register we wrote to back to its original value.

	payload.receiver_register = SI7021_WRITE_USER_REG_1;
	payload.byte_count = 1;
	payload.mode = W_MSB_First;

	i2c_start(&payload, &original_register_value);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	timer_delay(15);

	success = true;
	return success;
}
