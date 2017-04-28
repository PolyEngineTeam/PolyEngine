#include "CorePCH.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Color::Color() : R(0), G(0), B(0), A(1)
{
}

//------------------------------------------------------------------------------
Color::Color(float r, float g, float b, float a)
	: R(r), G(g), B(b), A(a)
{
	HEAVY_ASSERTE(IsInRange(R, 0.f, 1.f), "");
	HEAVY_ASSERTE(IsInRange(G, 0.f, 1.f), "");
	HEAVY_ASSERTE(IsInRange(B, 0.f, 1.f), "");
	HEAVY_ASSERTE(IsInRange(A, 0.f, 1.f), "");
}

//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& stream, const Color& color)
{
	return stream << "Color[ " << color.R << " " << color.G << " " << color.B << " " << color.A << " ]";
}
