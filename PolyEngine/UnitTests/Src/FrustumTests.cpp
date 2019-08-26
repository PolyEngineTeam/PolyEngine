#include <pe/Defines.hpp>
#include <catch.hpp>
#include <pe/core/math/Frustum.hpp>
#include <pe/core/math/Quaternion.hpp>
#include <pe/core/math/Random.hpp>
#include <pe/core/utils/Logger.hpp>

float maxValue(float zDepth, ::pe::core::math::Angle fov)
{
	return -zDepth * Tan(fov / 2);
}

std::pair<::pe::core::math::Vector, bool> RandomBoxPosition(const ::pe::core::math::Vector& size, ::pe::core::math::Angle fov, float aspect, float zNear, float zFar)
{
	::pe::core::math::Vector position = ::pe::core::math::RandomVectorRange(-200, 50);
	if (position.Z != ::pe::core::math::Clamp(position.Z, -zFar, -zNear))
	{
		return { position, false }; //not correct
	}

	const float xExtend = maxValue(position.Z, fov * aspect);
	const float yExtend = maxValue(position.Z, fov);

	const bool correct = position.X == ::pe::core::math::Clamp(position.X, -xExtend - size.X, xExtend)
		&& position.Y == ::pe::core::math::Clamp(position.Y, -yExtend - size.Y, yExtend);
	return { position, correct };
}

TEST_CASE("Frustum position tests", "[Frustum]") {
	const ::pe::core::math::Angle fov = 60_deg;
	const float aspect = 4.f / 3.f;
	const float zNear = 1.f;
	const float zFar = 1000.f;
	const size_t testCount = 10000;
	
	::pe::core::math::Frustum f(fov, aspect, zNear, zFar);
	::pe::core::math::Vector size(1, 1, 1);

	for (size_t i = 0; i < testCount; ++i)
	{
		auto[pos, correct] = RandomBoxPosition(size, fov, aspect, zNear, zFar);
		::pe::core::math::AABox box(pos, size);
		::pe::core::math::Frustum::eObjectLocation loc = f.GetObjectLocation(box, ::pe::core::math::Matrix());
		
		const bool match = correct == (loc == ::pe::core::math::Frustum::eObjectLocation::INSIDE || loc == ::pe::core::math::Frustum::eObjectLocation::PARTIALY_INSIDE);
		CHECK(match);
	}
	
}