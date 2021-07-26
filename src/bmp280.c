/**
 * @file bmp280.c
 * @author Peter Magro
 * @date April 24th, 2021
 * @brief SI7021.c contains all functions for interfacing with the SI7021.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "bmp280.h"

//***********************************************************************************
// defined files
//***********************************************************************************
typedef int32_t BMP280_S32_t;
typedef uint32_t BMP280_U32_t;
typedef int64_t BMP280_S64_t;

//***********************************************************************************
// Static / Private Variables
//***********************************************************************************

// Values used for the implementation of BMP280 code included in datasheet
static BMP280_S32_t t_fine;
static uint16_t 	dig_T1;
static int16_t		dig_T2;
static int16_t		dig_T3;
static uint16_t 	dig_P1;
static int16_t		dig_P2;
static int16_t		dig_P3;
static int16_t		dig_P4;
static int16_t		dig_P5;
static int16_t		dig_P6;
static int16_t		dig_P7;
static int16_t		dig_P8;
static int16_t		dig_P9;

static union {
	uint32_t u32;
	signed short s16;
} u32_s16;

static uint32_t temp_cb, pressure_cb;
static uint32_t temp, pressure;

//***********************************************************************************
// Private functions
//***********************************************************************************
void bmp280_read_compensation_values();
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
float bmp280_compensate_P_float(BMP280_S32_t adc_P);

/*
 * Code from BMP280 datasheet. Pressure adapted to return floating-point value in Pa, otherwise
 * I have no clue what's going on here.
 */
void bmp280_read_compensation_values() {
	I2C_PAYLOAD payload = {
			.byte_count = 2,
			.cb = 0,
			.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL,
			.mode = R_LSB_First,
			.receiver_address = BMP280_RECEIVER_ADDRESS,
			.receiver_register = BMP280_T1
	};

	uint32_t receive;

	i2c_start(&payload, &receive);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_T1 = (unsigned short) receive;

	payload.receiver_register = BMP280_T2;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_T2 = u32_s16.s16;

	payload.receiver_register = BMP280_T3;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_T3 = u32_s16.s16;

	payload.receiver_register = BMP280_P1;
	i2c_start(&payload, &receive);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P1 = (unsigned short) receive;

	payload.receiver_register = BMP280_P2;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P2 = u32_s16.s16;

	payload.receiver_register = BMP280_P3;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P3 = u32_s16.s16;

	payload.receiver_register = BMP280_P4;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P4 = u32_s16.s16;

	payload.receiver_register = BMP280_P5;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P5 = u32_s16.s16;

	payload.receiver_register = BMP280_P6;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P6 = u32_s16.s16;

	payload.receiver_register = BMP280_P7;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P7 = u32_s16.s16;

	payload.receiver_register = BMP280_P8;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P8 = u32_s16.s16;

	payload.receiver_register = BMP280_P9;
	i2c_start(&payload, &(u32_s16.u32));
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	dig_P9 = u32_s16.s16;
}
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T) {
	BMP280_S32_t var1, var2, T;
	var1  = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2  = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) * ((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}
float bmp280_compensate_P_float(BMP280_S32_t adc_P) {
	BMP280_S64_t var1, var2, p;
	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
	var2 = var2 + ((var1 * (BMP280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1*(BMP280_S64_t)dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
	if (var1 == 0) {
		return 0;
	}
	p = 1048576 - adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);

	float fp = (float)p / 256;
	return fp;
}
//***********************************************************************************
// Global functions
//***********************************************************************************
void bmp280_open(uint32_t temp_callback, uint32_t pressure_callback, uint32_t bmp280_open_callback) {
	I2C_PAYLOAD config = {
			.byte_count = 1,
			.cb = bmp280_open_callback,
			.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL,
			.mode = W_MSB_First,
			.receiver_address = BMP280_RECEIVER_ADDRESS,
			.receiver_register = BMP280_CTRL_MEAS_REG
	};
	uint32_t command = BMP280_CTRL_MEAS_VALUE;

	i2c_start(&config, &command);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));

	config.receiver_register = BMP280_CONFIG_REG;
	command = BMP280_CONFIG_VALUE;

	i2c_start(&config, &command);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));

	temp_cb = temp_callback;
	pressure_cb = pressure_callback;
}

