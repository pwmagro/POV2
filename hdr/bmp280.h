//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	BMP280_HG
#define	BMP280_HG

/* System include statements */
#include <math.h>

/* Silicon Labs include statements */


/* The developer's include statements */
#include "i2c.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define BMP280_RECEIVER_ADDRESS	0x77

#define BMP280_RESET_REG		0xE0
#define BMP280_RESET_VALUE		0xB6

#define BMP280_ID_REG			0xD0
#define BMP280_ID_VALUE			0x58
#define BMP280_CTRL_MEAS_REG	0xF4
#define BMP280_CTRL_MEAS_VALUE	0x3F	// no temp measurement; no pressure oversampling; constant measurement

#define	BMP280_CONFIG_REG		0xF5
#define BMP280_CONFIG_VALUE		0x30	// 0.5ms standby, 16 filter coefficient, SPI setting doesn't matter

#define BMP280_STATUS_REG		0xF3
#define BMP280_STATUS_MEASURING	(1u << 3)
#define BMP280_STATUS_IM_UPDATE	(1u << 0)

#define BMP280_PRESSURE_MSB_REG	0xF7
#define BMP280_PRESS_LSB_REG	0xF8
#define BMP280_PRESS_XLSB_REG	0xF9
#define BMP280_TEMP_MSB_REG		0xFA
#define BMP280_TEMP_LSB_REG		0xFB
#define BMP280_TEMP_XLSB_REG	0xFC

#define BMP280_T1	0x88
#define BMP280_T2	0x8A
#define BMP280_T3	0x8C
#define BMP280_P1	0x8E
#define BMP280_P2	0x90
#define BMP280_P3	0x92
#define BMP280_P4	0x94
#define BMP280_P5	0x96
#define BMP280_P6	0x98
#define BMP280_P7	0x9A
#define BMP280_P8	0x9C
#define BMP280_P9	0x9E

#define SEA_LEVEL_PRESSURE	1013.25

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
bool bmp280_i2c_test(uint32_t cb);
void bmp280_open(uint32_t temp_callback, uint32_t pressure_callback, uint32_t bmp280_open_callback);
void bmp280_read_temp(void);
void bmp280_read_pressure(void);

float bmp280_get_last_temp_read();
float bmp280_get_last_pressure_read();
float bmp280_get_altitude();

#endif
