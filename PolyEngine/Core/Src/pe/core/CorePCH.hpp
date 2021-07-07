#pragma once

#include <pe/Defines.hpp>

// rapidjson
SILENCE_GCC_WARNING(-Wclass-memaccess, "Rapidjson has no release containing fix to this issue (01.09.18).")
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
UNSILENCE_GCC_WARNING()

// ICU
//#include <unicode/unorm2.h>
#include <unicode/normalizer2.h>
#include <unicode/translit.h>
//#include <unicode/ustring.h>
//#include <unicode/utrans.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>