// TODO: these next two giblets need to be given get_temp and get_press fn's
void bmp280_read_temp() {
	I2C_PAYLOAD payload = {
			.byte_count = 3,
			.cb = temp_cb,
			.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL,
			.mode = R_MSB_First,
			.receiver_address = BMP280_RECEIVER_ADDRESS,
			.receiver_register = BMP280_TEMP_MSB_REG
	};

	i2c_start(&payload, &temp);
}

void bmp280_read_pressure() {
	I2C_PAYLOAD payload = {
			.byte_count = 3,
			.cb = pressure_cb,
			.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL,
			.mode = R_MSB_First,
			.receiver_address = BMP280_RECEIVER_ADDRESS,
			.receiver_register = BMP280_PRESSURE_MSB_REG
	};

	i2c_start(&payload, &pressure);
}

float bmp280_get_last_temp_read(void) {
	return (float)bmp280_compensate_T_int32(temp) / 100;
}

float bmp280_get_last_pressure_read(void) {
	return bmp280_compensate_P_float(pressure);
}

/*
 * Using formula from https://keisan.casio.com/exec/system/1224585971
 */
float bmp280_get_altitude(void) {
	float current_pressure = bmp280_compensate_P_float(pressure);
	float current_temp = (float)bmp280_compensate_T_int32(temp) / 100;
	float altitude = ((pow((SEA_LEVEL_PRESSURE / current_pressure), (1/5.257)) - 1) * (current_temp + 273.15)) / 0.0065;
	return altitude;
}

bool bmp280_i2c_test(uint32_t cb) {

	bmp280_read_compensation_values();

	// Test reading: ID (should be 0x58)
	I2C_PAYLOAD payload = {
			.byte_count = 1,
			.cb = cb,
			.i2c_peripheral = ENVSENSE_I2C_PERIPHERAL,
			.mode = R_MSB_First,
			.receiver_address = BMP280_RECEIVER_ADDRESS,
			.receiver_register = BMP280_ID_REG
	};
	uint32_t ret_value;
	i2c_start(&payload, &ret_value);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	EFM_ASSERT(ret_value == BMP280_ID_VALUE);

	// Now test it with 2-byte

	// Write CTRL_MEAS
	payload.mode = W_MSB_First;
	payload.receiver_register = BMP280_CTRL_MEAS_REG;

	uint32_t command = BMP280_CTRL_MEAS_VALUE;

	i2c_start(&payload, &command);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));


	// Read CTRL_MEAS to confirm we wrote successfully
	payload.mode = R_MSB_First;

	i2c_start(&payload, &ret_value);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	EFM_ASSERT(ret_value == command);

	// Write CONFIG
	payload.mode = W_MSB_First;
	payload.receiver_register = BMP280_CONFIG_REG;
	command = BMP280_CONFIG_VALUE;
//	i2c_start(&payload, &command);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));

	timer_delay(1000);

	// read temp and pressure registers and confirm they're reasonable values
	uint32_t pressure, temp;
	payload.mode = R_MSB_First;
	payload.byte_count = 3;
	payload.receiver_register = BMP280_PRESSURE_MSB_REG;
	i2c_start(&payload, &pressure);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	pressure = pressure >> 4;

	payload.receiver_register = BMP280_TEMP_MSB_REG;
	i2c_start(&payload, &temp);
	while(i2c_getBusy(ENVSENSE_I2C_PERIPHERAL));
	temp = temp >> 4;

	BMP280_S32_t comp_temp = bmp280_compensate_T_int32((BMP280_S32_t)temp);
	float comp_temp_f = (float)comp_temp / 100;
	EFM_ASSERT((comp_temp_f > 18) && (comp_temp_f < 32));

	float comp_press_f = bmp280_compensate_P_float((BMP280_S32_t)pressure);
	EFM_ASSERT((comp_press_f > 80000) && (comp_press_f < 101000));

	return true;
}


