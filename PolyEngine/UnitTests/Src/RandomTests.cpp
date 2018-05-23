#include <catch.hpp>

#include <Defines.hpp>
#include <Math/BasicMath.hpp>
#include <Math/Random.hpp>

using namespace Poly;

TEST_CASE("Generation of pesudorandom numbers", "Random") {

	SECTION("Getting random numbers from correct range") {
		RandomSetSeed(0);
		float rndMin = 0.5f;
		float rndMax = 0.5f;
		for (int i = 0; i < 1000; ++i)
		{
			float rnd = Random();
			rndMin = fmin(rndMin, rnd);
			rndMax = fmax(rndMax, rnd);
		}
		REQUIRE(rndMin >= 0.0f);
		REQUIRE(rndMax <= 1.0f);
	}

	SECTION("Setting random seed") {
		RandomSetSeed(42);
		float rnd00 = Random();
		float rnd01 = RandomRange(-1.0f, 1.0f);
	
		RandomSetSeed(0);
		float rnd10 = Random();
		float rnd11 = RandomRange(-1.0f, 1.0f);
	
		RandomSetSeed(42);
		float rnd20 = Random();
		float rnd21 = RandomRange(-1.0f, 1.0f);
	
		REQUIRE(rnd00 == rnd20);
		REQUIRE(rnd01 == rnd21);
	}
}
