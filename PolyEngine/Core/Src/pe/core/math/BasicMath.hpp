#pragma once

#include <pe/Defines.hpp>

namespace pe::core::math
{

	//cmath std::abs is not constexpr
	static constexpr float abs(float n) { return n < 0.0f ? -n : n; }
	static constexpr double abs(double n) { return n < 0.0 ? -n : n; }
	static constexpr long double abs(long double n) { return n < 0.0l ? -n : n; }

	//WORKAROUND(vuko): MSVC std::max{} is not constexpr
	template<class T> static constexpr const T& max3(const T& a, const T& b, const T& c) { return (std::max)((std::max)(a, b), c); };

	/// <summary>Default epsilon value for <see cref="Cmpf"/>. Determines precision of comparison.</summary>
	constexpr float CMPF_EPS = 1e-4f;

	/// <summary>Checks if given float value is in usable range (not Inf, NaN, etc.).</summary>
	/// <param name="val"></param>
	/// <returns>True if float is valid, false otherwise.</returns>
	inline bool IsValidf(float val) { return std::isfinite(val); }


	/// <summary>Checks if given single-precision floating-point numbers are roughly equal</summary>
	/// <param name="epsilon">The smallest unit of difference that can be ignored (by default == CMPF_EPS).</param>
	/// <param name="handle_infinities">Determines if we should care about infinite values (by default == false).</param>
	/// <returns>Rough equality test result</returns>
#ifdef _MSC_VER //WORKAROUND(vuko): MSVC does not support extended constexpr yet
	constexpr bool Cmpf(const float a, const float b, const float epsilon = CMPF_EPS, const bool handle_infinities = false) {
		return (handle_infinities && a == b) ||
			((abs(a - b) < MIN_FLOAT || abs(a) == 0.0f || abs(b) == 0.0f) && (abs(a - b) <= epsilon)) ||
			(!(abs(a - b) < MIN_FLOAT || abs(a) == 0.0f || abs(b) == 0.0f) && (abs(a - b) < (std::max)(abs(a), abs(b)) * epsilon));
	}
#else
	constexpr bool Cmpf(const float a, const float b, const float epsilon = CMPF_EPS, const bool handle_infinities = false) {
		if (handle_infinities && a == b) {
			return true;
		}

		const float absA = abs(a);
		const float absB = abs(b);
		const float diff = abs(a - b);
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
		return abs(a - b) <= (epsilon * max3(1.0f, abs(a), abs(b)));
	}

	/**
	 * Simplified version of @see[Cmpf] optimized for comparisons near zero
	 * Uses absolute difference
	 */
	inline constexpr bool CmpfNearZero(const float a, const float b, const float epsilon = CMPF_EPS) {
		return abs(a - b) < epsilon; //absolute
	}

	/**
	 * Simplified version of @see[Cmpf] optimized for comparisons of big numbers
	 * Uses relative difference
	 */
	inline constexpr bool CmpfBig(const float a, const float b, const float epsilon = CMPF_EPS) {
		return abs(a - b) < epsilon * (std::max)(abs(a), abs(b)); //relative
	}

	// Range checking functions
	template<typename T>
	constexpr bool IsInRange(const T& a, const T& min, const T& max) {
		HEAVY_ASSERTE(min <= max, "Min is larger than max when calling IsInRange!");
		return a <= max && a >= min;
	}

	// Basic math operations
	template <typename T> inline constexpr T Abs(T val) {
		return abs(val);
	}

	template <typename T> inline T Clamp(const T& val, const T& min, const T& max) {
		HEAVY_ASSERTE(min <= max, "Min is larger than max when calling clamp!");
		return val < min ? min : (val > max ? max : val);
	}

	template <typename T> inline T Lerp(const T& val1, const T& val2, float t) {
		return (T)(val1 * (1.0f - t) + val2 * t);
	}

	template <typename T> inline T SmoothStep(const T& edge1, const T& edge2, const T& x) {
		T t = Clamp((x - edge1) / (edge2 - edge1), 0.0f, 1.0f);
		return (T)(t * t * (3.0f - 2.0f * t));
	}
}
