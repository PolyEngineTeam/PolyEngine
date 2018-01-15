#include "catch.hpp"

#include "StringBuilder.hpp"

using namespace Poly;

TEST_CASE("StringBuilder - primitive appendings", "[StringBuilder]") {
	String test = StringBuilder()
            .Append('c')
            .Append(50)
            .Append(2.34f)
            .Append(1.23456, 3)
            .ToString();
    String pattern = String("c502.3400001.235");
    REQUIRE(test == pattern);
    REQUIRE(test.GetLength() == 16);
}

TEST_CASE("String builder - string appendings", "[StringBuilder]") {
    StringBuilder sb = StringBuilder().Append("AAA");
    String test = StringBuilder()
            .Append(sb)
            .Append(String("BBB"))
            .ToString();
    String pattern = String("AAABBB");
    REQUIRE(test == pattern);
}

TEST_CASE("String builder - formatting", "[StringBuilder]") {
    String test = StringBuilder()
            .AppendFormat("A {} A {} A {}", String("BB"), String("CCC"), String("D"))
            .ToString();
    String pattern = String("A BB A CCC A D");
    REQUIRE(test == pattern);
}

TEST_CASE("String builder - auxilary functions", "[StringBuilder]") {
    StringBuilder sb = StringBuilder("Some string to be cleared");
    REQUIRE(sb[3] == 'e');
    sb.Clear();
    REQUIRE(sb.ToString() == "");
    REQUIRE(sb.GetLength() == 0);
}

TEST_CASE("String builder - Stealing", "[StringBuilder]") {
    String base = String("Some string to be stolen");
    StringBuilder sb = StringBuilder(base);
    String s = sb.Steal();
    REQUIRE(s == base);
    REQUIRE(sb.GetLength() == 0);
    REQUIRE(sb.ToString() == "");
}

TEST_CASE("String builder - ToString", "[StringBuilder]") {
    String base = String("Some string to be stolen");
    StringBuilder sb = StringBuilder(base);
    String s = sb.ToString();
    REQUIRE(s == base);
    REQUIRE(sb.GetLength() == s.GetLength());
    REQUIRE(sb.ToString() == s);
}
