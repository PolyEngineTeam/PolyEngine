#pragma once

#include <pmmintrin.h>

#include "Defines.hpp"
#include "BasicMath.hpp"

namespace Poly {

	class ALIGN_16 CORE_DLLEXPORT Vector : public BaseObject<>{
	public:
		static const Vector ZERO;
		static const Vector UNIT_X;
		static const Vector UNIT_Y;
		static const Vector UNIT_Z;

		// Constructors
		constexpr Vector() : X(0.f), Y(0.f), Z(0.f), W(1.f) {}
		constexpr Vector(float x, float y, float z) : X(x), Y(y), Z(z), W(1.f) {}
		inline Vector(const Vector& rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(rhs.W) {}
	  #if !DISABLE_SIMD
		inline Vector& operator=(const Vector& rhs) { SimdData = rhs.SimdData; return *this; }
	  #else
		inline Vector& operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; W = rhs.W; return *this; }
	  #endif

		// Negation
		inline Vector operator-() const { return Vector(-X, -Y, -Z); }

		// Comparisons
		bool operator==(const Vector& rhs) const;
		inline bool operator!=(const Vector& rhs) const { return !(*this == rhs); }

		// Operators with other vectors
		Vector operator+(const Vector& rhs) const;
		Vector operator-(const Vector& rhs) const;
		Vector& operator+=(const Vector& rhs);
		Vector& operator-=(const Vector& rhs);

		// Operators with floats
		Vector operator*(float rhs) const;
		Vector operator/(float rhs) const;
		Vector& operator*=(float rhs);
		Vector& operator/=(float rhs);

		// Lengths
		float Length() const;
		float Length2() const;

		// Dot and cross products
		float Dot(const Vector& rhs) const;
		Vector Cross(const Vector& rhs) const;

		// Normalization
		Vector& Normalize();
		Vector GetNormalized() const;

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Vector& vec);

		// This structure allows to access vector elements by index or name.
		union {
	  #if !DISABLE_SIMD
		  __m128 SimdData;
	  #endif
		  float Data[4];
		  struct {
			float X,Y,Z,W;
		  };
		};

	  private:
	  #if !DISABLE_SIMD
		inline Vector(__m128 simd) : SimdData(simd) { W = 1.0f; }
	  #endif
	};

}
