#pragma once

#include "Defines.hpp"
#include "Vector.hpp"

namespace Poly
{
	/// Class representing 3D float vector.
	class CORE_DLLEXPORT Vector3f final : public BaseObjectLiteralType<>
	{
	public:
		/// Creates zero vector.
		explicit constexpr Vector3f() : X(0.f), Y(0.f), Z(0.f) {}

		/// Creates vector from float values.
		explicit constexpr Vector3f(float x, float y, float z) : X(x), Y(y), Z(z) {}

		Vector3f(const Vector3f& rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z) {}

		/// Helper method for getting a copy of Vector3f as Vector.
		Vector GetVector() const { return Vector(X, Y, Z); }

		// This structure allows to access vector elements by index or name.
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
	};
}