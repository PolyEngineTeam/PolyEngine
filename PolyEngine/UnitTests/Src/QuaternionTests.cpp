#include <catch.hpp>

#include <Quaternion.hpp>
//TODO implement

using namespace Poly;

TEST_CASE("Quaternion constructors", "[Quaternion]") {
	// empty constructor
	Quaternion q1;
	REQUIRE(q1.X == Approx(0));
	REQUIRE(q1.Y == Approx(0));
	REQUIRE(q1.Z == Approx(0));
	REQUIRE(q1.W == Approx(1));

	// axis angle constructor
	Quaternion q2(Vector(1, 0, 0), 180_deg);
	REQUIRE(q2.X == Approx(1));
	REQUIRE(q2.Y == Approx(0));
	REQUIRE(q2.Z == Approx(0));
	REQUIRE(q2.W == Approx(0).margin(1e-6)); //Litle numerical error workaround

	// copy constructor
	Quaternion q3(q2);
	REQUIRE(q3.X == Approx(1));
	REQUIRE(q3.Y == Approx(0));
	REQUIRE(q3.Z == Approx(0));
	REQUIRE(q3.W == Approx(0).margin(1e-6)); //Litle numerical error workaround

	// = operator
	q1 = q2;
	REQUIRE(q1.X == Approx(1));
	REQUIRE(q1.Y == Approx(0));
	REQUIRE(q1.Z == Approx(0));
	REQUIRE(q1.W == Approx(0).margin(1e-6)); //Litle numerical error workaround
}

TEST_CASE("Quaternion comparison operators", "[Quaternion]") {
	Quaternion q1, q2;
	Quaternion q3(Vector(1, 0, 0), 180_deg);

	SECTION("Positive comparison") {
		REQUIRE(q1 == q1);
		REQUIRE(q1 == q2);
		REQUIRE_FALSE(q1 == q3);
	}

	SECTION("Negative comparison") {
		REQUIRE_FALSE(q1 != q1);
		REQUIRE_FALSE(q1 != q2);
		REQUIRE(q1 != q3);
	}
}

TEST_CASE("Quaternion-Quaternion multiplication operator", "[Quaternion]") {
	Quaternion q1, q2;

	SECTION("Multiplication operator") {
		// basic rotations about one axis
		REQUIRE(q1*q2 == Quaternion());
		q1 = Quaternion(Vector(1, 0, 0), 180_deg);
		q2 = Quaternion(Vector(1, 0, 0), -180_deg);
		REQUIRE(q1*q2 == Quaternion(Vector(1, 0, 0), 0_deg));
		REQUIRE(q1*q1 == Quaternion(Vector(1, 0, 0), 360_deg));
		Quaternion qq = q1*q1*q1;
		Quaternion qq2 = Quaternion(Vector(1, 0, 0), 540_deg);
		REQUIRE(q1*q1*q1 == Quaternion(Vector(1, 0, 0), 540_deg));
		q1 *= q2;
		REQUIRE(q1 == Quaternion(Vector(1, 0, 0), 0_deg));

		// simple rotation about two different axes
		q1 = Quaternion(Vector(1, 0, 0), 180_deg);
		q2 = Quaternion(Vector(0, 1, 0), 180_deg);
		REQUIRE(q1*q2 == Quaternion(Vector(0, 0, 1), 180_deg));
		REQUIRE(q2*q1 == Quaternion(Vector(0, 0, 1), 180_deg));

		q1 *= q2;
		REQUIRE(q1 == Quaternion(Vector(0, 0, 1), 180_deg));

		// more complicated rotation
		q1 = Quaternion(Vector(1, 0, 0), 90_deg);
		q2 = Quaternion(Vector(0, 1, 0), 60_deg);
		Quaternion q3;
		q3.X = sqrt(1.5f) / 2.0f;
		q3.Y = 1.0f / (2.0f*sqrt(2.0f));
		q3.Z = 1.0f / (2.0f*sqrt(2.0f));
		q3.W = sqrt(1.5f) / 2.0f;
		REQUIRE(q1*q2 == q3);

		q3.Z = -q3.Z;
		REQUIRE(q2*q1 == q3);

		q2 *= q1;
		REQUIRE(q2 == q3);
	}
}

TEST_CASE("Quaternion algerbraic methods", "[Quaternion]") {
	Quaternion q1;
	Quaternion q2(Vector(1, 0, 0), 90_deg);
	Vector axis1(1, 1, 1);
	axis1.Normalize();
	Quaternion q3(axis1, 90_deg);

	SECTION("Set rotation method") {
		q1.SetRotation(Vector(1, 0, 0), 90_deg);
		REQUIRE(q1 == q2);
		q1.SetRotation(axis1, 90_deg);
		REQUIRE(q1 == q3);
	}

	SECTION("Length method") {
		REQUIRE(q1.LengthSquared() == Approx(1.0f));
		REQUIRE(q2.LengthSquared() == Approx(1.0f));

		q3.X *= sqrtf(3);
		q3.Y *= sqrtf(3);
		q3.Z *= sqrtf(3);
		REQUIRE(q3.LengthSquared() == Approx(2.0f));

		REQUIRE(q1.Length() == Approx(1.0f));
		REQUIRE(q2.Length() == Approx(1.0f));
		REQUIRE(q3.Length() == Approx(sqrt(2.0f)));
	}

	SECTION("Conjugate method") {
		Quaternion q4 = q2;
		q4.Conjugate();
		q1.SetRotation(Vector(-1, 0, 0), 90_deg);
		REQUIRE(q1 == q4);
		REQUIRE(q4 == q2.GetConjugated());

		q4 = q3;
		q4.Conjugate();
		q1.SetRotation(axis1*-1, 90_deg);
		REQUIRE(q1 == q4);
		REQUIRE(q4 == q3.GetConjugated());
	}

	SECTION("Normalize method") {
		q2.X *= 2;
		q2.Y *= 2;
		q2.Z *= 2;
		q2.W *= 2;
		Quaternion q4 = q2;
		q4.Normalize();
		q1.SetRotation(Vector(1, 0, 0), 90_deg);
		REQUIRE(q1 == q4);
		REQUIRE(q4 == q2.GetNormalized());

		q3.X *= 2;
		q3.Y *= 2;
		q3.Z *= 2;
		q3.W *= 2;
		q4 = q3;
		q4.Normalize();
		q1.SetRotation(axis1, 90_deg);
		REQUIRE(q1 == q4);
		REQUIRE(q4 == q3.GetNormalized());
	}

	SECTION("Get rotation matrix") {
		Matrix m1;
		m1.SetRotationX(90_deg);
		REQUIRE(m1 == q2.ToRotationMatrix());

		m1.SetRotationZ(35_deg);
		q2.SetRotation(Vector(0, 0, 1), 35_deg);
		REQUIRE(m1 == q2.ToRotationMatrix());

		Matrix m2;
		m2.SetRotationY(25_deg);
		m1 *= m2;
		q3.SetRotation(Vector(0, 1, 0), 25_deg);
		q2 *= q3;
		REQUIRE(m1 == q2.ToRotationMatrix());
	}

	SECTION("Vector rotate method") {
		Vector v1(1, 2, 3);
		Matrix m1 = q1.ToRotationMatrix();
		REQUIRE(m1*v1 == q1*v1);

		m1 = q2.ToRotationMatrix();
		REQUIRE(m1*v1 == q2*v1);

		m1 = q3.ToRotationMatrix();
		REQUIRE(m1*v1 == q3*v1);
	}
}