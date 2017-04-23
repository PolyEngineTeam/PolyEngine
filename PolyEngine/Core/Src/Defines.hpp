#pragma once

// STL
// This is only legal place for STL includes
#include <ostream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <type_traits>
#include <limits>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// TODO suport more platforms.
#if !defined(_WIN32)
	#error [ERROR] Unsupported platform! You are trying to compile for unsupported platform. This won't work.'
#endif

// ALIGN_16
#if defined(_WIN32)
	#define ALIGN_16 __declspec(align(16))
#elif defined(_LINUX) || defined(_MAC)
	#define ALIGN_16 __attribute__ ((aligned(16)))
#endif

// CORE_DLLEXPORT
#if defined(_WIN32)
	#if defined(_WINDLL) && defined(_CORE)
		#define CORE_DLLEXPORT __declspec(dllexport)
	#else
		#define CORE_DLLEXPORT __declspec(dllimport)
	#endif

	#if defined(_WINDLL) && defined(_ENGINE)
		#define ENGINE_DLLEXPORT __declspec(dllexport)
	#else
		#define ENGINE_DLLEXPORT __declspec(dllimport)
	#endif

	#if defined(_WINDLL) && defined(_GAME)
		#define GAME_DLLEXPORT __declspec(dllexport)
	#else
		#define GAME_DLLEXPORT __declspec(dllimport)
	#endif
#else
	#define CORE_DLLEXPORT
	#define ENGINE_DLLEXPORT
	#define GAME_DLLEXPORT
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

// Utilities
#define BIT(x) 1u<<x

#include "BaseObject.hpp"