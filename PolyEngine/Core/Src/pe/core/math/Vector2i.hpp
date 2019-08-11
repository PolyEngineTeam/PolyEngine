#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>

namespace Poly
{
	class Vector2f;
	using VectorIntType = int32_t;

	/// <summary>Class representing 2D integer vector.</summary>
	class CORE_DLLEXPORT Vector2i final : public BaseObjectLiteralType<>
	{
	public:
		static const Vector2i ZERO;
		static const Vector2i UNIT_X;
		static const Vector2i UNIT_Y;

		/// <summary>Creates zero vector.</summary>
		constexpr Vector2i() : X(0), Y(0) {}

		/// <summary>Creates vector from int values.</summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		constexpr Vector2i(VectorIntType x, VectorIntType y) : X(x), Y(y) {}

		inline Vector2i(const Vector2i& rhs) : X(rhs.X), Y(rhs.Y) {}
		inline Vector2i& operator=(const Vector2i& rhs) { X = rhs.X; Y = rhs.Y; return *this; }

		/// <summary>Creates Vector2f based on this vector.</summary>
		/// <returns>Converted vector.</returns>
		Vector2f ToVector2f() const;
		explicit operator Vector2f() const;

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

		// Operators with ints
		Vector2i operator*(VectorIntType rhs) const;
		Vector2i operator/(VectorIntType rhs) const;
		Vector2i& operator*=(VectorIntType rhs);
		Vector2i& operator/=(VectorIntType rhs);

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Vector2i& vec);

		// This structure allows to access vector elements by index or name.
		union
		{
			std::array<VectorIntType, 2> Data;
			struct { VectorIntType X, Y; };
		};

	};

}
