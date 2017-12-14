#include "catch.hpp"

#include "StringBuilder.hpp"

using namespace Poly;

TEST_CASE("StringBuilder appendings", "[StringBuilder]") {
	StringBuilder sb = StringBuilder();
    REQUIRE(sb.ToString() == "");
    sb.Append('a');
    REQUIRE(sb.ToString() == "a");
    sb.Append('b');
    REQUIRE(sb.ToString() == "ab");
    sb.Append("ccc").Append("dd");
    REQUIRE(sb.ToString() == "abcccdd");
}
