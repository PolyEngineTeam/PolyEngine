#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/math/Matrix.hpp>
#include <pe/core/math/Quaternion.hpp>
#include <pe/core/math/Vector.hpp>

/*
 Our coordinate system is RH
 ::pe::core::math::Matrix is row major order
 ::pe::core::math::Matrix/Quaternion multiplication happens from right to left.

 Operations LH/RH sensitive
- Cross
- Rotations
- LooktAt
*/

TEST_CASE("Vector operation", "[Coord system]")
{
	CHECK(::pe::core::math::Vector::UNIT_X.Cross(::pe::core::math::Vector::UNIT_Y) == ::pe::core::math::Vector::UNIT_Z);
	CHECK(::pe::core::math::Vector::UNIT_Y.Cross(::pe::core::math::Vector::UNIT_X) == -::pe::core::math::Vector::UNIT_Z);

	CHECK(::pe::core::math::Vector::UNIT_X.Cross(::pe::core::math::Vector::UNIT_Z) == -::pe::core::math::Vector::UNIT_Y);
	CHECK(::pe::core::math::Vector::UNIT_Z.Cross(::pe::core::math::Vector::UNIT_X) == ::pe::core::math::Vector::UNIT_Y);

	CHECK(::pe::core::math::Vector::UNIT_Y.Cross(::pe::core::math::Vector::UNIT_Z) == ::pe::core::math::Vector::UNIT_X);
	CHECK(::pe::core::math::Vector::UNIT_Z.Cross(::pe::core::math::Vector::UNIT_Y) == -::pe::core::math::Vector::UNIT_X);
}

TEST_CASE("Matrix rotations", "[Coord system]")
{
	::pe::core::math::Matrix rotMat;

	// X rot
	rotMat.SetRotationX(90_deg);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_X == ::pe::core::math::Vector::UNIT_X);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_Y == ::pe::core::math::Vector::UNIT_Z);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_Z == -::pe::core::math::Vector::UNIT_Y);

	// Y rot
	rotMat.SetRotationY(90_deg);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_X == -::pe::core::math::Vector::UNIT_Z);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_Y == ::pe::core::math::Vector::UNIT_Y);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_Z == ::pe::core::math::Vector::UNIT_X);

	// Z rot
	rotMat.SetRotationZ(90_deg);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_X == ::pe::core::math::Vector::UNIT_Y);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_Y == -::pe::core::math::Vector::UNIT_X);
	CHECK(rotMat * ::pe::core::math::Vector::UNIT_Z == ::pe::core::math::Vector::UNIT_Z);
}

TEST_CASE("Quaternion rotations", "[Coord system]")
{
	::pe::core::math::Quaternion rotQuat;

	rotQuat.SetRotation(::pe::core::math::Vector::UNIT_X, 90_deg);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_X == ::pe::core::math::Vector::UNIT_X);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_Y == ::pe::core::math::Vector::UNIT_Z);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_Z == -::pe::core::math::Vector::UNIT_Y);

	rotQuat.SetRotation(::pe::core::math::Vector::UNIT_Y, 90_deg);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_X == -::pe::core::math::Vector::UNIT_Z);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_Y == ::pe::core::math::Vector::UNIT_Y);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_Z == ::pe::core::math::Vector::UNIT_X);

	// Z rot
	rotQuat.SetRotation(::pe::core::math::Vector::UNIT_Z, 90_deg);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_X == ::pe::core::math::Vector::UNIT_Y);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_Y == -::pe::core::math::Vector::UNIT_X);
	CHECK(rotQuat * ::pe::core::math::Vector::UNIT_Z == ::pe::core::math::Vector::UNIT_Z);
}

