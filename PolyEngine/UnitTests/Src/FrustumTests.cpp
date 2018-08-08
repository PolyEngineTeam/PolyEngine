#include <Defines.hpp>
#include <catch.hpp>
#include <Math/Frustum.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Random.hpp>
#include <Utils/Logger.hpp>

using namespace Poly;

float maxValue(float zDepth, Angle fov)
{
	return -zDepth * Tan(fov / 2);
}

std::pair<Vector, bool> RandomBoxPosition(const Vector& size, Angle fov, float aspect, float zNear, float zFar)
{
	//const bool
	const float maxFlt = std::numeric_limits<float>::max();

	Vector position = Poly::RandomVectorRange(-200, 50);
	if (position.Z != Clamp(position.Z, -zFar, -zNear))
	{
		return { position, false }; //not correct
	}

	const float xExtend = maxValue(position.Z, fov * aspect);
	const float yExtend = maxValue(position.Z, fov);

	const bool correct = position.X == Clamp(position.X, -xExtend - size.X, xExtend)
		&& position.Y == Clamp(position.Y, -yExtend - size.Y, yExtend);
	return { position, correct };
}

TEST_CASE("Frustum position tests", "[Frustum]") {
	const Angle fov = 60_deg;
	const float aspect = 4.f / 3.f;
	const float zNear = 1.f;
	const float zFar = 1000.f;
	const size_t testCount = 10000;
	
	Frustum f(fov, aspect, zNear, zFar);
	Vector size(1, 1, 1);

	for (size_t i = 0; i < testCount; ++i)
	{
		auto[pos, correct] = RandomBoxPosition(size, fov, aspect, zNear, zFar);
		AABox box(pos, size);
		Frustum::eObjectLocation loc = f.GetObjectLocation(box, Matrix());
		
		const bool match = correct == (loc == Frustum::eObjectLocation::INSIDE || loc == Frustum::eObjectLocation::PARTIALY_INSIDE);
		CHECK(match);
	}
	
}