#include <catch.hpp>

#include <Vector2i.hpp>

using namespace Poly;

TEST_CASE("Vector2i constructors", "[Vector2i]") {
	// empty constructor
	Vector2i v1;
	REQUIRE(v1.X == 0);
	REQUIRE(v1.Y == 0);

	// basic constructor
	Vector2i v2(1, 2);
	REQUIRE(v2.X == 1);
	REQUIRE(v2.Y == 2);

	// copy constructor
	Vector2i v3(v2);
	REQUIRE(v3.X == 1);
	REQUIRE(v3.Y == 2);

	// = operator
	v1 = v2;
	REQUIRE(v1.X == 1);
	REQUIRE(v1.Y == 2);
}

TEST_CASE("Vector2i comparison operators", "[Vector2i]") {
	Vector2i v1(1, 2);
	Vector2i v2(1, 2);

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

	SECTION("Vector2i negation operator") {
		REQUIRE(-v1 == Vector2i(-1, -2));
		REQUIRE(-(-v1) == v1);
	}
}

TEST_CASE("Vector2i-Vector2i operators", "[Vector2i]") {
	Vector2i v1(3, 4);
	Vector2i v2(1, 2);
	Vector2i v3;

	SECTION("Vector2i sum operator") {
		Vector2i v4(4, 6);
		REQUIRE(v1 + v2 == v4);
		REQUIRE(v2 + v1 == v4);

		v3 = v1;
		v3 += v2;
		REQUIRE(v3 == v4);

		v3 = v2;
		v3 += v1;
		REQUIRE(v3 == v4);
	}

	SECTION("Vector2i subtraction operator") {
		REQUIRE(v1 - v2 == Vector2i(2, 2));
		REQUIRE(v2 - v1 == Vector2i(-2, -2));

		v3 = v1;
		v3 -= v2;
		REQUIRE(v3 == v1 - v2);

		v3 = v2;
		v3 -= v1;
		REQUIRE(v3 == v2 - v1);
	}
}

TEST_CASE("Vector2i-scalar operators", "[Vector2i]") {
	Vector2i v1(1, 2);
	Vector2i v2(v1);
	float s1 = 2;

	SECTION("Multiplication operator") {
		REQUIRE(v1*s1 == Vector2i(2, 4));
		v2 *= s1;
		REQUIRE(v2 == v1*s1);
	}

	SECTION("Division operator") {
		REQUIRE(v1 / s1 == Vector2i(0.5f, 1.0f));
		v2 /= s1;
		REQUIRE(v2 == v1 / s1);
	}
}

TEST_CASE("Vector2i algebraic methods", "[Vector2i]") {
	Vector2i v1(1, 2);
	Vector2i v2(3, 4);

	SECTION("Vector2i dot product") {
		REQUIRE(v1.Dot(v2) == 11);
		REQUIRE(v1.Dot(v2) == v2.Dot(v1));
	}

	//SECTION("Vector2i cross product") {

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
		Vector2i v3 = v1;
		v3.Normalize();
		float l = v1.Length();
		REQUIRE(v3 == v1 / l);
		Vector2i v4 = v1.GetNormalized();
		REQUIRE(v4 == v3);

		v3 = v2;
		v3.Normalize();
		l = v2.Length();
		REQUIRE(v3 == v2 / l);
		v4 = v2.GetNormalized();
		REQUIRE(v4 == v3);
	}
}
