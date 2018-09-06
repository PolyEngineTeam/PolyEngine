#pragma once

#include <Defines.hpp>

namespace Poly
{
	enum class eControllerAxis
	{
		UNKNOWN = -1,
		AXIS_LX = 0,
		AXIS_LY = 1,
		AXIS_RX = 2,
		AXIS_RY = 3,
		AXIS_LT = 4,
		AXIS_RT = 5,
		_COUNT = 6,
	};
	
	enum class eControllerButton
	{
		UNKNOWN = -1,
		BUTTON_A = 0,
		BUTTON_B = 1,
		BUTTON_X = 2,
		BUTTON_Y = 3,
		BUTTON_BACK = 4,
		BUTTON_GUIDE = 5,
		BUTTON_START = 6,
		BUTTON_LS = 7,
		BUTTON_RS = 8,
		BUTTON_LB = 9,
		BUTTON_RB = 10,
		BUTTON_UP = 11,
		BUTTON_DOWN = 12,
		BUTTON_LEFT = 13,
		BUTTON_RIGHT = 14,
		_COUNT = 15,
	};

	enum class eMouseButton
	{
		UNKNOWN = 0,
		LEFT = 1,
		MIDDLE = 2,
		RIGHT = 3,
		X1 = 4,
		X2 = 5,
		_COUNT = 6
	};

	enum class eKey
	{
		UNKNOWN = 0,
		KEY_A = 4,
		KEY_B = 5,
		KEY_C = 6,
		KEY_D = 7,
		KEY_E = 8,
		KEY_F = 9,
		KEY_G = 10,
		KEY_H = 11,
		KEY_I = 12,
		KEY_J = 13,
		KEY_K = 14,
		KEY_L = 15,
		KEY_M = 16,
		KEY_N = 17,
		KEY_O = 18,
		KEY_P = 19,
		KEY_Q = 20,
		KEY_R = 21,
		KEY_S = 22,
		KEY_T = 23,
		KEY_U = 24,
		KEY_V = 25,
		KEY_W = 26,
		KEY_X = 27,
		KEY_Y = 28,
		KEY_Z = 29,

		KEY_1 = 30,
		KEY_2 = 31,
		KEY_3 = 32,
		KEY_4 = 33,
		KEY_5 = 34,
		KEY_6 = 35,
		KEY_7 = 36,
		KEY_8 = 37,
		KEY_9 = 38,
		KEY_0 = 39,

		RETURN = 40,
		ESCAPE = 41,
		BACKSPACE = 42,
		TAB = 43,
		SPACE = 44,

		MINUS = 45,
		EQUALS = 46,
		LEFTBRACKET = 47,
		RIGHTBRACKET = 48,
		BACKSLASH = 49, /**< Located at the lower left of the return
								  *   key on ISO keyboards and at the right end
								  *   of the QWERTY row on ANSI keyboards.
								  *   Produces REVERSE SOLIDUS (backslash) and
								  *   VERTICAL LINE in a US layout, REVERSE
								  *   SOLIDUS and VERTICAL LINE in a UK Mac
								  *   layout, NUMBER SIGN and TILDE in a UK
								  *   Windows layout, DOLLAR SIGN and POUND SIGN
								  *   in a Swiss German layout, NUMBER SIGN and
								  *   APOSTROPHE in a German layout, GRAVE
								  *   ACCENT and POUND SIGN in a French Mac
								  *   layout, and ASTERISK and MICRO SIGN in a
								  *   French Windows layout.
								  */
		NONUSHASH = 50, /**< ISO USB keyboards actually use this code
								  *   instead of 49 for the same key, but all
								  *   OSes I've seen treat the two codes
								  *   identically. So, as an implementor, unless
								  *   your keyboard generates both of those
								  *   codes and your OS treats them differently,
								  *   you should generate SDL_SCANCODE_BACKSLASH
								  *   instead of this code. As a user, you
								  *   should not rely on this code because SDL
								  *   will never generate it with most (all?)
								  *   keyboards.
								  */
		SEMICOLON = 51,
		POSTROPHE = 52,
		GRAVE = 53, /**< Located in the top left corner (on both ANSI
							  *   and ISO keyboards). Produces GRAVE ACCENT and
							  *   TILDE in a US Windows layout and in US and UK
							  *   Mac layouts on ANSI keyboards, GRAVE ACCENT
							  *   and NOT SIGN in a UK Windows layout, SECTION
							  *   SIGN and PLUS-MINUS SIGN in US and UK Mac
							  *   layouts on ISO keyboards, SECTION SIGN and
							  *   DEGREE SIGN in a Swiss German layout (Mac:
							  *   only on ISO keyboards), CIRCUMFLEX ACCENT and
							  *   DEGREE SIGN in a German layout (Mac: only on
							  *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
							  *   French Windows layout, COMMERCIAL AT and
							  *   NUMBER SIGN in a French Mac layout on ISO
							  *   keyboards, and LESS-THAN SIGN and GREATER-THAN
							  *   SIGN in a Swiss German, German, or French Mac
							  *   layout on ANSI keyboards.
							  */
		COMMA = 54,
		PERIOD = 55,
		SLASH = 56,
		CAPSLOCK = 57,

