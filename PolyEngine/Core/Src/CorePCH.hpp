#pragma once

#include "Defines.hpp"

SILENCE_GCC_WARNING(-Wclass-memaccess, "Rapidjson has no release containing fix to this issue (01.09.18)")
// rapidjson
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
UNSILENCE_GCC_WARNING()