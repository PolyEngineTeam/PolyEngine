#pragma once

#include <pmmintrin.h>

#include "Defines.hpp"
#include "BasicMath.hpp"

namespace Poly {

#if DISABLE_SIMD 
	typedef float Scalar;
#else
	class ALIGN_16 DLLEXPORT Scalar : public BaseObject<>
	{
	public:
		// create from float, implicit
		inline Scalar(float val = 0) { _mm_load_ps1(&val); }
		inline Scalar(__m128 data) : SimdData(data) {}
		inline Scalar(const Scalar& rhs) : SimdData(rhs.SimdData) {}
		// implicit cast to float
		inline operator float() const { return Value; }
		// Get simd data from scalar
		inline __m128 GetSimdData() { return SimdData; }

		//operators
		// Negation
		inline Scalar operator-() const { return Scalar(-Value); }

		inline Scalar& operator=(const Scalar& rhs) { SimdData = rhs.SimdData; return *this; }

		// Comparisons
		inline bool operator==(const Scalar& rhs) const { return Cmpf(Value, rhs.Value); }
		inline bool operator!=(const Scalar& rhs) const { return !(*this == rhs); }
		inline bool operator>(const Scalar& rhs) const { return Value > rhs.Value; }
		inline bool operator>=(const Scalar& rhs) const { return Value >= rhs.Value; }
		inline bool operator<(const Scalar& rhs) const { return Value < rhs.Value; }
		inline bool operator<=(const Scalar& rhs) const { return Value <= rhs.Value; }

		// Sum and subtraction of two scalars
		inline Scalar operator+(const Scalar& rhs) const { return Scalar(_mm_add_ps(SimdData, rhs.SimdData)); }
		inline Scalar operator-(const Scalar& rhs) const { return Scalar(_mm_sub_ps(SimdData, rhs.SimdData)); }
		inline Scalar& operator+=(const Scalar& rhs) { SimdData = _mm_add_ps(SimdData, rhs.SimdData); return *this; }
		inline Scalar& operator-=(const Scalar& rhs) { SimdData = _mm_sub_ps(SimdData, rhs.SimdData); return *this; }

		// Multiply and divison of two scalars
		inline Scalar operator*(const Scalar& rhs) const { return Scalar(_mm_mul_ps(SimdData, rhs.SimdData)); }
		inline Scalar operator/(const Scalar& rhs) const { return Scalar(_mm_div_ps(SimdData, rhs.SimdData)); }
		inline Scalar& operator*=(const Scalar& rhs) { SimdData = _mm_mul_ps(SimdData, rhs.SimdData); return *this; }
		inline Scalar& operator/=(const Scalar& rhs) { SimdData = _mm_div_ps(SimdData, rhs.SimdData); return *this; }

		// Conversion to stream
		friend std::ostream& operator<< (std::ostream& stream, const Scalar& scalar) { return stream << scalar.Value; }
	private:
		union
		{
			__m128 SimdData;
			float Data[4];
			float Value;
		};
	};
#endif

}