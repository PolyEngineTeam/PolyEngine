#pragma once

#include <pe/Defines.hpp>

namespace Poly {

	constexpr u8 HexCharToValue(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		else if (c >= 'a' && c <= 'f')
			return 0x0A + c - 'a';
		else if (c >= 'A' && c <= 'F')
			return 0x0A + c - 'A';
		else
			HEAVY_ASSERTE(false, "Not a hex value");
		return 0xF0;
	}

	constexpr bool IsValidHex(char c)
	{ 
		return (c >= '0' && c <= '9') 
			|| (c >= 'a' && c <= 'f') 
			|| (c >= 'A' && c <= 'F');
	}
}