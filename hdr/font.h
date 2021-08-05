#ifndef FONT_HG
#define FONT_HG

//***********************************************************************************
// Include files
//***********************************************************************************

//***********************************************************************************
// defined files
//***********************************************************************************
/*
 * 	   ROW 0   1   2   3   4
 * 	COL
 * 	 0     5   11  17  23  29
 * 	 1     4   10  16  22  28
 * 	 2     3   9   15  21  27
 * 	 3     2   8   14  20  26
 * 	 4     1   7   13  19  25
 * 	 5     0   6   12  18  24
 */
typedef enum {
	SPACE = 				0x00000000,
	EXCLAMATION = 			0x0003D000,
	DOUBLE_QUOTE = 			0x00C00C00,
	HASH = 					0x14FE4FCA,
	DOLLAR = 				0x005B3680,
	PERCENT = 				0x2240C091,
	AMPERSAND = 			0x054AD542,
	SINGLE_QUOTE = 			0x00030000,
	LEFT_PARENTHESES = 		0x00021780,
	RIGHT_PARENTHESES =		0x007A1000,
	ASTERISK =				0x00A10A00,
	PLUS = 					0x0411F104,
	COMMA =					0x00002040,
	DASH = 					0x04104104,
	PERIOD =				0x00001000,
	FORWARD_SLASH = 		0x00C0C0C0,
	ZERO =					0x1E961A5E,
	ONE =					0x01FD1441,
	TWO =					0x190658D1,
	THREE =					0x16A69852,
	FOUR =					0x3F208238,
	FIVE =					0x26A69A7A,
	SIX = 					0x06A6995E,
	SEVEN =					0x38922860,
	EIGHT =					0x16A69A56,
	NINE =					0x1E965958,
	COLON =					0x00012000,
	SEMICOLON = 			0x00012040,
	LESS_THAN =				0x1144A284,
	EQUAL =					0x12492492,
	GREATER_THAN =			0x0428A451,
	QUESTION =				0x10A25810,
	AT = 					0x1E867990,
	A_u =					0x1FA28A1F,
	B_u =					0x16A69A7F,
	C_u =					0x1286185E,
	D_u =					0x1E86187F,
	E_u =					0x21869A7F,
	F_u =					0x20828A3F,
	G_u =					0x1796585E,
	H_u =					0x3F20823F,
	I_u =					0x2183F861,
	J_u =					0x20FA1862,
	K_u =					0x2350823F,
	L_u =					0x0104107F,
	M_u =					0x3F40843F,
	N_u =					0x3F10843F,
	O_u =					0x1E86185E,
	P_u =					0x1892493F,
	Q_u =					0x1D8A185E,
	R_u =					0x1B92493F,
	S_u =					0x12969A52,
	T_u =					0x2083F820,
	U_u =					0x3E04107E,
	V_u =					0x381811B8,
	W_u =					0x3F0840BF,
	X_u =					0x23508523,
	Y_u =					0x30207230,
	Z_u =					0x31A658B1,
	LEFT_BRACKET =			0x00861FC0,
	BACK_SLASH =			0x000CCC00,
	RIGHT_BRACKET =			0x00FE1840,
	CARAT =					0x08420408,
	UNDERSCORE =			0x01041041,
	TICK =					0x00010800,
	A_l =					0x1F25144E,
	B_l =					0x0624917F,
	C_l =					0x0A45144E,
	D_l =					0x3F149246,
	E_l =					0x0855554E,
	F_l =					0x004247C0,
	G_l =					0x1E555548,
	H_l =					0x0720813F,
	I_l =					0x00017000,
	J_l =					0x00B81080,
	K_l =					0x002C413F,
	L_l =					0x00001F80,
	M_l =					0x0F80C41F,
	N_l =					0x0F41021F,
	O_l =					0x0E45144E,
	P_l =					0x0C49229F,
	Q_l =					0x1F29248C,
	R_l =					0x0841021F,
	S_l =					0x02555548,
	T_l =					0x0027E200,
	U_l =					0x1F04105E,
	V_l =					0x1C08109C,
	W_l =					0x1E04605E,
	X_l =					0x11282291,
	Y_l =					0x1E145158,
	Z_l =					0x19555553,
	LEFT_CURLY_BRACKET =	0x00861588,
	PIPE =					0x0003F000,
	RIGHT_CURLY_BRACKET =	0x085A1840,
	TILDE =					0x0608420C,
	FULL_BLOCK =			0x3FFFFFFF,
	TOP_HALF =				0x38E38E38,
	BOTTOM_HALF = 			0x071C71C7,
	LEFT_HALF =				0x000CFFFF,
	RIGHT_HALF =			0x3FFFF000,
} POV_CHAR;

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
POV_CHAR convert_to_pov_char(char character);

#endif