		F1 = 58,
		F2 = 59,
		F3 = 60,
		F4 = 61,
		F5 = 62,
		F6 = 63,
		F7 = 64,
		F8 = 65,
		F9 = 66,
		F10 = 67,
		F11 = 68,
		F12 = 69,

		PRINTSCREEN = 70,
		SCROLLLOCK = 71,
		PAUSE = 72,
		INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
								   does send code 73, not 117) */
		HOME = 74,
		PAGEUP = 75,
		DEL = 76,
		END = 77,
		PAGEDOWN = 78,
		RIGHT = 79,
		LEFT = 80,
		DOWN = 81,
		UP = 82,

		NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
									 */
		NUM_DIVIDE = 84,
		NUM_MULTIPLY = 85,
		NUM_MINUS = 86,
		NUM_PLUS = 87,
		NUM_ENTER = 88,
		NUM_1 = 89,
		NUM_2 = 90,
		NUM_3 = 91,
		NUM_4 = 92,
		NUM_5 = 93,
		NUM_6 = 94,
		NUM_7 = 95,
		NUM_8 = 96,
		NUM_9 = 97,
		NUM_0 = 98,
		NUM_PERIOD = 99,

		NONUSBACKSLASH = 100, /**< This is the additional key that ISO
										*   keyboards have over ANSI ones,
										*   located between left shift and Y.
										*   Produces GRAVE ACCENT and TILDE in a
										*   US or UK Mac layout, REVERSE SOLIDUS
										*   (backslash) and VERTICAL LINE in a
										*   US or UK Windows layout, and
										*   LESS-THAN SIGN and GREATER-THAN SIGN
										*   in a Swiss German, German, or French
										*   layout. */
		APPLICATION = 101, /**< windows contextual menu, compose */
		POWER = 102, /**< The USB document says this is a status flag,
							   *   not a physical key - but some Mac keyboards
							   *   do have a power key. */
		NUM_EQUALS = 103,
		F13 = 104,
		F14 = 105,
		F15 = 106,
		F16 = 107,
		F17 = 108,
		F18 = 109,
		F19 = 110,
		F20 = 111,
		F21 = 112,
		F22 = 113,
		F23 = 114,
		F24 = 115,
		EXECUTE = 116,
		HELP = 117,
		MENU = 118,
		SELECT = 119,
		STOP = 120,
		AGAIN = 121,   /**< redo */
		UNDO = 122,
		CUT = 123,
		COPY = 124,
		PASTE = 125,
		FIND = 126,
		MUTE = 127,
		VOLUMEUP = 128,
		VOLUMEDOWN = 129,
		NUM_COMMA = 133,
		//SDL_SCANCODE_KP_EQUALSAS400 = 134,

		INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
							   footnotes in USB doc */
		INTERNATIONAL2 = 136,
		INTERNATIONAL3 = 137, /**< Yen */
		INTERNATIONAL4 = 138,
		INTERNATIONAL5 = 139,
		INTERNATIONAL6 = 140,
		INTERNATIONAL7 = 141,
		INTERNATIONAL8 = 142,
		INTERNATIONAL9 = 143,
		LANG1 = 144, /**< Hangul/English toggle */
		LANG2 = 145, /**< Hanja conversion */
		LANG3 = 146, /**< Katakana */
		LANG4 = 147, /**< Hiragana */
		LANG5 = 148, /**< Zenkaku/Hankaku */
		LANG6 = 149, /**< reserved */
		LANG7 = 150, /**< reserved */
		LANG8 = 151, /**< reserved */
		LANG9 = 152, /**< reserved */

