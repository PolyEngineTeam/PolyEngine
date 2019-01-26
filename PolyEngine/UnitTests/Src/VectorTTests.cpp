#include <Defines.hpp>
#include <catch.hpp>

#include <Math/VectorT.hpp>

using namespace Poly;

TEST_CASE("Vector<u32> empty constructors", "[VectorT]") {
	// empty constructor
	VectorT<u32, 2> v2;
	REQUIRE(v2.GetSize() == 2);
	CHECK(v2.GetX() == 0);
	CHECK(v2.GetY() == 0);

	VectorT<u32, 3> v3;
	REQUIRE(v3.GetSize() == 3);
	CHECK(v3.GetX() == 0);
	CHECK(v3.GetY() == 0);
	CHECK(v3.GetZ() == 0);

	VectorT<u32, 4> v4;
	REQUIRE(v4.GetSize() == 4);
	CHECK(v4.GetX() == 0);
	CHECK(v4.GetY() == 0);
	CHECK(v4.GetZ() == 0);
	CHECK(v4.GetW() == 0);
}

TEST_CASE("Vector<float> empty constructors", "[VectorT]") {
	// empty constructor
	VectorT<float, 2> v2;
	REQUIRE(v2.GetSize() == 2);
	CHECK(v2.GetX() == 0.f);
	CHECK(v2.GetY() == 0.f);

	VectorT<float, 3> v3;
	REQUIRE(v3.GetSize() == 3);
	CHECK(v3.GetX() == 0.f);
	CHECK(v3.GetY() == 0.f);
	CHECK(v3.GetZ() == 0.f);

	VectorT<float, 4> v4;
	REQUIRE(v4.GetSize() == 4);
	CHECK(v4.GetX() == 0.f);
	CHECK(v4.GetY() == 0.f);
	CHECK(v4.GetZ() == 0.f);
	CHECK(v4.GetW() == 0.f);
}

TEST_CASE("Vector<u32> value constructors", "[VectorT]") {
	// empty constructor
	VectorT<u32, 2> v2(1, 2);
	REQUIRE(v2.GetSize() == 2);
	CHECK(v2.GetX() == 1);
	CHECK(v2.GetY() == 2);

	VectorT<u32, 3> v3(1, 2, 3);
	REQUIRE(v3.GetSize() == 3);
	CHECK(v3.GetX() == 1);
	CHECK(v3.GetY() == 2);
	CHECK(v3.GetZ() == 3);

	VectorT<u32, 4> v4(1, 2, 3, 4);
	REQUIRE(v4.GetSize() == 4);
	CHECK(v4.GetX() == 1);
	CHECK(v4.GetY() == 2);
	CHECK(v4.GetZ() == 3);
	CHECK(v4.GetW() == 4);
}

TEST_CASE("Vector<float> value constructors", "[VectorT]") {
	// empty constructor
	VectorT<float, 2> v2(1.f, 2.f);
	REQUIRE(v2.GetSize() == 2);
	CHECK(v2.GetX() == 1.f);
	CHECK(v2.GetY() == 2.f);

	VectorT<float, 3> v3(1.f, 2.f, 3.f);
	REQUIRE(v3.GetSize() == 3);
	CHECK(v3.GetX() == 1.f);
	CHECK(v3.GetY() == 2.f);
	CHECK(v3.GetZ() == 3.f);

	VectorT<float, 4> v4(1.f, 2.f, 3.f, 4.f);
	REQUIRE(v4.GetSize() == 4);
	CHECK(v4.GetX() == 1.f);
	CHECK(v4.GetY() == 2.f);
	CHECK(v4.GetZ() == 3.f);
	CHECK(v4.GetW() == 4.f);
}