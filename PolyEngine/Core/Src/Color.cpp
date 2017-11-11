#include "CorePCH.hpp"

#include "Color.hpp"

using namespace Poly;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& stream, const Color& color)
{
	return stream << "Color[ " << color.R << " " << color.G << " " << color.B << " " << color.A << " ]";
}
