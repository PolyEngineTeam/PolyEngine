#include <catch.hpp>

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>
#include <pe/core/math/Random.hpp>

TEST_CASE("Generation of pesudorandom numbers", "Random") {

	SECTION("Getting random numbers from correct range") {
		::pe::core::math::RandomSetSeed(0);
		float rndMin = 0.5f;
		float rndMax = 0.5f;
		for (int i = 0; i < 1000; ++i)
		{
			float rnd = ::pe::core::math::Random();
			rndMin = fmin(rndMin, rnd);
			rndMax = fmax(rndMax, rnd);
		}
		REQUIRE(rndMin >= 0.0f);
		REQUIRE(rndMax <= 1.0f);
	}

	SECTION("Setting random seed") {
		::pe::core::math::RandomSetSeed(42);
		float rnd00 = ::pe::core::math::Random();
		float rnd01 = ::pe::core::math::RandomRange(-1.0f, 1.0f);
	
		::pe::core::math::RandomSetSeed(0);
		::pe::core::math::Random();
		::pe::core::math::RandomRange(-1.0f, 1.0f);
	
		::pe::core::math::RandomSetSeed(42);
		float rnd20 = ::pe::core::math::Random();
		float rnd21 = ::pe::core::math::RandomRange(-1.0f, 1.0f);
	
		REQUIRE(rnd00 == rnd20);
		REQUIRE(rnd01 == rnd21);
	}
}
