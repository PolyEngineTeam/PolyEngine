#include "CorePCH.hpp"

#include "Color.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Color::Color() : R(0), G(0), B(0), A(1)
{
}

//------------------------------------------------------------------------------
Color::Color(float r, float g, float b, float a)
	: R(r), G(g), B(b), A(a)
{
}

//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& stream, const Color& color)
{
	return stream << "Color[ " << color.R << " " << color.G << " " << color.B << " " << color.A << " ]";
}
