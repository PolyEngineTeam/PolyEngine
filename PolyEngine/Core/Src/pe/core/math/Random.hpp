#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>
#include <pe/core/math/Vector.hpp>

namespace Poly {

	inline void RandomSetSeed(int value)
	{
		srand(value);
	}

	inline float Random()
	{
		float rnd = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return rnd - floor(rnd);
	}

	template <typename T> inline T RandomRange(const T& min, const T& max)
	{
		float rnd = Random();
		return Lerp(min, max, rnd);
	}

	inline Vector RandomVectorRange(float min, float max)
	{
		return Vector(RandomRange(min, max), RandomRange(min, max), RandomRange(min, max));
	}

	inline Vector RandomVector()
	{
		return RandomVectorRange(0.0f, 1.0f);
	}
}
