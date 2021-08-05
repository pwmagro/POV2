#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG

//***********************************************************************************
// Include files
//***********************************************************************************
/* System include statements */


/* Silicon Labs include statements */

/* The developer's include statements */


//***********************************************************************************
// defined files
//***********************************************************************************

// GPIO pin setup
#define STRONG_DRIVE

// System Clock setup
#define MCU_HFRCO_FREQ				cmuHFRCOFreq_38M0Hz

// LETIMER PWM Configuration
#define     PWM_ROUTE_0				LETIMER_ROUTELOC0_OUT0LOC_LOC0
#define     PWM_ROUTE_1				LETIMER_ROUTELOC0_OUT1LOC_LOC0

// TIMER Configuration


// ADC config
#define		ADC_TARGET_FREQ			148436
#define		ADC_INPUT_BUS			adcPosSelAPORT2XCH21	// PF5
#define		ADC_PORT				gpioPortF
#define		ADC_PIN					5u
#define		ADC_GPIOMODE			gpioModeDisabled
#define		ADC_DEFAULT				true

// I2C pin config
#define		I2C_SCL_PORT			gpioPortC
#define		I2C_SCL_PIN				11u
#define		I2C_SCL_DEFAULT			true
#define		I2C_SCL_GPIOMODE		gpioModeWiredAnd

#define		I2C_SDA_PORT			gpioPortC
#define		I2C_SDA_PIN				10u
#define		I2C_SDA_DEFAULT			true
#define		I2C_SDA_GPIOMODE		gpioModeWiredAnd

// Hall Effect Sensor
#define		HALL_EFFECT_PORT			gpioPortA
#define		HALL_EFFECT_PIN				5u
#define		HALL_EFFECT_DEFAULT			true
#define		HALL_EFFECT_GPIOMODE		gpioModeInput
#define		HALL_EFFECT_INT_NUM			HALL_EFFECT_PIN
#define		HALL_EFFECT_INT_RISING		true
#define		HALL_EFFECT_INT_FALLING		false
#define		HALL_EFFECT_INT_EN			true

// WS2812B config
#define		WS2812B_SPI_MOSI_PORT		gpioPortK
#define		WS2812B_SPI_MOSI_PIN		0u
#define		WS2812B_SPI_MOSI_DEFAULT	true
#define		WS2812B_SPI_MOSI_GPIOMODE	gpioModePushPull
#define		WS2812B_SPI_MOSI_DSTRENGTH	gpioDriveStrengthWeakAlternateWeak
#define		WS2812B_USART				USART2

#define		WS2812B_NUM_LEDS			12u

// Button config
#define		BUTTON_DEFAULT				true

#define		BUTTON_0_PORT				gpioPortD
#define		BUTTON_0_PIN				14u
#define		BUTTON_0_CONFIG				gpioModeInput
#define		BUTTON_0_INT_NUM			BUTTON_0_PIN
#define		BUTTON_0_INT_RISING			false
#define		BUTTON_0_INT_FALLING		true
#define		BUTTON_0_INT_ENABLE			true

#define		BUTTON_1_PORT				gpioPortD
#define		BUTTON_1_PIN				15u
#define		BUTTON_1_CONFIG				gpioModeInput
#define		BUTTON_1_INT_NUM			BUTTON_1_PIN
#define		BUTTON_1_INT_RISING			false
#define		BUTTON_1_INT_FALLING		true
#define		BUTTON_1_INT_ENABLE			true

// SI7021 config
#define		ENVSENSE_I2C_PERIPHERAL		I2C1

#define		ENVSENSE_SCL_PORT			gpioPortC
#define		ENVSENSE_SCL_PIN			5u
#define		ENVSENSE_SCL_ROUTE			I2C_ROUTELOC0_SCLLOC_LOC17
#define		ENVSENSE_SDA_PORT			gpioPortC
#define		ENVSENSE_SDA_PIN			4u
#define		ENVSENSE_SDA_ROUTE			I2C_ROUTELOC0_SDALOC_LOC17
#define		ENVSENSE_EN_PORT			gpioPortF
#define		ENVSENSE_EN_PIN				9u

#define 	ENVSENSE_SCL_DEFAULT		true
#define 	ENVSENSE_SCL_GPIOMODE		gpioModeWiredAnd
#define 	ENVSENSE_SDA_DEFAULT		true
#define 	ENVSENSE_SDA_GPIOMODE		gpioModeWiredAnd
#define		ENVSENSE_EN_DRIVE_STRENGTH 	gpioDriveStrengthWeakAlternateWeak
#define 	ENVSENSE_EN_DEFAULT			true		// Default false (0) = off, true (1) = on
#define 	ENVSENSE_EN_GPIOMODE		gpioModePushPull

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
