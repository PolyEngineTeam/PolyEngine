#include <CorePCH.hpp>

#include <Math/Plane.hpp>

using namespace Poly;

Plane::eObjectLocation Plane::GetAABoxLocation(const AABox& box) const
{
	auto vertices = box.GetVertices();

	eObjectLocation guessedLocation = GetPointLocation(vertices[0]);
	for (Vector vert : vertices)
	{
		eObjectLocation loc = GetPointLocation(vert);

		if (loc == eObjectLocation::INTERSECTS || loc != guessedLocation)
			return eObjectLocation::INTERSECTS;
	}

	// guess was ok
	return guessedLocation;
}

Plane::eObjectLocation Plane::GetPointLocation(const Vector& point) const
{
	const float dot = (point - Point).Dot(Normal);

	if (dot > 0)
		return eObjectLocation::FRONT;
	else if (dot < 0)
		return eObjectLocation::BEHIND;
	else
		return eObjectLocation::INTERSECTS;
}

//------------------------------------------------------------------------------
namespace Poly {
	std::ostream & operator<<(std::ostream& stream, const Plane& rect)
	{
		return stream << "Plane[Point: " << rect.Point << " Normal: " << rect.Normal << " ]";
	}
}
