#include <pe/core/CorePCH.hpp>

#include <pe/core/math/Vector2i.hpp>
#include <pe/core/math/Vector2f.hpp>

using namespace ::pe::core::math;

const Vector2i Vector2i::ZERO = Vector2i(0, 0);
const Vector2i Vector2i::UNIT_X = Vector2i(1, 0);
const Vector2i Vector2i::UNIT_Y = Vector2i(0, 1);

//------------------------------------------------------------------------------
bool Vector2i::operator==(const Vector2i& rhs) const
{
	return X == rhs.X && Y == rhs.Y;
}

//------------------------------------------------------------------------------
Vector2i Vector2i::operator+(const Vector2i& rhs) const
{
	return Vector2i(X + rhs.X, Y + rhs.Y);
}

//------------------------------------------------------------------------------
Vector2i Vector2i::operator-(const Vector2i& rhs) const
{
	return Vector2i(X - rhs.X, Y - rhs.Y);
}

//------------------------------------------------------------------------------
Vector2i& Vector2i::operator+=(const Vector2i& rhs)
{
	X += rhs.X; Y += rhs.Y;
	return *this;
}

//------------------------------------------------------------------------------
Vector2i& Vector2i::operator-=(const Vector2i& rhs)
{
	X -= rhs.X; Y -= rhs.Y;
	return *this;
}

//------------------------------------------------------------------------------
Vector2i Vector2i::operator*(VectorIntType rhs) const
{
	return Vector2i(X * rhs, Y * rhs);
}

//------------------------------------------------------------------------------
Vector2i Vector2i::operator/(VectorIntType rhs) const
{
	return Vector2i(X / rhs, Y / rhs);
}

//------------------------------------------------------------------------------
Vector2i& Vector2i::operator*=(VectorIntType rhs)
{
	X *= rhs; Y *= rhs;
	return *this;
}

//------------------------------------------------------------------------------
Vector2i& Vector2i::operator/=(VectorIntType rhs)
{
	X /= rhs; Y /= rhs;
	return *this;
}

//------------------------------------------------------------------------------
Vector2f Vector2i::ToVector2f() const
{
	return Vector2f(static_cast<float>(X), static_cast<float>(Y));
}

Vector2i::operator Vector2f() const
{
	return ToVector2f();
}

//------------------------------------------------------------------------------
namespace pe::core::math
{
	//------------------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& stream, const Vector2i& vec)
	{
		return stream << "Vec[ " << vec.X << " " << vec.Y << " ]";
	}
}
