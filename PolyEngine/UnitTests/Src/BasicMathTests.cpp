#include <catch.hpp>

#include <Defines.hpp>
#include <BasicMath.hpp>

constexpr auto INF_FLOAT = std::numeric_limits<float>::infinity();
constexpr auto NAN_FLOAT = std::numeric_limits<float>::quiet_NaN();
constexpr auto LOW_FLOAT = std::numeric_limits<float>::denorm_min();

using namespace Poly;

constexpr bool Cmpft(float a, float b, float epsilon = 0.00001f) {
	return Cmpf(a, b, epsilon, true);
}

TEST_CASE("Floating-point equality comparisons", "[FloatCmp]") {

	SECTION("Large positive numbers") {
		CHECK(Cmpft(1000000.0f, 1000001.0f));
		CHECK(Cmpft(1000001.0f, 1000000.0f));
		CHECK_FALSE(Cmpft(10000.0f, 10001.0f));
		CHECK_FALSE(Cmpft(10001.0f, 10000.0f));
	}

	SECTION("Large negative numbers") {
		CHECK(Cmpft(-1000000.0f, -1000001.0f));
		CHECK(Cmpft(-1000001.0f, -1000000.0f));
		CHECK_FALSE(Cmpft(-10000.0f, -10001.0f));
		CHECK_FALSE(Cmpft(-10001.0f, -10000.0f));
	}

	SECTION("Numbers around 1.0f") {
		CHECK(Cmpft(1.0000001f, 1.0000002f));
		CHECK(Cmpft(1.0000002f, 1.0000001f));
		CHECK_FALSE(Cmpft(1.0002f, 1.0001f));
		CHECK_FALSE(Cmpft(1.0001f, 1.0002f));
	}

	SECTION("Numbers around -1.0f") {
		CHECK(Cmpft(-1.0000001f, -1.0000002f));
		CHECK(Cmpft(-1.0000002f, -1.0000001f));
		CHECK_FALSE(Cmpft(-1.0002f, -1.0001f));
		CHECK_FALSE(Cmpft(-1.0001f, -1.0002f));
	}

	SECTION("Numbers between 0.0f and 1.0f") {
		CHECK(Cmpft(0.000000001000001f, 0.000000001000002f));
		CHECK(Cmpft(0.000000001000002f, 0.000000001000001f));
		CHECK_FALSE(Cmpft(0.000000000001002f, 0.000000000001001f));
		CHECK_FALSE(Cmpft(0.000000000001001f, 0.000000000001002f));
	}

	SECTION("Numbers between 0.0f and -1.0f") {
		CHECK(Cmpft(-0.000000001000001f, -0.000000001000002f));
		CHECK(Cmpft(-0.000000001000002f, -0.000000001000001f));
		CHECK_FALSE(Cmpft(-0.000000000001002f, -0.000000000001001f));
		CHECK_FALSE(Cmpft(-0.000000000001001f, -0.000000000001002f));
	}

	SECTION("Comparisons with 0.0f") {
		CHECK(Cmpft(0.0f, 0.0f));
		CHECK(Cmpft(0.0f, -0.0f));
		CHECK(Cmpft(-0.0f, -0.0f));
		CHECK(Cmpft(0.00000001f, 0.0f));
		CHECK(Cmpft(0.0f, 0.00000001f));
		CHECK(Cmpft(-0.00000001f, 0.0f));
		CHECK(Cmpft(0.0f, -0.00000001f));

		CHECK(Cmpft(0.0f, 1e-40f, 0.01f));
		CHECK(Cmpft(1e-40f, 0.0f, 0.01f));
		CHECK(Cmpft(1e-40f, 0.0f, 0.000001f));
		CHECK(Cmpft(0.0f, 1e-40f, 0.000001f));

		CHECK(Cmpft(0.0f, -1e-40f, 0.1f));
		CHECK(Cmpft(-1e-40f, 0.0f, 0.1f));
		CHECK(Cmpft(-1e-40f, 0.0f, 0.00000001f));
		CHECK(Cmpft(0.0f, -1e-40f, 0.00000001f));
		CHECK_FALSE(Cmpft(-0.0001f, 0.0f, 0.00000001f));
		CHECK_FALSE(Cmpft(0.0f, -0.0001f, 0.00000001f));
	}

	SECTION("Extreme values (overflow potential)") {
		CHECK(Cmpft(MAX_FLOAT, MAX_FLOAT));
		CHECK_FALSE(Cmpft(MAX_FLOAT, -MAX_FLOAT));
		CHECK_FALSE(Cmpft(-MAX_FLOAT, MAX_FLOAT));
		CHECK_FALSE(Cmpft(MAX_FLOAT, MAX_FLOAT / 2));
		CHECK_FALSE(Cmpft(MAX_FLOAT, -MAX_FLOAT / 2));
		CHECK_FALSE(Cmpft(-MAX_FLOAT, MAX_FLOAT / 2));
	}

	SECTION("Infinities") {
		CHECK(Cmpft(INF_FLOAT, INF_FLOAT));
		CHECK(Cmpft(-INF_FLOAT, -INF_FLOAT));
		CHECK_FALSE(Cmpft(-INF_FLOAT, INF_FLOAT));
		CHECK_FALSE(Cmpft(INF_FLOAT, MAX_FLOAT));
		CHECK_FALSE(Cmpft(-INF_FLOAT, -MAX_FLOAT));
	}

	SECTION("NaN values") {
		CHECK_FALSE(Cmpft(NAN_FLOAT, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, 0.0f));
		CHECK_FALSE(Cmpft(-0.0f, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, -0.0f));
		CHECK_FALSE(Cmpft(0.0f, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, INF_FLOAT));
		CHECK_FALSE(Cmpft(INF_FLOAT, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, -INF_FLOAT));
		CHECK_FALSE(Cmpft(-INF_FLOAT, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, MAX_FLOAT));
		CHECK_FALSE(Cmpft(MAX_FLOAT, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, -MAX_FLOAT));
		CHECK_FALSE(Cmpft(-MAX_FLOAT, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, LOW_FLOAT));
		CHECK_FALSE(Cmpft(LOW_FLOAT, NAN_FLOAT));
		CHECK_FALSE(Cmpft(NAN_FLOAT, -LOW_FLOAT));
		CHECK_FALSE(Cmpft(-LOW_FLOAT, NAN_FLOAT));
	}

	SECTION("Numbers on opposite sides of 0.0f") {
		CHECK_FALSE(Cmpft(1.000000001f, -1.0f));
		CHECK_FALSE(Cmpft(-1.0f, 1.000000001f));
		CHECK_FALSE(Cmpft(-1.000000001f, 1.0f));
		CHECK_FALSE(Cmpft(1.0f, -1.000000001f));
		CHECK(Cmpft(10.0f * LOW_FLOAT, 10.0f * -LOW_FLOAT));
		CHECK(Cmpft(10000.0f * LOW_FLOAT, 10000.0f * -LOW_FLOAT));
		CHECK_FALSE(Cmpft(-0.00001f, 0.00003f));
	}

	SECTION("Numbers very close to 0.0f") {
		CHECK(Cmpft(LOW_FLOAT, LOW_FLOAT));
		CHECK(Cmpft(LOW_FLOAT, -LOW_FLOAT));
		CHECK(Cmpft(-LOW_FLOAT, LOW_FLOAT));
		CHECK(Cmpft(LOW_FLOAT, 0.0f));
		CHECK(Cmpft(0.0f, LOW_FLOAT));
		CHECK(Cmpft(-LOW_FLOAT, 0.0f));
		CHECK(Cmpft(0.0f, -LOW_FLOAT));

		CHECK_FALSE(Cmpft(0.000000001f, -LOW_FLOAT));
		CHECK_FALSE(Cmpft(0.000000001f, LOW_FLOAT));
		CHECK_FALSE(Cmpft(LOW_FLOAT, 0.000000001f));
		CHECK_FALSE(Cmpft(-LOW_FLOAT, 0.000000001f));
	}
}
