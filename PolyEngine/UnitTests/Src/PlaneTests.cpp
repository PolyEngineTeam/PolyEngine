#include <pe/Defines.hpp>
#include <catch.hpp>
#include <pe/core/math/Plane.hpp>
#include <pe/core/math/Quaternion.hpp>

using namespace Poly;

TEST_CASE("Plane point position", "[Plane]") {

	Plane p(core::math::Vector(5, 0, 0), core::math::Vector::UNIT_X);

	CHECK(p.GetPointLocation(core::math::Vector(6, 12, -1)) == Plane::eObjectLocation::FRONT);
	CHECK(p.GetPointLocation(core::math::Vector(4, 3, 4)) == Plane::eObjectLocation::BEHIND);
	CHECK(p.GetPointLocation(core::math::Vector(5, 4, 8)) == Plane::eObjectLocation::INTERSECTS);
}

TEST_CASE("Plane box position", "[Plane]") {

	Plane p(core::math::Vector(5, 0, 0), core::math::Vector::UNIT_X);
	core::math::Vector size(2, 2, 2);

	CHECK(p.GetAABoxLocation(AABox(core::math::Vector(6, 12, -1), size)) == Plane::eObjectLocation::FRONT);
	CHECK(p.GetAABoxLocation(AABox(core::math::Vector(1, 3, 4), size)) == Plane::eObjectLocation::BEHIND);
	CHECK(p.GetAABoxLocation(AABox(core::math::Vector(4, 4, 8), size)) == Plane::eObjectLocation::INTERSECTS);
}