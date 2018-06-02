#include "CorePCH.hpp"

#include "Math/Vector2f.hpp"
#include "Math/Vector2i.hpp"

using namespace Poly;

const Vector2f Vector2f::ZERO = Vector2f(0.f, 0.f);
const Vector2f Vector2f::UNIT_X = Vector2f(1.f, 0.f);
const Vector2f Vector2f::UNIT_Y = Vector2f(0.f, 1.f);

//------------------------------------------------------------------------------
bool Vector2f::operator==(const Vector2f& rhs) const
{
	return Cmpf(X, rhs.X) && Cmpf(Y, rhs.Y);
}

//------------------------------------------------------------------------------
Vector2f Vector2f::operator+(const Vector2f& rhs) const
{
	return Vector2f(X + rhs.X, Y + rhs.Y);
}

//------------------------------------------------------------------------------
Vector2f Vector2f::operator-(const Vector2f& rhs) const
{
	return Vector2f(X - rhs.X, Y - rhs.Y);
}

//------------------------------------------------------------------------------
Vector2f& Vector2f::operator+=(const Vector2f& rhs)
{
	X += rhs.X; Y += rhs.Y;
	return *this;
}

//------------------------------------------------------------------------------
Vector2f& Vector2f::operator-=(const Vector2f& rhs)
{
	X -= rhs.X; Y -= rhs.Y;
	return *this;
}

//------------------------------------------------------------------------------
Vector2f Vector2f::operator*(float rhs) const
{
	return Vector2f(X*rhs, Y*rhs);
}

//------------------------------------------------------------------------------
Vector2f Vector2f::operator/(float rhs) const
{
	return Vector2f(X / rhs, Y / rhs);
}

//------------------------------------------------------------------------------
Vector2f& Vector2f::operator*=(float rhs)
{
	X *= rhs; Y *= rhs;
	return *this;
}

//------------------------------------------------------------------------------
Vector2f& Vector2f::operator/=(float rhs)
{
	X /= rhs; Y /= rhs;
	return *this;
}

//------------------------------------------------------------------------------
//TODO maybe inline this?
float Vector2f::Length() const { return std::sqrt(LengthSquared()); }
float Vector2f::LengthSquared() const { return Dot(*this); }

//------------------------------------------------------------------------------
float Vector2f::Dot(const Vector2f& rhs) const
{
	return X * rhs.X + Y * rhs.Y;
}

//------------------------------------------------------------------------------
float Vector2f::Cross(const Vector2f& rhs) const
{
	return X * rhs.Y - Y * rhs.X;
}

//------------------------------------------------------------------------------
Vector2f& Vector2f::Normalize()
{
	*this /= Length();
	return *this;
}

//------------------------------------------------------------------------------
Vector2f Vector2f::GetNormalized() const
{
	return *this / Length();
}

Vector2i Vector2f::ToVector2i() const
{
	return Vector2i(static_cast<int>(X), static_cast<int>(Y));
}

Vector2f::operator Vector2i() const
{
	return ToVector2i();
}

Vector2f Vector2f::Max(const Vector2f& v1, const Vector2f& v2)
{
	return Vector2f(std::max(v1.X, v2.X), std::max(v1.Y, v2.Y));
}

Vector2f Vector2f::Min(const Vector2f& v1, const Vector2f& v2)
{
	return Vector2f(std::min(v1.X, v2.X), std::min(v1.Y, v2.Y));
}

Vector2f Vector2f::Clamp(const Vector2f& v1, const Vector2f& min, const Vector2f& max)
{
	return Vector2f(::Clamp(v1.X, min.X, max.X), ::Clamp(v1.Y, min.Y, max.Y));
}

namespace Poly
{
	//------------------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& stream, const Vector2f& vec)
	{
		return stream << "Vec[ " << vec.X << " " << vec.Y << " ]";
	}
}
