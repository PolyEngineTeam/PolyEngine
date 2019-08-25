#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/math/Angle.hpp>
//TODO implement

using namespace Poly;

TEST_CASE("Angle constructors", "[Angle]") {
  // degree constructor
  core::math::Angle a1 = core::math::Angle::FromDegrees(180.0f);
  REQUIRE(Cmpf(a1.AsDegrees(), 180.0f) == true);

  // radians constructor
  core::math::Angle a2 = core::math::Angle::FromRadians((float)M_PI);
  REQUIRE(Cmpf(a2.AsRadians(), (float)M_PI) == true);
  REQUIRE(Cmpf(a2.AsDegrees(), 180.0f) == true);

  // User defined literals
  core::math::Angle a3 = 180_deg;
  core::math::Angle a4 = 180.0_deg;
  REQUIRE(Cmpf(a3.AsDegrees(), 180.0f) == true);
  REQUIRE(Cmpf(a3.AsRadians(), (float)M_PI) == true);
  REQUIRE(Cmpf(a4.AsDegrees(), 180.0f) == true);
  REQUIRE(Cmpf(a4.AsRadians(), (float)M_PI) == true);

  core::math::Angle a5 = 3.14159265358979323846264338327950288_rad;
  REQUIRE(Cmpf(a5.AsDegrees(), 180.0f) == true);
  REQUIRE(Cmpf(a5.AsRadians(), (float)M_PI) == true);
}

TEST_CASE("Comparison operators", "[Angle]") {
  REQUIRE(180_deg == 180_deg);
  REQUIRE(180_deg == 3.14159265358979323846264338327950288_rad);

  REQUIRE(180_deg != 90_deg);
  REQUIRE(180_deg != 270_deg);
}
