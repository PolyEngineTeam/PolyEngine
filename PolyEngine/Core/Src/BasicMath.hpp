#pragma once

#include "Defines.hpp"

#define IS_VALID(val) std::isfinite(val)

namespace Poly {

	//cmath std::abs is not constexpr
	static constexpr float abs(float n) { return n < 0.0f ? -n : n; }
	static constexpr double abs(double n) { return n < 0.0 ? -n : n; }
	static constexpr long double abs(long double n) { return n < 0.0l ? -n : n; }

	//WORKAROUND(vuko): MSVC std::max{} is not constexpr
	template<class T> static constexpr const T& max3(const T& a, const T& b, const T& c) { return (std::max)((std::max)(a, b), c); };

	/**
	 * Default epsilon value for @see[Cmpf]
	 * Determines precision of comparison
	 */
	constexpr float CMPF_EPS = 1e-4f;

	/**
	 *  Checks if given single-precision floating-point
	 *  numbers are roughly equal
	 *
	 *  @param[in] epsilon the smallest unit of difference that can be ignored
	 *  @param[in] handle_infinities determines if we should care about infinite values
	 *  @return Rough equality test result
	 */
#ifdef _MSC_VER //WORKAROUND(vuko): MSVC does not support extended constexpr yet
	constexpr bool Cmpf(const float a, const float b, const float epsilon = CMPF_EPS, const bool handle_infinities = false) {
	    return (handle_infinities && a == b) ||
	    ((Poly::abs(a - b) < MIN_FLOAT || Poly::abs(a) == 0.0f || Poly::abs(b) == 0.0f) && (Poly::abs(a - b) <= epsilon)) ||
	    (!(Poly::abs(a - b) < MIN_FLOAT || Poly::abs(a) == 0.0f || Poly::abs(b) == 0.0f) && (Poly::abs(a - b) < (std::max)(Poly::abs(a), Poly::abs(b)) * epsilon));
	}
#else
	constexpr bool Cmpf(const float a, const float b, const float epsilon = CMPF_EPS, const bool handle_infinities = false) {
		if (handle_infinities && a == b) {
			return true;
		}

		const float absA = Poly::abs(a);
		const float absB = Poly::abs(b);
		const float diff = Poly::abs(a - b);
		if (diff < MIN_FLOAT || absA == 0.0f || absB == 0.0f) {
			//a or b are extremely close to zero
			//absolute comparison
			return diff <= epsilon;
		}

		//relative comparison
		const float relative_factor = std::max(absA, absB);
		return diff < relative_factor * epsilon;
	}
#endif //_MSC_VER

	/**
	 * Fast version of @see[Cmpf] for when you don't need to handle all the cases
	 */
	inline constexpr bool CmpfFast(const float a, const float b, const float epsilon = CMPF_EPS) {
		return Poly::abs(a - b) <= (epsilon * Poly::max3(1.0f, Poly::abs(a), Poly::abs(b)));
	}

	/**
	 * Simplified version of @see[Cmpf] optimized for comparisons near zero
	 * Uses absolute difference
	 */
	inline constexpr bool CmpfNearZero(const float a, const float b, const float epsilon = CMPF_EPS) {
		return Poly::abs(a - b) < epsilon; //absolute
	}

	/**
	 * Simplified version of @see[Cmpf] optimized for comparisons of big numbers
	 * Uses relative difference
	 */
	inline constexpr bool CmpfBig(const float a, const float b, const float epsilon = CMPF_EPS) {
		return Poly::abs(a - b) < epsilon * (std::max)(Poly::abs(a), Poly::abs(b)); //relative
	}

	// Range checking functions
	template<typename T>
	constexpr bool IsInRange(const T& a, const T& min, const T& max) {
		HEAVY_ASSERTE(min <= max, "Min is larger than max when calling IsInRange!");
		return a <= max && a >= min;
	}

	// Basic math operations
	template <typename T> inline constexpr T Abs(T val) { return Poly::abs(val); }
	template <typename T> inline T Clamp(const T& val, const T& min, const T& max) {
		HEAVY_ASSERTE(min <= max, "Min is larger than max when calling clamp!");
		return val < min ? min : (val > max ? max : val);
	}
	template <typename T> inline T Lerp(const T& val1, const T& val2, float t) {
		HEAVY_ASSERTE(t >= 0.f && t <= 1.f, "Value of t needs to be between 0 and 1.");
		return val1 * (1.0f - t) + val2 * t;
	}

}
