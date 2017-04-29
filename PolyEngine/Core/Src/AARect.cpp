#include "CorePCH.hpp"
#include "AARect.hpp"

using namespace Poly;

AARect::AARect(const Vector& position, const Vector& size)
	: Pos(position), Size(size)
{
}

namespace Poly {
	std::ostream & operator<<(std::ostream& stream, const AARect& rect)
	{
		return stream << "AARect[Pos: " << rect.Pos << " Size: " << rect.Size << " ]";
	}
}
