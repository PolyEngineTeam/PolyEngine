#include <catch.hpp>

#include <AARect.hpp>

using namespace Poly;

TEST_CASE("AARect contains", "[AARect]") {

	const Vector position(1.f, 2.f, 3.f);
	const Vector size(1.f, 2.f, 3.f);

	const AARect ar(position, size);
	const Vector point(1.f, 3.f, 4.f);
	REQUIRE(ar.Contains(point) == true);

	REQUIRE(ar.Contains(-point) == false);
}

TEST_CASE("AARect collisions with other AARect", "[AARect]") {
	//Positive collision
	const Vector position(1.f, 2.f, 3.f);
	const Vector size(1.f, 2.f, 3.f);

	const AARect ar(position, size);
	const AARect ar2(position + Vector(0.5f, 0.5f, 0.5f), size * 0.5f);
	REQUIRE(ar.DoesIntersect(&ar2) == true);
	REQUIRE(ar2.DoesIntersect(&ar) == true);

	//No collision
	const AARect ar3(position * 3.f, size * 0.5f);
	REQUIRE(ar.DoesIntersect(&ar3) == false);
	REQUIRE(ar2.DoesIntersect(&ar3) == false);
}

TEST_CASE("AARect intersection calculation", "[AARect]") {
	const Vector pos1(0.f, 0.f, 0.f);
	const Vector size1(2.f, 2.f, 2.f);
	const Vector pos2(1.f, 1.f, 1.f);
	const Vector size2(1.5f, 1.5f, 1.5f);

	const AARect ar(pos1, size1);
	const AARect ar2(pos2, size2);

	REQUIRE(ar.GetIntersection(&ar2).GetSize() == Vector(1.f, 1.f, 1.f));
}