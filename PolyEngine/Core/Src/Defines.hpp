#pragma once

// STL
// This is only legal place for STL includes
#include <ostream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <type_traits>
#include <limits>
#include <array>
#include <cstddef>
#include <memory>
#include <algorithm>
#include <iostream>
#include <string>

// Custom type names
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef unsigned int uint;

// stupid warning in MSVC about template specialization exporting, according to https://msdn.microsoft.com/en-US/library/esew7y1w.aspx it can be ignored
#if defined(_WIN32)
	#pragma warning(disable: 4251)
	#pragma warning(disable: 4275)
	#define SILENCE_MSVC_WARNING(warning_id, reason) \
		__pragma(warning(push))                      \
		__pragma(warning(disable:warning_id))
	#define UNSILENCE_MSVC_WARNING() __pragma(warning(pop))
#else
	#define SILENCE_MSVC_WARNING(unused_warning_id, unused_reason)
	#define UNSILENCE_MSVC_WARNING()
#endif

#ifdef __GNUC__ //todo(vuko): add a reason parameter to the warning-silencing macro
	#define IMPL_SAVE_WARNING_SETTINGS _Pragma("GCC diagnostic push")
	#define IMPL_SILENCE_WARNING(w) _Pragma(#w)
	#define SILENCE_GCC_WARNING(w, reason)             \
		IMPL_SAVE_WARNING_SETTINGS                     \
		IMPL_SILENCE_WARNING(GCC diagnostic ignored #w)
	#define UNSILENCE_GCC_WARNING() _Pragma("GCC diagnostic pop")
#else
	#define SILENCE_GCC_WARNING(unused_w, unused_reason)
	#define UNSILENCE_GCC_WARNING()
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ALIGN_16
#if defined(_WIN32)
	#define ALIGN_16 __declspec(align(16))
#elif defined(__GNUC__) || defined(__clang__)
	#define ALIGN_16 __attribute__ ((aligned(16)))
#else
	#define ALIGN_16 alignas(16)
#endif

#if defined(__GNUC__) && !defined(offsetof)
	#define offsetof(type, member) __builtin_offsetof(type, member)
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

	#if defined(_WINDLL) && defined(_DEVICE)
		#define DEVICE_DLLEXPORT __declspec(dllexport)
	#else
		#define DEVICE_DLLEXPORT __declspec(dllimport)
	#endif
#elif defined(__GNUC__) || defined(__clang__)
	//NOTE(vuko): dllexport and dllimport are the same as far as GCC and Clang are concerned
	#define CORE_DLLEXPORT __attribute__ ((visibility ("default")))
	#define ENGINE_DLLEXPORT __attribute__ ((visibility ("default")))
	#define GAME_DLLEXPORT __attribute__ ((visibility ("default")))
	#define DEVICE_DLLEXPORT __attribute__ ((visibility ("default")))
#else
	#define CORE_DLLEXPORT
	#define ENGINE_DLLEXPORT
	#define GAME_DLLEXPORT
	#define DEVICE_DLLEXPORT
#endif

#if defined(_WIN32)
#define POLY_STDCALL __stdcall
#else
#define POLY_STDCALL
#endif

#ifndef DISABLE_SIMD
#define DISABLE_SIMD false
#endif

// limits
constexpr auto MAX_FLOAT = (std::numeric_limits<float>::max)(); //the parentheses are there to prevent WinAPI macros from breaking this
constexpr auto MIN_FLOAT = (std::numeric_limits<float>::min)();

// Assertions
#define ASSERTE(expr, msg) assert((expr) && static_cast<const char*>(#msg)) //note(muniu): enabled in all builds except Release
#define HEAVY_ASSERTE(expr, msg) assert((expr) && static_cast<const char*>(#msg)) //todo(muniu): enabled only in Debug
#define STATIC_ASSERTE(expr, msg) static_assert(expr, msg)

// `Unreachable code` compiler intrinsic
#if defined(FINAL) && defined(_WIN32)
#define UNREACHABLE() __assume(0);
#elif defined(FINAL) && (defined(__GNUC__) || defined(__clang__))
#define UNREACHABLE() __builtin_unreachable();
#else
#define UNREACHABLE() do { assert("Unreachable code reached!" && false); throw; } while (false) //note(vuko): `throw` is needed to terminate the control path (assert is sadly not enough)
#endif

// Utilities
#define BIT(x) (1u<<x)

#define UNUSED(expr) do { (void)(expr); } while (false)

#include "BaseObject.hpp"
