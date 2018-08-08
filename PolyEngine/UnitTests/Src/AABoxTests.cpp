#include <Defines.hpp>
#include <catch.hpp>
#include <Math/AABox.hpp>
#include <Math/Quaternion.hpp>

using namespace Poly;

TEST_CASE("AABox contains", "[AABox]") {

	const Vector position(1.f, 2.f, 3.f);
	const Vector size(1.f, 2.f, 3.f);

	const AABox ar(position, size);
	const Vector point(1.f, 3.f, 4.f);
	REQUIRE(ar.Contains(point) == true);

	REQUIRE(ar.Contains(-point) == false);
}

TEST_CASE("AABox collisions with other AABox", "[AABox]") {
	//Positive collision
	const Vector position(1.f, 2.f, 3.f);
	const Vector size(1.f, 2.f, 3.f);

	const AABox ar(position, size);
	const AABox ar2(position + Vector(0.5f, 0.5f, 0.5f), size * 0.5f);
	REQUIRE(ar.Intersects(ar2) == true);
	REQUIRE(ar2.Intersects(ar) == true);

	//No collision
	const AABox ar3(position * 3.f, size * 0.5f);
	REQUIRE(ar.Intersects(ar3) == false);
	REQUIRE(ar2.Intersects(ar3) == false);
}

TEST_CASE("AABox intersection calculation", "[AABox]") {
	const Vector pos1(0.f, 0.f, 0.f);
	const Vector size1(2.f, 2.f, 2.f);
	const Vector pos2(1.f, 1.f, 1.f);
	const Vector size2(1.5f, 1.5f, 1.5f);

	const AABox ar(pos1, size1);
	const AABox ar2(pos2, size2);

	REQUIRE(ar.GetIntersectionVolume(ar2).GetSize() == Vector(1.f, 1.f, 1.f));
}

TEST_CASE("AABox expanding", "[AABox]") {
	const Vector pos1(0.f, 0.f, 0.f);
	const Vector size1(2.f, 2.f, 2.f);
	const Vector pos2(1.f, -1.f, -2.f);
	const Vector size2(15.f, 10.f, 1.f);

	AABox ar(pos1, size1);
	const AABox ar2(pos2, size2);

	ar.Expand(ar2);
	CHECK(ar.GetMin() == Vector(0.f, -1.f, -2.f));
	CHECK(ar.GetMax() == Vector(16.f, 9.f, 2.f));
}

TEST_CASE("AABox transformation", "[AABox]") {
	const Vector pos1(-1.f, -1.f, -1.f);
	const Vector size1(2.f, 2.f, 2.f);

	Matrix rot = Quaternion(Vector(0, 1, 0), 45_deg).ToRotationMatrix();

	const AABox ar(pos1, size1);
	const AABox ar2 = ar.GetTransformed(rot);

	CHECK(ar2.GetMin() == Vector(-sqrtf(2), -1.f, -sqrtf(2)));
	CHECK(ar2.GetMax() == Vector(sqrtf(2), 1.f, sqrtf(2)));
}