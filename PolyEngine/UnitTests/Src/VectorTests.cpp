#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/math/Vector.hpp>

using namespace Poly;

TEST_CASE("Vector constructors", "[Vector]") {
  // empty constructor
  Vector v1;
  REQUIRE(v1.X == 0);
  REQUIRE(v1.Y == 0);
  REQUIRE(v1.Z == 0);

  // basic constructor
  Vector v2(1, 2, 3);
  REQUIRE(v2.X == 1);
  REQUIRE(v2.Y == 2);
  REQUIRE(v2.Z == 3);

  // copy constructor
  Vector v3(v2);
  REQUIRE(v3.X == 1);
  REQUIRE(v3.Y == 2);
  REQUIRE(v3.Z == 3);

  // = operator
  v1 = v2;
  REQUIRE(v1.X == 1);
  REQUIRE(v1.Y == 2);
  REQUIRE(v1.Z == 3);
}

TEST_CASE("Vector comparison operators", "[Vector]") {
  Vector v1(1, 2, 3);
  Vector v2(1, 2, 3);

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
  
  SECTION("Vector negation operator") {
    REQUIRE(-v1 == Vector(-1,-2,-3));
    REQUIRE(-(-v1) == v1);
  }
}

TEST_CASE("Vector-Vector operators", "[Vector]") {
  Vector v1(3, 4, 6);
  Vector v2(1, 2, 3);
  Vector v3;
  
  SECTION("Vector sum operator") {
	  Vector v4(4, 6, 9);
	  REQUIRE(v1 + v2 == v4);
	  REQUIRE(v2 + v1 == v4);

	  v3 = v1;
	  v3 += v2;
	  REQUIRE(v3 == v4);

	  v3 = v2;
	  v3 += v1;
	  REQUIRE(v3 == v4);
  }
  
  SECTION("Vector subtraction operator") {
	  REQUIRE(v1 - v2 == Vector(2, 2, 3));
	  REQUIRE(v2 - v1 == Vector(-2, -2, -3));

	  v3 = v1;
	  v3 -= v2;
	  REQUIRE(v3 == v1 - v2);

	  v3 = v2;
	  v3 -= v1;
	  REQUIRE(v3 == v2 - v1);
  }
}

TEST_CASE("Vector-scalar operators", "[Vector]") {
	Vector v1(1, 2, 3);
	Vector v2(v1);
	float s1 = 2;

	SECTION("Multiplication operator") {
		REQUIRE(v1*s1 == Vector(2, 4, 6));
		v2 *= s1;
		REQUIRE(v2 == v1*s1);
	}

	SECTION("Division operator") {
		REQUIRE(v1 / s1 == Vector(0.5f, 1.0f, 1.5f));
		v2 /= s1;
		REQUIRE(v2 == v1 / s1);
	}

	SECTION("Sum operator") {
		REQUIRE(v1 + s1 == Vector(3.0f, 4.0f, 5.0f));
		v2 += s1;
		REQUIRE(v2 == v1 + s1);
	}

	SECTION("Subtraction operator") {
		REQUIRE(v1 - s1 == Vector(-1.0f, 0.0f, 1.0f));
		v2 -= s1;
		REQUIRE(v2 == v1 - s1);
	}
}

TEST_CASE("Vector algebraic methods", "[Vector]") {
  Vector v1(1, 2, 3);
  Vector v2(3, 4, 6);
  
  SECTION("Vector dot product") {
    REQUIRE(v1.Dot(v2) == 29);
    REQUIRE(v1.Dot(v2) == v2.Dot(v1));
  }
  
  SECTION("Vector cross product") {
    Vector v3 = v1.Cross(v2);
    REQUIRE(v3 == Vector(0, 3, -2));
    REQUIRE( v2.Cross(v1) == -v3);
  }

  SECTION("Length calculation") {
	  REQUIRE(v1.LengthSquared() == 14.0f);
	  REQUIRE(v2.LengthSquared() == 61.0f);
	  REQUIRE(v1.Length() == Approx(sqrt(14.0f)));
	  REQUIRE(v2.Length() == Approx(sqrt(61.0f)));
  }

  SECTION("Normalization") {
	  Vector v3 = v1;
	  v3.Normalize();
	  float l = v1.Length();
	  REQUIRE(v3 == v1 / l);
	  Vector v4 = v1.GetNormalized();
	  REQUIRE(v4 == v3);

	  v3 = v2;
	  v3.Normalize();
	  l = v2.Length();
	  REQUIRE(v3 == v2 / l);
	  v4 = v2.GetNormalized();
	  REQUIRE(v4 == v3);
  }
}
