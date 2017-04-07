#pragma once

#include "Defines.hpp"

#define IS_VALID(val) std::isfinite(val)

namespace Poly {

	// Float precission comparison constant
	constexpr float CMPF_EPS = 1e-4f;

	// Float comparison function
	constexpr bool Cmpf(float a, float b) {
		return (a - b >= 0 && a - b <= CMPF_EPS) || (a - b < 0 && a - b >= -CMPF_EPS);
	}

	// Range checking functions
	template<typename T>
	constexpr bool IsInRange(const T& a, const T& min, const T& max) {
		HEAVY_ASSERTE(min <= max, "Min is larger than max when calling IsInRange!");
		return a <= max && a >= min;
	}

	// Basic math operations
	template <typename T> inline T Abs(T val) { return std::abs(val); }
	template <typename T> inline void Swap(T& val1, T& val2) { T tmp = val2; val2 = val1; val1 = tmp; }
	template <typename T> inline T Clamp(const T& val, const T& min, const T& max) {
		HEAVY_ASSERTE(min <= max, "Min is larger than max when calling clamp!");
		return val < min ? min : (val > max ? max : val);
	}
	template <typename T> inline T Lerp(const T& val1, const T& val2, float t) {
		HEAVY_ASSERTE(t >= 0.f && t <= 1.f, "Value of t needs to be between 0 and 1.");
		return val1 * (1.0f - t) + val2 * t;
	}

}