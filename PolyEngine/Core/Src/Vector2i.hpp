#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"

namespace Poly
{
	/// <summary>Class representing 2D integer vector.</summary>
	class ALIGN_16 CORE_DLLEXPORT Vector2i : public BaseObject<>{
	public:
		static const Vector2i ZERO;
		static const Vector2i UNIT_X;
		static const Vector2i UNIT_Y;

		/// <summary>Creates zero vector.</summary>
		constexpr Vector2i() : X(0), Y(0) {}

		/// <summary>Creates vector from float values.</summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		constexpr Vector2i(int x, int y) : X(x), Y(y) {}


		inline Vector2i(const Vector2i& rhs) : X(rhs.X), Y(rhs.Y) {}
		inline Vector2i& operator=(const Vector2i& rhs) { X = rhs.X; Y = rhs.Y; return *this; }

		/// <summary>Negation operator</summary>
		inline Vector2i operator-() const { return Vector2i(-X, -Y); }

		// Comparisons
		bool operator==(const Vector2i& rhs) const;
		inline bool operator!=(const Vector2i& rhs) const { return !(*this == rhs); }

		// Operators with other vectors
		Vector2i operator+(const Vector2i& rhs) const;
		Vector2i operator-(const Vector2i& rhs) const;
		Vector2i& operator+=(const Vector2i& rhs);
		Vector2i& operator-=(const Vector2i& rhs);

		// Operators with floats
		Vector2i operator*(float rhs) const;
		Vector2i operator/(float rhs) const;
		Vector2i& operator*=(float rhs);
		Vector2i& operator/=(float rhs);

		/// <summary>Returns length of the vector.</summary>
		/// <returns>Length of the vector.</returns>
		float Length() const;

		/// <summary>Returns square length of the vector.</summary>
		/// <returns>Square length of the vector.</returns>
		float LengthSquared() const;

		/// <summary>Calculates dot product of two vectors.</summary>
		/// <param name="rhs">The other vector.</param>
		/// <returns>Dot product of the two vectors.</returns>
		float Dot(const Vector2i& rhs) const;

		/// <summary>Calculates cross product of two vectors.</summary>
		/// <param name="rhs">The other vector (right).</param>
		/// <returns>Cross product of the two vectors.</returns>
		float Cross(const Vector2i& rhs) const;

		/// <summary>Normalizes the vector.</summary>
		/// <returns>Reference to itself.</returns>
		Vector2i& Normalize();

		/// <summary>Creates normalized vector.</summary>
		/// <returns>Normalized vector.</returns>
		Vector2i GetNormalized() const;

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Vector2i& vec);

		// This structure allows to access vector elements by index or name.
		union
		{
			float Data[2];
			struct { float X,Y; };
		};

	private:
	};

}
