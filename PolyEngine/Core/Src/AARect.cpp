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

	AARect AARect::GetIntersection(const AARect* otherRect) const
	{
		ASSERTE(otherRect != nullptr, "AARect::GetIntersection() called with otherRect == nullptr.");

		const float r1MinX = std::min(Pos.X, Pos.X + Size.X);
		const float r1MaxX = std::max(Pos.X, Pos.X + Size.X);
		const float r1MinY = std::min(Pos.Y, Pos.Y + Size.Y);
		const float r1MaxY = std::max(Pos.Y, Pos.Y + Size.Y);
		const float r1MinZ = std::min(Pos.Z, Pos.Z + Size.Z);
		const float r1MaxZ = std::max(Pos.Z, Pos.Z + Size.Z);

		const float r2MinX = std::min(otherRect->Pos.X, otherRect->Pos.X + otherRect->Size.X);
		const float r2MaxX = std::max(otherRect->Pos.X, otherRect->Pos.X + otherRect->Size.X);
		const float r2MinY = std::min(otherRect->Pos.Y, otherRect->Pos.Y + otherRect->Size.Y);
		const float r2MaxY = std::max(otherRect->Pos.Y, otherRect->Pos.Y + otherRect->Size.Y);
		const float r2MinZ = std::min(otherRect->Pos.Z, otherRect->Pos.Z + otherRect->Size.Z);
		const float r2MaxZ = std::max(otherRect->Pos.Z, otherRect->Pos.Z + otherRect->Size.Z);

		const float interLeft = std::max(r1MinX, r2MinX);
		const float interTop = std::max(r1MinY, r2MinY);
		const float interUp = std::max(r1MinZ, r2MinZ);
		const float interRight = std::min(r1MaxX, r2MaxX);
		const float interBottom = std::min(r1MaxY, r2MaxY);
		const float interDown = std::min(r1MaxZ, r2MaxZ);

		if ((interLeft < interRight) && (interTop < interBottom) && (interUp < interDown))
		{
			return AARect(Vector(interLeft, interTop, interUp), Vector(interRight - interLeft, interBottom - interTop, interDown - interUp));
		}
		else
			return AARect(Vector::ZERO, Vector::ZERO);
	}
}
