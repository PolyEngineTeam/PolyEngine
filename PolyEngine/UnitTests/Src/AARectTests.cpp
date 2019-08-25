#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/math/AARect.hpp>

using namespace Poly;

TEST_CASE("AARect contains", "[AARect]")
{
	const core::math::Vector2f position(1.f, 2.f);
	const core::math::Vector2f size(1.f, 2.f);

	const core::math::AARect ar(position, size);
	const core::math::Vector2f point(1.f, 3.f);
	REQUIRE(ar.Contains(point) == true);

	REQUIRE(ar.Contains(-point) == false);
}

TEST_CASE("AARect collisions with other core::math::AARect", "[AARect]")
{
	//Positive collision
	const core::math::Vector2f position(1.f, 2.f);
	const core::math::Vector2f size(1.f, 2.f);

	const core::math::AARect ar(position, size);
	const core::math::AARect ar2(position + core::math::Vector2f(0.5f, 0.5f), size * 0.5f);
	REQUIRE(ar.Intersects(ar2) == true);
	REQUIRE(ar2.Intersects(ar) == true);

	//No collision
	const core::math::AARect ar3(position * 3.f, size * 0.5f);
	REQUIRE(ar.Intersects(ar3) == false);
	REQUIRE(ar2.Intersects(ar3) == false);
}

TEST_CASE("AARect intersection calculation", "[AARect]")
{
	const core::math::Vector2f pos1(0.f, 0.f);
	const core::math::Vector2f size1(2.f, 2.f);
	const core::math::Vector2f pos2(1.f, 1.f);
	const core::math::Vector2f size2(1.5f, 1.5f);

	const core::math::AARect ar(pos1, size1);
	const core::math::AARect ar2(pos2, size2);

	REQUIRE(ar.GetIntersectionVolume(ar2).GetSize() == core::math::Vector2f(1.f, 1.f));
}