TEST_CASE("Matrix lookt at", "[Coord system]")
{
	::pe::core::math::Matrix lookMat;
	::pe::core::math::Matrix cmpMat;

	// look forward
	{
		lookMat.SetLookAt(::pe::core::math::Vector::ZERO, -::pe::core::math::Vector::UNIT_Z, ::pe::core::math::Vector::UNIT_Y);
		CHECK(lookMat == ::pe::core::math::Matrix::IDENTITY);
	}
	
	// look backward
	{
		lookMat.SetLookAt(::pe::core::math::Vector::ZERO, ::pe::core::math::Vector::UNIT_Z, ::pe::core::math::Vector::UNIT_Y);
		cmpMat.SetRotationY(180_deg);
		CHECK(lookMat == cmpMat);
	}

	// look right
	{
		lookMat.SetLookAt(::pe::core::math::Vector::ZERO, ::pe::core::math::Vector::UNIT_X, ::pe::core::math::Vector::UNIT_Y);
		cmpMat.SetRotationY(-90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look left
	{
		lookMat.SetLookAt(::pe::core::math::Vector::ZERO, -::pe::core::math::Vector::UNIT_X, ::pe::core::math::Vector::UNIT_Y);
		cmpMat.SetRotationY(90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look up
	{
		lookMat.SetLookAt(::pe::core::math::Vector::ZERO, ::pe::core::math::Vector::UNIT_Y, ::pe::core::math::Vector::UNIT_Z);
		cmpMat.SetRotationX(90_deg);
		CHECK(lookMat == cmpMat);
	}

	// look down
	{
		lookMat.SetLookAt(::pe::core::math::Vector::ZERO, -::pe::core::math::Vector::UNIT_Y, -::pe::core::math::Vector::UNIT_Z);
		cmpMat.SetRotationX(-90_deg);
		CHECK(lookMat == cmpMat);
	}
}

TEST_CASE("Quaternion lookt at", "[Coord system]")
{
	::pe::core::math::Quaternion lookQuat;
	::pe::core::math::Quaternion cmpQuat;

	// look forward
	{
		lookQuat = ::pe::core::math::Quaternion::LookAt(::pe::core::math::Vector::ZERO, -::pe::core::math::Vector::UNIT_Z, ::pe::core::math::Vector::UNIT_Y);
		CHECK(lookQuat == ::pe::core::math::Quaternion::IDENTITY);
	}

	// look backward
	{
		lookQuat = ::pe::core::math::Quaternion::LookAt(::pe::core::math::Vector::ZERO, ::pe::core::math::Vector::UNIT_Z, ::pe::core::math::Vector::UNIT_Y);
		cmpQuat.SetRotation(::pe::core::math::Vector::UNIT_Y, 180_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look right
	{
		lookQuat = ::pe::core::math::Quaternion::LookAt(::pe::core::math::Vector::ZERO, ::pe::core::math::Vector::UNIT_X, ::pe::core::math::Vector::UNIT_Y);
		cmpQuat.SetRotation(::pe::core::math::Vector::UNIT_Y, -90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look left
	{
		lookQuat = ::pe::core::math::Quaternion::LookAt(::pe::core::math::Vector::ZERO, -::pe::core::math::Vector::UNIT_X, ::pe::core::math::Vector::UNIT_Y);
		cmpQuat.SetRotation(::pe::core::math::Vector::UNIT_Y, 90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look up
	{
		lookQuat = ::pe::core::math::Quaternion::LookAt(::pe::core::math::Vector::ZERO, ::pe::core::math::Vector::UNIT_Y, ::pe::core::math::Vector::UNIT_Z);
		cmpQuat.SetRotation(::pe::core::math::Vector::UNIT_X, 90_deg);
		CHECK(lookQuat == cmpQuat);
	}

	// look down
	{
		lookQuat = ::pe::core::math::Quaternion::LookAt(::pe::core::math::Vector::ZERO, -::pe::core::math::Vector::UNIT_Y, -::pe::core::math::Vector::UNIT_Z);
		cmpQuat.SetRotation(::pe::core::math::Vector::UNIT_X, -90_deg);
		CHECK(lookQuat == cmpQuat);
	}
}