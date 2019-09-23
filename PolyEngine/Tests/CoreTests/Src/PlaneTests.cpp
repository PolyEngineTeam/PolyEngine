#include <pe/Defines.hpp>
#include <catch.hpp>
#include <pe/core/math/Plane.hpp>
#include <pe/core/math/Quaternion.hpp>

TEST_CASE("Plane point position", "[Plane]") {

	::pe::core::math::Plane p(::pe::core::math::Vector(5, 0, 0), ::pe::core::math::Vector::UNIT_X);

	CHECK(p.GetPointLocation(::pe::core::math::Vector(6, 12, -1)) == ::pe::core::math::Plane::eObjectLocation::FRONT);
	CHECK(p.GetPointLocation(::pe::core::math::Vector(4, 3, 4)) == ::pe::core::math::Plane::eObjectLocation::BEHIND);
	CHECK(p.GetPointLocation(::pe::core::math::Vector(5, 4, 8)) == ::pe::core::math::Plane::eObjectLocation::INTERSECTS);
}

TEST_CASE("Plane box position", "[Plane]") {

	::pe::core::math::Plane p(::pe::core::math::Vector(5, 0, 0), ::pe::core::math::Vector::UNIT_X);
	::pe::core::math::Vector size(2, 2, 2);

	CHECK(p.GetAABoxLocation(::pe::core::math::AABox(::pe::core::math::Vector(6, 12, -1), size)) == ::pe::core::math::Plane::eObjectLocation::FRONT);
	CHECK(p.GetAABoxLocation(::pe::core::math::AABox(::pe::core::math::Vector(1, 3, 4), size)) == ::pe::core::math::Plane::eObjectLocation::BEHIND);
	CHECK(p.GetAABoxLocation(::pe::core::math::AABox(::pe::core::math::Vector(4, 4, 8), size)) == ::pe::core::math::Plane::eObjectLocation::INTERSECTS);
}