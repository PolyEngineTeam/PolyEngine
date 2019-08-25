#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/math/Matrix.hpp>
#include <pe/core/math/Quaternion.hpp>
#include <pe/core/math/Vector.hpp>

using namespace Poly;

/*
 Our coordinate system is RH
 core::math::Matrix is row major order
 core::math::Matrix/Quaternion multiplication happens from right to left.

 Operations LH/RH sensitive
- Cross
- Rotations
- LooktAt
*/

TEST_CASE("Vector operation", "[Coord system]")
{
	CHECK(core::math::Vector::UNIT_X.Cross(core::math::Vector::UNIT_Y) == core::math::Vector::UNIT_Z);
	CHECK(core::math::Vector::UNIT_Y.Cross(core::math::Vector::UNIT_X) == -core::math::Vector::UNIT_Z);

	CHECK(core::math::Vector::UNIT_X.Cross(core::math::Vector::UNIT_Z) == -core::math::Vector::UNIT_Y);
	CHECK(core::math::Vector::UNIT_Z.Cross(core::math::Vector::UNIT_X) == core::math::Vector::UNIT_Y);

	CHECK(core::math::Vector::UNIT_Y.Cross(core::math::Vector::UNIT_Z) == core::math::Vector::UNIT_X);
	CHECK(core::math::Vector::UNIT_Z.Cross(core::math::Vector::UNIT_Y) == -core::math::Vector::UNIT_X);
}

TEST_CASE("Matrix rotations", "[Coord system]")
{
	core::math::Matrix rotMat;

	// X rot
	rotMat.SetRotationX(90_deg);
	CHECK(rotMat * core::math::Vector::UNIT_X == core::math::Vector::UNIT_X);
	CHECK(rotMat * core::math::Vector::UNIT_Y == core::math::Vector::UNIT_Z);
	CHECK(rotMat * core::math::Vector::UNIT_Z == -core::math::Vector::UNIT_Y);

	// Y rot
	rotMat.SetRotationY(90_deg);
	CHECK(rotMat * core::math::Vector::UNIT_X == -core::math::Vector::UNIT_Z);
	CHECK(rotMat * core::math::Vector::UNIT_Y == core::math::Vector::UNIT_Y);
	CHECK(rotMat * core::math::Vector::UNIT_Z == core::math::Vector::UNIT_X);

	// Z rot
	rotMat.SetRotationZ(90_deg);
	CHECK(rotMat * core::math::Vector::UNIT_X == core::math::Vector::UNIT_Y);
	CHECK(rotMat * core::math::Vector::UNIT_Y == -core::math::Vector::UNIT_X);
	CHECK(rotMat * core::math::Vector::UNIT_Z == core::math::Vector::UNIT_Z);
}

TEST_CASE("Quaternion rotations", "[Coord system]")
{
	core::math::Quaternion rotQuat;

	rotQuat.SetRotation(core::math::Vector::UNIT_X, 90_deg);
	CHECK(rotQuat * core::math::Vector::UNIT_X == core::math::Vector::UNIT_X);
	CHECK(rotQuat * core::math::Vector::UNIT_Y == core::math::Vector::UNIT_Z);
	CHECK(rotQuat * core::math::Vector::UNIT_Z == -core::math::Vector::UNIT_Y);

	rotQuat.SetRotation(core::math::Vector::UNIT_Y, 90_deg);
	CHECK(rotQuat * core::math::Vector::UNIT_X == -core::math::Vector::UNIT_Z);
	CHECK(rotQuat * core::math::Vector::UNIT_Y == core::math::Vector::UNIT_Y);
	CHECK(rotQuat * core::math::Vector::UNIT_Z == core::math::Vector::UNIT_X);

	// Z rot
	rotQuat.SetRotation(core::math::Vector::UNIT_Z, 90_deg);
	CHECK(rotQuat * core::math::Vector::UNIT_X == core::math::Vector::UNIT_Y);
	CHECK(rotQuat * core::math::Vector::UNIT_Y == -core::math::Vector::UNIT_X);
	CHECK(rotQuat * core::math::Vector::UNIT_Z == core::math::Vector::UNIT_Z);
}

TEST_CASE("Matrix lookt at", "[Coord system]")
{
	core::math::Matrix lookMat;
	core::math::Matrix cmpMat;

	// look forward
	{
		lookMat.SetLookAt(core::math::Vector::ZERO, -core::math::Vector::UNIT_Z, core::math::Vector::UNIT_Y);
		CHECK(lookMat == core::math::Matrix::IDENTITY);
	}
	
	// look backward
	{
		lookMat.SetLookAt(core::math::Vector::ZERO, core::math::Vector::UNIT_Z, core::math::Vector::UNIT_Y);
		cmpMat.SetRotationY(180_deg);
		CHECK(lookMat == cmpMat);
	}

	// look right
	{
		lookMat.SetLookAt(core::math::Vector::ZERO, core::math::Vector::UNIT_X, core::math::Vector::UNIT_Y);
		cmpMat.SetRotationY(-90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look left
	{
		lookMat.SetLookAt(core::math::Vector::ZERO, -core::math::Vector::UNIT_X, core::math::Vector::UNIT_Y);
		cmpMat.SetRotationY(90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look up
	{
		lookMat.SetLookAt(core::math::Vector::ZERO, core::math::Vector::UNIT_Y, core::math::Vector::UNIT_Z);
		cmpMat.SetRotationX(90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look down
	{
		lookMat.SetLookAt(core::math::Vector::ZERO, -core::math::Vector::UNIT_Y, -core::math::Vector::UNIT_Z);
		cmpMat.SetRotationX(-90_deg);
		CHECK(lookMat == cmpMat);
	}
}

TEST_CASE("Quaternion lookt at", "[Coord system]")
{
	core::math::Quaternion lookQuat;
	core::math::Quaternion cmpQuat;

	// look forward
	{
		lookQuat = core::math::Quaternion::LookAt(core::math::Vector::ZERO, -core::math::Vector::UNIT_Z, core::math::Vector::UNIT_Y);
		CHECK(lookQuat == core::math::Quaternion::IDENTITY);
	}

	// look backward
	{
		lookQuat = core::math::Quaternion::LookAt(core::math::Vector::ZERO, core::math::Vector::UNIT_Z, core::math::Vector::UNIT_Y);
		cmpQuat.SetRotation(core::math::Vector::UNIT_Y, 180_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look right
	{
		lookQuat = core::math::Quaternion::LookAt(core::math::Vector::ZERO, core::math::Vector::UNIT_X, core::math::Vector::UNIT_Y);
		cmpQuat.SetRotation(core::math::Vector::UNIT_Y, -90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look left
	{
		lookQuat = core::math::Quaternion::LookAt(core::math::Vector::ZERO, -core::math::Vector::UNIT_X, core::math::Vector::UNIT_Y);
		cmpQuat.SetRotation(core::math::Vector::UNIT_Y, 90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look up
	{
		lookQuat = core::math::Quaternion::LookAt(core::math::Vector::ZERO, core::math::Vector::UNIT_Y, core::math::Vector::UNIT_Z);
		cmpQuat.SetRotation(core::math::Vector::UNIT_X, 90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look down
	{
		lookQuat = core::math::Quaternion::LookAt(core::math::Vector::ZERO, -core::math::Vector::UNIT_Y, -core::math::Vector::UNIT_Z);
		cmpQuat.SetRotation(core::math::Vector::UNIT_X, -90_deg);
		CHECK(lookQuat == cmpQuat);
	}
}