#ifndef POV_HG
#define POV_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_core.h"

#include "brd_config.h"
#include "ws2812b.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define		MEASURE_ZONE_WIDTH			25u
#define		DEAD_ZONE_WIDTH				100u
#define		DISPLAY_ZONE_WIDTH			135u
#define		DISPLAY_PIXEL_WIDTH			1.40625
#define		DEGREES_360					360u

#define		DISPLAY_NUM_CHARS			16u
#define		DISPLAY_CHAR_PIXELS_WIDE	5
#define		DISPLAY_CHAR_PIXELS_HIGH	6
#define		DISPLAY_NUM_PIXELS_WIDE		DISPLAY_NUM_CHARS * 6

#define		TWO_SECONDS					MCU_HFRCO_FREQ * 2

#define		POV_MEASURE_TIMER			WTIMER0
#define		POV_TICK_TIMER				WTIMER1
#define		POV_INFO_LETIMER			LETIMER1

#define		POV_INFO_TICK_RATE			2

#define		GPIO_EVEN_CB		0x0001
#define		GPIO_ODD_CB			0x0002
#define		BOOT_UP_CB			0x0004
#define		SI7021_HUMIDITY_CB	0x0008
#define		SI7021_TEMP_CB		0x0010
#define		BMP280_TEMP_CB		0x0020
#define		BMP280_PRESSURE_CB	0x0040

//***********************************************************************************
// global variables
//***********************************************************************************
typedef enum {
	measure,
	dead_one,
	display,
	dead_two
} pov_position;

typedef struct {
	char *top_string;
	char *bottom_string;
	GRB_TypeDef top_colors[DISPLAY_NUM_CHARS + 1];    // Extra character for string termination
	GRB_TypeDef bottom_colors[DISPLAY_NUM_CHARS + 1];
} POV_Display_TypeDef;

typedef enum {
	HelloWorld,
	TempHumidity,
	Credits,
	BatteryLevel,
	PressureAltitude,
	Filler6,
	Filler7,
	Filler8,
	Filler9,
	Filler10,
	Filler11,
	Filler12,
} POV_DisplayMode_TypeDef;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void pov_open(void);
void pov_handle_measure(uint32_t count);
void pov_start_display(void);
void pov_end_display(void);
void pov_tick(void);
void pov_update_display(POV_Display_TypeDef display);
void pov_update_humidity(void);
void pov_update_si7021_temp(void);
void pov_update_bmp280(void);
void pov_show_menu(void);
void pov_change_mode(bool direction);

#endif
