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

	bool AARect::IsCollidingWith(const AARect& otherRect) const
	{
		return IsCollidingWithInternal(otherRect) || otherRect.IsCollidingWithInternal(*this);
	}

	bool AARect::IsCollidingWithInternal(const AARect& otherRect) const
	{
		const Vector vertexes[8] = {
			Pos,
			Vector(Pos.X, Pos.Y + Size.Y, Pos.Z),
			Vector(Pos.X, Pos.Y + Size.Y, Pos.Z + Size.Z),
			Vector(Pos.X, Pos.Y, Pos.Z + Size.Z),
			Vector(Pos.X + Size.X, Pos.Y, Pos.Z),
			Vector(Pos.X + Size.X, Pos.Y + Size.Y, Pos.Z),
			Vector(Pos.X + Size.X, Pos.Y + Size.Y, Pos.Z + Size.Z),
			Vector(Pos.X + Size.X, Pos.Y, Pos.Z + Size.Z)
		};
		for (const auto& vertex : vertexes)
		{
			if (otherRect.Contains(vertex))
				return true;
		}
		return false;
	}
}
