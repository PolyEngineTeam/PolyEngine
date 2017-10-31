#include "CorePCH.hpp"
#include "AARect.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Poly::AARect::AARect(const Vector2f& position, const Vector2f& size)
	: Pos(position), Size(size)
{
}

//------------------------------------------------------------------------------
AARect Poly::AARect::GetIntersectionVolume(const AARect& rhs) const
{
	const float r1MinX = std::min(Pos.X, Pos.X + Size.X);
	const float r1MaxX = std::max(Pos.X, Pos.X + Size.X);
	const float r1MinY = std::min(Pos.Y, Pos.Y + Size.Y);
	const float r1MaxY = std::max(Pos.Y, Pos.Y + Size.Y);

	const float r2MinX = std::min(rhs.Pos.X, rhs.Pos.X + rhs.Size.X);
	const float r2MaxX = std::max(rhs.Pos.X, rhs.Pos.X + rhs.Size.X);
	const float r2MinY = std::min(rhs.Pos.Y, rhs.Pos.Y + rhs.Size.Y);
	const float r2MaxY = std::max(rhs.Pos.Y, rhs.Pos.Y + rhs.Size.Y);

	const float interLeft = std::max(r1MinX, r2MinX);
	const float interTop = std::max(r1MinY, r2MinY);
	const float interRight = std::min(r1MaxX, r2MaxX);
	const float interBottom = std::min(r1MaxY, r2MaxY);

	if ((interLeft < interRight) && (interTop < interBottom))
	{
		return AARect(Vector2f(interLeft, interTop), Vector2f(interRight - interLeft, interBottom - interTop));
	}
	else
		return AARect(Vector2f::ZERO, Vector2f::ZERO);
}

//------------------------------------------------------------------------------
namespace Poly {
	std::ostream& operator<<(std::ostream& stream, const AARect& rect)
	{
		return stream << "AARect[Pos: " << rect.Pos << " Size: " << rect.Size << " ]";
	}
}