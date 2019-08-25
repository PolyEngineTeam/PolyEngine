#include <pe/Defines.hpp>
#include <catch.hpp>
#include <pe/core/math/AABox.hpp>
#include <pe/core/math/Quaternion.hpp>

using namespace Poly;

TEST_CASE("AABox contains", "[AABox]") {
	// point
	const core::math::Vector position(1.f, 2.f, 3.f);
	const core::math::Vector size(1.f, 2.f, 3.f);

	const core::math::AABox ar(position, size);
	const core::math::Vector point(1.f, 3.f, 4.f);
	REQUIRE(ar.Contains(point) == true);
	REQUIRE(ar.Contains(-point) == false);

	// other AABB inside ar AABB on each axis
	const core::math::AABox aabbX( core::math::Vector(1.5f, 0.0f, 0.0f), core::math::Vector(0.5f, 0.5f, 0.5f));
	const core::math::AABox aabbY( core::math::Vector(0.0f, 2.5f, 0.0f), core::math::Vector(0.5f, 0.5f, 0.5f));
	const core::math::AABox aabbZ( core::math::Vector(0.0f, 0.0f, 3.5f), core::math::Vector(0.5f, 0.5f, 0.5f));
	const core::math::AABox aabbXY(core::math::Vector(1.5f, 2.5f, 0.0f), core::math::Vector(0.5f, 0.5f, 0.5f));

	REQUIRE(ar.ContainsX(aabbX) == true);
	REQUIRE(ar.ContainsX(aabbY) == false);
	REQUIRE(ar.ContainsX(aabbZ) == false);
	
	REQUIRE(ar.ContainsY(aabbX) == false);
	REQUIRE(ar.ContainsY(aabbY) == true);
	REQUIRE(ar.ContainsY(aabbZ) == false);

	REQUIRE(ar.ContainsZ(aabbX) == false);
	REQUIRE(ar.ContainsZ(aabbY) == false);
	REQUIRE(ar.ContainsZ(aabbZ) == true);

	REQUIRE(ar.ContainsXY(aabbXY) == true);
	REQUIRE(ar.ContainsXY(aabbX) == false);
	REQUIRE(ar.ContainsXY(aabbY) == false);
	REQUIRE(ar.ContainsXY(aabbZ) == false);

	// ar AABB inside other AABB on each axis
	const core::math::AABox aabbX2( core::math::Vector::ZERO, core::math::Vector(10.0f, 0.5f, 0.5f));
	const core::math::AABox aabbY2( core::math::Vector::ZERO, core::math::Vector(0.5f, 10.0f, 0.5f));
	const core::math::AABox aabbZ2( core::math::Vector::ZERO, core::math::Vector(0.5f, 0.5f, 10.0f));
	const core::math::AABox aabbXY2(core::math::Vector::ZERO, core::math::Vector(10.0f, 10.0f, 0.5f));

	REQUIRE(ar.ContainsX(aabbX2) == true);
	REQUIRE(ar.ContainsX(aabbY2) == false);
	REQUIRE(ar.ContainsX(aabbZ2) == false);

	REQUIRE(ar.ContainsY(aabbX2) == false);
	REQUIRE(ar.ContainsY(aabbY2) == true);
	REQUIRE(ar.ContainsY(aabbZ2) == false);

	REQUIRE(ar.ContainsZ(aabbX2) == false);
	REQUIRE(ar.ContainsZ(aabbY2) == false);
	REQUIRE(ar.ContainsZ(aabbZ2) == true);

	REQUIRE(ar.ContainsXY(aabbXY2) == true);
	REQUIRE(ar.ContainsXY(aabbX2) == false);
	REQUIRE(ar.ContainsXY(aabbY2) == false);
	REQUIRE(ar.ContainsXY(aabbZ2) == false);
}

TEST_CASE("AABox collisions with other core::math::AABox", "[AABox]") {
	//Positive collision
	const core::math::Vector position(1.f, 2.f, 3.f);
	const core::math::Vector size(1.f, 2.f, 3.f);

	const core::math::AABox ar(position, size);
	const core::math::AABox ar2(position + core::math::Vector(0.5f, 0.5f, 0.5f), size * 0.5f);
	REQUIRE(ar.Intersects(ar2) == true);
	REQUIRE(ar2.Intersects(ar) == true);

	//No collision
	const core::math::AABox ar3(position * 3.f, size * 0.5f);
	REQUIRE(ar.Intersects(ar3) == false);
	REQUIRE(ar2.Intersects(ar3) == false);
}

TEST_CASE("AABox intersection calculation", "[AABox]") {
	const core::math::Vector pos1(0.f, 0.f, 0.f);
	const core::math::Vector size1(2.f, 2.f, 2.f);
	const core::math::Vector pos2(1.f, 1.f, 1.f);
	const core::math::Vector size2(1.5f, 1.5f, 1.5f);

	const core::math::AABox ar(pos1, size1);
	const core::math::AABox ar2(pos2, size2);

	REQUIRE(ar.GetIntersectionVolume(ar2).GetSize() == core::math::Vector(1.f, 1.f, 1.f));
}

TEST_CASE("AABox expanding", "[AABox]") {
	const core::math::Vector pos1(0.f, 0.f, 0.f);
	const core::math::Vector size1(2.f, 2.f, 2.f);
	const core::math::Vector pos2(1.f, -1.f, -2.f);
	const core::math::Vector size2(15.f, 10.f, 1.f);

	core::math::AABox ar(pos1, size1);
	const core::math::AABox ar2(pos2, size2);

	ar.Expand(ar2);
	CHECK(ar.GetMin() == core::math::Vector(0.f, -1.f, -2.f));
	CHECK(ar.GetMax() == core::math::Vector(16.f, 9.f, 2.f));
}

TEST_CASE("AABox transformation", "[AABox]") {
	const core::math::Vector pos1(-1.f, -1.f, -1.f);
	const core::math::Vector size1(2.f, 2.f, 2.f);

	core::math::Matrix rot = core::math::Quaternion(core::math::Vector(0, 1, 0), 45_deg).ToRotationMatrix();

	const core::math::AABox ar(pos1, size1);
	const core::math::AABox ar2 = ar.GetTransformed(rot);

	CHECK(ar2.GetMin() == core::math::Vector(-sqrtf(2), -1.f, -sqrtf(2)));
	CHECK(ar2.GetMax() == core::math::Vector(sqrtf(2), 1.f, sqrtf(2)));
}