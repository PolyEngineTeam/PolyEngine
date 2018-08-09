#pragma once

#include "Defines.hpp"

namespace Poly {

	CORE_DLLEXPORT u8 HexCharToValue(char c);
	inline bool IsValidHex(char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
}