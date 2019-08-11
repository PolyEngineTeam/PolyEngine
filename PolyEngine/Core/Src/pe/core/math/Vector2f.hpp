#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>

namespace Poly 
{
	class Vector2i;

	/// <summary>Class representing 2D float vector.</summary>
	class CORE_DLLEXPORT Vector2f final : public BaseObjectLiteralType<>
	{
	public:
		static const Vector2f ZERO;
		static const Vector2f ONE;
		static const Vector2f UNIT_X;
		static const Vector2f UNIT_Y;

		/// <summary>Creates zero vector.</summary>
		constexpr Vector2f() : X(0.f), Y(0.f) {}

		/// <summary>Creates vector from float values.</summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		constexpr Vector2f(float x, float y) : X(x), Y(y) {}

		inline Vector2f(const Vector2f& rhs) : X(rhs.X), Y(rhs.Y) {}
		inline Vector2f& operator=(const Vector2f& rhs) { X = rhs.X; Y = rhs.Y; return *this; }

		/// <summary>Creates Vector2i based on this vector.</summary>
		/// <returns>Converted vector.</returns>
		Vector2i ToVector2i() const;
		explicit operator Vector2i() const;

		/// <summary>Negation operator</summary>
		inline Vector2f operator-() const { return Vector2f(-X, -Y); }

		// Comparisons
		bool operator==(const Vector2f& rhs) const;
		inline bool operator!=(const Vector2f& rhs) const { return !(*this == rhs); }

		// Operators with other vectors
		Vector2f operator+(const Vector2f& rhs) const;
		Vector2f operator-(const Vector2f& rhs) const;
		Vector2f& operator+=(const Vector2f& rhs);
		Vector2f& operator-=(const Vector2f& rhs);

		Vector2f operator*(const Vector2f& rhs) const;
		Vector2f operator/(const Vector2f& rhs) const;
		Vector2f& operator*=(const Vector2f& rhs);
		Vector2f& operator/=(const Vector2f& rhs);

		// Operators with floats
		Vector2f operator*(float rhs) const;
		Vector2f operator/(float rhs) const;
		Vector2f& operator*=(float rhs);
		Vector2f& operator/=(float rhs);

		/// <summary>Returns length of the vector.</summary>
		/// <returns>Length of the vector.</returns>
		float Length() const;

		/// <summary>Returns square length of the vector.</summary>
		/// <returns>Square length of the vector.</returns>
		float LengthSquared() const;

		/// <summary>Calculates dot product of two vectors.</summary>
		/// <param name="rhs">The other vector.</param>
		/// <returns>Dot product of the two vectors.</returns>
		float Dot(const Vector2f& rhs) const;

		/// <summary>Calculates cross product of two vectors.</summary>
		/// <param name="rhs">The other vector (right).</param>
		/// <returns>Cross product of the two vectors.</returns>
		float Cross(const Vector2f& rhs) const;

		/// <summary>Normalizes the vector.</summary>
		/// <returns>Reference to itself.</returns>
		Vector2f& Normalize();

		/// <summary>Creates normalized vector.</summary>
		/// <returns>Normalized vector.</returns>
		Vector2f GetNormalized() const;

		static Vector2f Max(const Vector2f& v1, const Vector2f& v2);
		static Vector2f Min(const Vector2f& v1, const Vector2f& v2);
		static Vector2f Clamp(const Vector2f& v1, const Vector2f& min, const Vector2f& max);

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Vector2f& vec);

		// This structure allows to access vector elements by index or name.
		union 
		{
			std::array<float, 2> Data;
			struct { float X,Y; };
		};

	};

}
