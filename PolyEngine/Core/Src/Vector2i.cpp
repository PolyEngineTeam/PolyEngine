#include "CorePCH.hpp"

#include "Vector2i.hpp"

using namespace Poly;

const Vector2i Vector2i::ZERO = Vector2i(0, 0);
const Vector2i Vector2i::UNIT_X = Vector2i(1, 0);
const Vector2i Vector2i::UNIT_Y = Vector2i(0, 1);

//------------------------------------------------------------------------------
bool Vector2i::operator==(const Vector2i& rhs) const
{
	return Cmpf(X, rhs.X) && Cmpf(Y, rhs.Y);
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
Vector2i Vector2i::operator*(int rhs) const
{
	return Vector2i(X*rhs, Y*rhs);
}

//------------------------------------------------------------------------------
Vector2i Vector2i::operator/(int rhs) const
{
	return Vector2i(X / rhs, Y / rhs);
}

//------------------------------------------------------------------------------
Vector2i& Vector2i::operator*=(int rhs)
{
	X *= rhs; Y *= rhs;
	return *this;
}

//------------------------------------------------------------------------------
Vector2i& Vector2i::operator/=(int rhs)
{
	X /= rhs; Y /= rhs;
	return *this;
}

//------------------------------------------------------------------------------
//TODO maybe inline this?
float Vector2i::Length() const { return std::sqrt(LengthSquared()); }
int Vector2i::LengthSquared() const { return Dot(*this); }

//------------------------------------------------------------------------------
int Vector2i::Dot(const Vector2i& rhs) const
{
	return X * rhs.X + Y * rhs.Y;
}

//------------------------------------------------------------------------------
int Vector2i::Cross(const Vector2i& rhs) const
{
	return X * rhs.Y - Y * rhs.X;
}

//------------------------------------------------------------------------------
Vector2i& Vector2i::Normalize()
{
	*this /= Length();
	return *this;
}

//------------------------------------------------------------------------------
Vector2i Vector2i::GetNormalized() const
{
	return *this / Length();
}

namespace Poly
{
	//------------------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& stream, const Vector2i& vec)
	{
		return stream << "Vec[ " << vec.X << " " << vec.Y << " ]";
	}
}
