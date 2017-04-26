#include "CorePCH.hpp"
#include "Rectangle.hpp"

using namespace Poly;

Rectangle::Rectangle(const Vector& position, const Vector& size)
	: Pos(position), Size(size)
{
}

namespace Poly {
	std::ostream & operator<<(std::ostream& stream, const Rectangle& rect)
	{
		return stream << "Rect[Pos: " << rect.Pos << " Size: " << rect.Size << " ]";
	}
}
