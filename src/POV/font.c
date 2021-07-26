/**
 * @file font.c
 * @author Peter Magro
 * @date June 17th, 2021
 * @brief Establishes a text format for POV display.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "font.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *		Converts a char to a POV_CHAR
 *
 * @details
 *		POV_CHAR stores ASCII characters in a format readable by the POV driver.
 *
 ******************************************************************************/
POV_CHAR convert_to_pov_char(char character) {
	switch(character) {
	case ' ':
		return SPACE;
		break;
	case '!':
		return EXCLAMATION;
		break;
	case '"':
		return DOUBLE_QUOTE;
		break;
	case '#':
		return HASH;
		break;
	case '$':
		return DOLLAR;
		break;
	case '%':
		return PERCENT;
		break;
	case '&':
		return AMPERSAND;
		break;
	case '\'':
		return SINGLE_QUOTE;
		break;
	case '(':
		return LEFT_PARENTHESES;
		break;
	case ')':
		return RIGHT_PARENTHESES;
		break;
	case '*':
		return ASTERISK;
		break;
	case '+':
		return PLUS;
		break;
	case ',':
		return COMMA;
		break;
	case '-':
		return DASH;
		break;
	case '.':
		return PERIOD;
		break;
	case '/':
		return FORWARD_SLASH;
		break;
	case '0':
		return ZERO;
		break;
	case '1':
		return ONE;
		break;
	case '2':
		return TWO;
		break;
	case '3':
		return THREE;
		break;
	case '4':
		return FOUR;
		break;
	case '5':
		return FIVE;
		break;
	case '6':
		return SIX;
		break;
	case '7':
		return SEVEN;
		break;
	case '8':
		return EIGHT;
		break;
	case '9':
		return NINE;
		break;
	case ':':
		return COLON;
		break;
	case ';':
		return SEMICOLON;
		break;
	case '<':
		return LESS_THAN;
		break;
	case '=':
		return EQUAL;
		break;
	case '>':
		return GREATER_THAN;
		break;
	case '?':
		return QUESTION;
		break;
	case '@':
		return AT;
		break;
	case 'A':
		return A_u;
		break;
	case 'B':
		return B_u;
		break;
	case 'C':
		return C_u;
		break;
	case 'D':
		return D_u;
		break;
	case 'E':
		return E_u;
		break;
	case 'F':
		return F_u;
		break;
	case 'G':
		return G_u;
		break;
	case 'H':
		return H_u;
		break;
	case 'I':
		return I_u;
		break;
	case 'J':
		return J_u;
		break;
	case 'K':
		return K_u;
		break;
	case 'L':
		return L_u;
		break;
	case 'M':
		return M_u;
		break;
	case 'N':
		return N_u;
		break;
	case 'O':
		return O_u;
		break;
	case 'P':
		return P_u;
		break;
	case 'Q':
		return Q_u;
		break;
	case 'R':
		return R_u;
		break;
	case 'S':
		return S_u;
		break;
	case 'T':
		return T_u;
		break;
	case 'U':
		return U_u;
		break;
	case 'V':
		return V_u;
		break;
	case 'W':
		return W_u;
		break;
	case 'X':
		return X_u;
		break;
	case 'Y':
		return Y_u;
		break;
	case 'Z':
		return Z_u;
		break;
	case '[':
		return LEFT_BRACKET;
		break;
	case '\\':
		return BACK_SLASH;
		break;
	case ']':
		return RIGHT_BRACKET;
		break;
	case '^':
		return CARAT;
		break;
	case '_':
		return UNDERSCORE;
		break;
	case '`':
		return TICK;
		break;
	case 'a':
		return A_l;
		break;
	case 'b':
		return B_l;
		break;
	case 'c':
		return C_l;
		break;
	case 'd':
		return D_l;
		break;
	case 'e':
		return E_l;
		break;
	case 'f':
		return F_l;
		break;
	case 'g':
		return G_l;
		break;
	case 'h':
		return H_l;
		break;
	case 'i':
		return I_l;
		break;
	case 'j':
		return J_l;
		break;
	case 'k':
		return K_l;
		break;
	case 'l':
		return L_l;
		break;
	case 'm':
		return M_l;
		break;
	case 'n':
		return N_l;
		break;
	case 'o':
		return O_l;
		break;
	case 'p':
		return P_l;
		break;
	case 'q':
		return Q_l;
		break;
	case 'r':
		return R_l;
		break;
	case 's':
		return S_l;
		break;
	case 't':
		return T_l;
		break;
	case 'u':
		return U_l;
		break;
	case 'v':
		return V_l;
		break;
	case 'w':
		return W_l;
		break;
	case 'x':
		return X_l;
		break;
	case 'y':
		return Y_l;
		break;
	case 'z':
		return Z_l;
		break;
	case '{':
		return LEFT_CURLY_BRACKET;
		break;
	case '|':
		return PIPE;
		break;
	case '}':
		return RIGHT_CURLY_BRACKET;
		break;
	case '~':
		return TILDE;
		break;
	case '\0':
		return SPACE;
		break;
	default:
		EFM_ASSERT(false);
		return SPACE;				// SPACE = 0x00000000
		break;
	}
}
