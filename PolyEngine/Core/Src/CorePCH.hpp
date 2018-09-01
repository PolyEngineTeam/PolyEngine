#pragma once

#include "Defines.hpp"

// rapidjson
SILENCE_GCC_WARNING(-Wclass-memaccess)
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
UNSILENCE_GCC_WARNING()