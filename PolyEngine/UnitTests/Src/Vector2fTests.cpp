#include <Defines.hpp>
#include <catch.hpp>

#include <Math/Vector2f.hpp>

using namespace Poly;

TEST_CASE("Vector2f constructors", "[Vector2f]") {
	// empty constructor
	Vector2f v1;
	REQUIRE(v1.X == 0);
	REQUIRE(v1.Y == 0);

	// basic constructor
	Vector2f v2(1, 2);
	REQUIRE(v2.X == 1);
	REQUIRE(v2.Y == 2);

	// copy constructor
	Vector2f v3(v2);
	REQUIRE(v3.X == 1);
	REQUIRE(v3.Y == 2);

	// = operator
	v1 = v2;
	REQUIRE(v1.X == 1);
	REQUIRE(v1.Y == 2);
}

TEST_CASE("Vector2f comparison operators", "[Vector2f]") {
	Vector2f v1(1, 2);
	Vector2f v2(1, 2);

	SECTION("Positive comparison operator") {
		REQUIRE(v1 == v2);
		v1.X = 5;
		REQUIRE_FALSE(v1 == v2);
	}

	SECTION("Negative comparison operator") {
		REQUIRE_FALSE(v1 != v2);
		v1.X = 5;
		REQUIRE(v1 != v2);
	}

	SECTION("Vector2f negation operator") {
		REQUIRE(-v1 == Vector2f(-1, -2));
		REQUIRE(-(-v1) == v1);
	}
}

TEST_CASE("Vector2f-Vector2f operators", "[Vector2f]") {
	Vector2f v1(3, 4);
	Vector2f v2(1, 2);
	Vector2f v3;

	SECTION("Vector2f sum operator") {
		Vector2f v4(4, 6);
		REQUIRE(v1 + v2 == v4);
		REQUIRE(v2 + v1 == v4);

		v3 = v1;
		v3 += v2;
		REQUIRE(v3 == v4);

		v3 = v2;
		v3 += v1;
		REQUIRE(v3 == v4);
	}

	SECTION("Vector2f subtraction operator") {
		REQUIRE(v1 - v2 == Vector2f(2, 2));
		REQUIRE(v2 - v1 == Vector2f(-2, -2));

		v3 = v1;
		v3 -= v2;
		REQUIRE(v3 == v1 - v2);

		v3 = v2;
		v3 -= v1;
		REQUIRE(v3 == v2 - v1);
	}
}

TEST_CASE("Vector2f-scalar operators", "[Vector2f]") {
	Vector2f v1(1, 2);
	Vector2f v2(v1);
	float s1 = 2;

	SECTION("Multiplication operator") {
		REQUIRE(v1*s1 == Vector2f(2, 4));
		v2 *= s1;
		REQUIRE(v2 == v1*s1);
	}

	SECTION("Division operator") {
		REQUIRE(v1 / s1 == Vector2f(0.5f, 1.0f));
		v2 /= s1;
		REQUIRE(v2 == v1 / s1);
	}
}

TEST_CASE("Vector2f algebraic methods", "[Vector2f]") {
	Vector2f v1(1, 2);
	Vector2f v2(3, 4);

	SECTION("Vector2f dot product") {
		REQUIRE(v1.Dot(v2) == 11);
		REQUIRE(v1.Dot(v2) == v2.Dot(v1));
	}

	//SECTION("Vector2f cross product") {

	//	REQUIRE(v1.Cross(v2) == );
	//	REQUIRE(v2.Cross(v1) == -v3);
	//}

	SECTION("Length calculation") {
		REQUIRE(v1.LengthSquared() == 5.0f);
		REQUIRE(v2.LengthSquared() == 25.0f);
		REQUIRE(v1.Length() == Approx(sqrt(5.0f)));
		REQUIRE(v2.Length() == Approx(sqrt(25.0f)));
	}

	SECTION("Normalization") {
		Vector2f v3 = v1;
		v3.Normalize();
		float l = v1.Length();
		REQUIRE(v3 == v1 / l);
		Vector2f v4 = v1.GetNormalized();
		REQUIRE(v4 == v3);

		v3 = v2;
		v3.Normalize();
		l = v2.Length();
		REQUIRE(v3 == v2 / l);
		v4 = v2.GetNormalized();
		REQUIRE(v4 == v3);
	}
}
