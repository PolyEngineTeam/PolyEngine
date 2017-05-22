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
	REQUIRE(ar.IsCollidingWith(ar2) == true);

	//No collision
	const AARect ar3(position * 3.f, size * 0.5f);
	REQUIRE(ar.IsCollidingWith(ar3) == false);
	REQUIRE(ar2.IsCollidingWith(ar3) == false);
}