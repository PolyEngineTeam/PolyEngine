#pragma once

// STL
// This is only legal place for STL includes
#include <ostream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <type_traits>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// TODO suport more platforms.
#if !defined(_WIN32)
	#error [ERROR] Unsupported platform! You are rying to compile for unsupported platform. This won't work.'
#endif

// ALIGN_16
#if defined(_WIN32)
	#define ALIGN_16 __declspec(align(16))
#elif defined(_LINUX) || defined(_MAC)
	#define ALIGN_16 __attribute__ ((aligned(16)))
#endif

// DLLEXPORT
#if defined(_WIN32)
	#if defined(_WINDLL)
		#define DLLEXPORT __declspec(dllexport)
	#else
		#define DLLEXPORT __declspec(dllimport)
	#endif
#else
	#define DLLEXPORT
#endif

#ifndef DISABLE_SIMD
#define DISABLE_SIMD false
#endif

// limits
#define MAX_FLOAT std::numeric_limits<float>::max()
#define MIN_FLOAT std::numeric_limits<float>::min()

// Assertions
#define ASSERTE(expr, msg) assert(expr && #msg)
#define HEAVY_ASSERTE(expr, msg) assert(expr && #msg)
#define STATIC_ASSERTE(expr, msg) static_assert(expr, msg)

#include "BaseObject.hpp"