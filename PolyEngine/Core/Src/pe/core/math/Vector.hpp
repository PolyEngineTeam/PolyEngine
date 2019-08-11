#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>
#include <pe/core/math/Color.hpp>
#include <pmmintrin.h> //SIMD library

namespace Poly
{
	/// <summary>Class representing 3D vector that takes advantage of SIMD (if possible).</summary>
	class ALIGN_16 CORE_DLLEXPORT Vector final : public BaseObjectLiteralType<>
	{
	public:
		static const Vector ZERO;
		static const Vector ONE;
		static const Vector UNIT_X;
		static const Vector UNIT_Y;
		static const Vector UNIT_Z;

		/// <summary>Creates zero vector.</summary>
		constexpr Vector() : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {}

		/// <summary>Creates vector from float values.</summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="w">Default value is 1.0f.</param>
		constexpr Vector(float x, float y, float z, float w = 1.0f) : X(x), Y(y), Z(z), W(w) {}


		inline Vector(const Vector& rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(rhs.W) {}
		explicit inline Vector(const Color& rhs) : X(rhs.R), Y(rhs.G), Z(rhs.B), W(rhs.A) {}
	#if !DISABLE_SIMD
		inline Vector& operator=(const Vector& rhs) { SimdData = rhs.SimdData; return *this; }
	#else
		inline Vector& operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; W = rhs.W; return *this; }
	#endif

		/// <summary>Negation operator</summary>
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
		Vector operator+(float rhs) const;
		Vector operator-(float rhs) const;
		Vector operator*(float rhs) const;
		Vector operator/(float rhs) const;
		Vector& operator+=(float rhs);
		Vector& operator-=(float rhs);
		Vector& operator*=(float rhs);
		Vector& operator/=(float rhs);

		/// <summary>Returns length of the vector.</summary>
		/// <returns>Length of the vector.</returns>
		float Length() const;

		/// <summary>Returns square length of the vector.</summary>
		/// <returns>Square length of the vector.</returns>
		float LengthSquared() const;

		/// <summary>Calculates dot product of two vectors.</summary>
		/// <param name="rhs">The other vector.</param>
		/// <returns>Dot product of the two vectors.</returns>
		float Dot(const Vector& rhs) const;

		/// <summary>Calculates cross product of two vectors.</summary>
		/// <param name="rhs">The other vector (right).</param>
		/// <returns>Cross product of the two vectors.</returns>
		Vector Cross(const Vector& rhs) const;

		/// <summary>Normalizes the vector.</summary>
		/// <returns>Reference to itself.</returns>
		Vector& Normalize();

		/// <summary>Creates normalized vector.</summary>
		/// <returns>Normalized vector.</returns>
		Vector GetNormalized() const;

		static Vector Max(const Vector& a, const Vector& b);
		static Vector Min(const Vector& a, const Vector& b);

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Vector& vec);

		// This structure allows to access vector elements by index or name.
		union {
		#if !DISABLE_SIMD
		  __m128 SimdData;
		#endif
		  alignas(16) std::array<float, 4> Data;
		  struct { float X,Y,Z,W; };
		};

	private:
	  #if !DISABLE_SIMD
		inline Vector(__m128 simd) : SimdData(simd) { W = 1.0f; }
	  #endif
	};

}
