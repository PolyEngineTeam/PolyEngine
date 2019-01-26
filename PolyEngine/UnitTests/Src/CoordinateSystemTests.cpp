#include <Defines.hpp>
#include <catch.hpp>

#include <Math/Matrix.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Vector.hpp>

using namespace Poly;

/*
 Our coordinate system is RH
 Matrix is row major order
 Matrix/Quaternion multiplication happens from right to left.

 Operations LH/RH sensitive
- Cross
- Rotations
- LooktAt
*/

TEST_CASE("Vector operation", "[Coord system]")
{
	CHECK(Vector::UNIT_X.Cross(Vector::UNIT_Y) == Vector::UNIT_Z);
	CHECK(Vector::UNIT_Y.Cross(Vector::UNIT_X) == -Vector::UNIT_Z);

	CHECK(Vector::UNIT_X.Cross(Vector::UNIT_Z) == -Vector::UNIT_Y);
	CHECK(Vector::UNIT_Z.Cross(Vector::UNIT_X) == Vector::UNIT_Y);

	CHECK(Vector::UNIT_Y.Cross(Vector::UNIT_Z) == Vector::UNIT_X);
	CHECK(Vector::UNIT_Z.Cross(Vector::UNIT_Y) == -Vector::UNIT_X);
}

TEST_CASE("Matrix rotations", "[Coord system]")
{
	Matrix rotMat;

	// X rot
	rotMat.SetRotationX(90_deg);
	CHECK(rotMat * Vector::UNIT_X == Vector::UNIT_X);
	CHECK(rotMat * Vector::UNIT_Y == Vector::UNIT_Z);
	CHECK(rotMat * Vector::UNIT_Z == -Vector::UNIT_Y);

	// Y rot
	rotMat.SetRotationY(90_deg);
	CHECK(rotMat * Vector::UNIT_X == -Vector::UNIT_Z);
	CHECK(rotMat * Vector::UNIT_Y == Vector::UNIT_Y);
	CHECK(rotMat * Vector::UNIT_Z == Vector::UNIT_X);

	// Z rot
	rotMat.SetRotationZ(90_deg);
	CHECK(rotMat * Vector::UNIT_X == Vector::UNIT_Y);
	CHECK(rotMat * Vector::UNIT_Y == -Vector::UNIT_X);
	CHECK(rotMat * Vector::UNIT_Z == Vector::UNIT_Z);
}

TEST_CASE("Quaternion rotations", "[Coord system]")
{
	Quaternion rotQuat;

	rotQuat.SetRotation(Vector::UNIT_X, 90_deg);
	CHECK(rotQuat * Vector::UNIT_X == Vector::UNIT_X);
	CHECK(rotQuat * Vector::UNIT_Y == Vector::UNIT_Z);
	CHECK(rotQuat * Vector::UNIT_Z == -Vector::UNIT_Y);

	rotQuat.SetRotation(Vector::UNIT_Y, 90_deg);
	CHECK(rotQuat * Vector::UNIT_X == -Vector::UNIT_Z);
	CHECK(rotQuat * Vector::UNIT_Y == Vector::UNIT_Y);
	CHECK(rotQuat * Vector::UNIT_Z == Vector::UNIT_X);

	// Z rot
	rotQuat.SetRotation(Vector::UNIT_Z, 90_deg);
	CHECK(rotQuat * Vector::UNIT_X == Vector::UNIT_Y);
	CHECK(rotQuat * Vector::UNIT_Y == -Vector::UNIT_X);
	CHECK(rotQuat * Vector::UNIT_Z == Vector::UNIT_Z);
}

TEST_CASE("Matrix lookt at", "[Coord system]")
{
	Matrix lookMat;
	Matrix cmpMat;

	// look forward
	{
		lookMat.SetLookAt(Vector::ZERO, -Vector::UNIT_Z, Vector::UNIT_Y);
		CHECK(lookMat == Matrix::IDENTITY);
	}
	
	// look backward
	{
		lookMat.SetLookAt(Vector::ZERO, Vector::UNIT_Z, Vector::UNIT_Y);
		cmpMat.SetRotationY(180_deg);
		CHECK(lookMat == cmpMat);
	}

	// look right
	{
		lookMat.SetLookAt(Vector::ZERO, Vector::UNIT_X, Vector::UNIT_Y);
		cmpMat.SetRotationY(-90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look left
	{
		lookMat.SetLookAt(Vector::ZERO, -Vector::UNIT_X, Vector::UNIT_Y);
		cmpMat.SetRotationY(90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look up
	{
		lookMat.SetLookAt(Vector::ZERO, Vector::UNIT_Y, Vector::UNIT_Z);
		cmpMat.SetRotationX(90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look down
	{
		lookMat.SetLookAt(Vector::ZERO, -Vector::UNIT_Y, -Vector::UNIT_Z);
		cmpMat.SetRotationX(-90_deg);
		CHECK(lookMat == cmpMat);
	}
}

TEST_CASE("Quaternion lookt at", "[Coord system]")
{
	Quaternion lookQuat;
	Quaternion cmpQuat;

	// look forward
	{
		lookQuat = Quaternion::LookAt(Vector::ZERO, -Vector::UNIT_Z, Vector::UNIT_Y);
		CHECK(lookQuat == Quaternion::IDENTITY);
	}

	// look backward
	{
		lookQuat = Quaternion::LookAt(Vector::ZERO, Vector::UNIT_Z, Vector::UNIT_Y);
		cmpQuat.SetRotation(Vector::UNIT_Y, 180_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look right
	{
		lookQuat = Quaternion::LookAt(Vector::ZERO, Vector::UNIT_X, Vector::UNIT_Y);
		cmpQuat.SetRotation(Vector::UNIT_Y, -90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look left
	{
		lookQuat = Quaternion::LookAt(Vector::ZERO, -Vector::UNIT_X, Vector::UNIT_Y);
		cmpQuat.SetRotation(Vector::UNIT_Y, 90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look up
	{
		lookQuat = Quaternion::LookAt(Vector::ZERO, Vector::UNIT_Y, Vector::UNIT_Z);
		cmpQuat.SetRotation(Vector::UNIT_X, 90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look down
	{
		lookQuat = Quaternion::LookAt(Vector::ZERO, -Vector::UNIT_Y, -Vector::UNIT_Z);
		cmpQuat.SetRotation(Vector::UNIT_X, -90_deg);
		CHECK(lookQuat == cmpQuat);
	}
}