		SDL_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
		SDL_SCANCODE_SYSREQ = 154,
		SDL_SCANCODE_CANCEL = 155,
		SDL_SCANCODE_CLEAR = 156,
		SDL_SCANCODE_PRIOR = 157,
		SDL_SCANCODE_RETURN2 = 158,
		SDL_SCANCODE_SEPARATOR = 159,
		SDL_SCANCODE_OUT = 160,
		SDL_SCANCODE_OPER = 161,
		SDL_SCANCODE_CLEARAGAIN = 162,
		SDL_SCANCODE_CRSEL = 163,
		SDL_SCANCODE_EXSEL = 164,

		/*SDL_SCANCODE_KP_00 = 176,
		SDL_SCANCODE_KP_000 = 177,
		SDL_SCANCODE_THOUSANDSSEPARATOR = 178,
		SDL_SCANCODE_DECIMALSEPARATOR = 179,
		SDL_SCANCODE_CURRENCYUNIT = 180,
		SDL_SCANCODE_CURRENCYSUBUNIT = 181,
		SDL_SCANCODE_KP_LEFTPAREN = 182,
		SDL_SCANCODE_KP_RIGHTPAREN = 183,
		SDL_SCANCODE_KP_LEFTBRACE = 184,
		SDL_SCANCODE_KP_RIGHTBRACE = 185,
		SDL_SCANCODE_KP_TAB = 186,
		SDL_SCANCODE_KP_BACKSPACE = 187,
		SDL_SCANCODE_KP_A = 188,
		SDL_SCANCODE_KP_B = 189,
		SDL_SCANCODE_KP_C = 190,
		SDL_SCANCODE_KP_D = 191,
		SDL_SCANCODE_KP_E = 192,
		SDL_SCANCODE_KP_F = 193,
		SDL_SCANCODE_KP_XOR = 194,
		SDL_SCANCODE_KP_POWER = 195,
		SDL_SCANCODE_KP_PERCENT = 196,
		SDL_SCANCODE_KP_LESS = 197,
		SDL_SCANCODE_KP_GREATER = 198,
		SDL_SCANCODE_KP_AMPERSAND = 199,
		SDL_SCANCODE_KP_DBLAMPERSAND = 200,
		SDL_SCANCODE_KP_VERTICALBAR = 201,
		SDL_SCANCODE_KP_DBLVERTICALBAR = 202,
		SDL_SCANCODE_KP_COLON = 203,
		SDL_SCANCODE_KP_HASH = 204,
		SDL_SCANCODE_KP_SPACE = 205,
		SDL_SCANCODE_KP_AT = 206,
		SDL_SCANCODE_KP_EXCLAM = 207,
		SDL_SCANCODE_KP_MEMSTORE = 208,
		SDL_SCANCODE_KP_MEMRECALL = 209,
		SDL_SCANCODE_KP_MEMCLEAR = 210,
		SDL_SCANCODE_KP_MEMADD = 211,
		SDL_SCANCODE_KP_MEMSUBTRACT = 212,
		SDL_SCANCODE_KP_MEMMULTIPLY = 213,
		SDL_SCANCODE_KP_MEMDIVIDE = 214,
		SDL_SCANCODE_KP_PLUSMINUS = 215,
		SDL_SCANCODE_KP_CLEAR = 216,
		SDL_SCANCODE_KP_CLEARENTRY = 217,
		SDL_SCANCODE_KP_BINARY = 218,
		SDL_SCANCODE_KP_OCTAL = 219,
		SDL_SCANCODE_KP_DECIMAL = 220,
		SDL_SCANCODE_KP_HEXADECIMAL = 221,*/

		LCTRL = 224,
		LSHIFT = 225,
		LALT = 226, /**< alt, option */
		LGUI = 227, /**< windows, command (apple), meta */
		RCTRL = 228,
		RSHIFT = 229,
		RALT = 230, /**< alt gr, option */
		RGUI = 231, /**< windows, command (apple), meta */

		_COUNT = 256
	};
}
