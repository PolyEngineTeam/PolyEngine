#include "CorePCH.hpp"
#include "Math/AABox.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
AABox::AABox(const Vector& position, const Vector& size)
	: Pos(position), Size(size)
{
}

//------------------------------------------------------------------------------
AABox AABox::GetIntersectionVolume(const AABox& rhs) const
{
	const float r1MinX = std::min(Pos.X, Pos.X + Size.X);
	const float r1MaxX = std::max(Pos.X, Pos.X + Size.X);
	const float r1MinY = std::min(Pos.Y, Pos.Y + Size.Y);
	const float r1MaxY = std::max(Pos.Y, Pos.Y + Size.Y);
	const float r1MinZ = std::min(Pos.Z, Pos.Z + Size.Z);
	const float r1MaxZ = std::max(Pos.Z, Pos.Z + Size.Z);

	const float r2MinX = std::min(rhs.Pos.X, rhs.Pos.X + rhs.Size.X);
	const float r2MaxX = std::max(rhs.Pos.X, rhs.Pos.X + rhs.Size.X);
	const float r2MinY = std::min(rhs.Pos.Y, rhs.Pos.Y + rhs.Size.Y);
	const float r2MaxY = std::max(rhs.Pos.Y, rhs.Pos.Y + rhs.Size.Y);
	const float r2MinZ = std::min(rhs.Pos.Z, rhs.Pos.Z + rhs.Size.Z);
	const float r2MaxZ = std::max(rhs.Pos.Z, rhs.Pos.Z + rhs.Size.Z);

	const float interLeft = std::max(r1MinX, r2MinX);
	const float interTop = std::max(r1MinY, r2MinY);
	const float interUp = std::max(r1MinZ, r2MinZ);
	const float interRight = std::min(r1MaxX, r2MaxX);
	const float interBottom = std::min(r1MaxY, r2MaxY);
	const float interDown = std::min(r1MaxZ, r2MaxZ);

	if ((interLeft < interRight) && (interTop < interBottom) && (interUp < interDown))
	{
		return AABox(Vector(interLeft, interTop, interUp), Vector(interRight - interLeft, interBottom - interTop, interDown - interUp));
	}
	else
		return AABox(Vector::ZERO, Vector::ZERO);
}

//------------------------------------------------------------------------------
namespace Poly {
	std::ostream & operator<<(std::ostream& stream, const AABox& rect)
	{
		return stream << "AABox[Pos: " << rect.Pos << " Size: " << rect.Size << " ]";
	}
}
