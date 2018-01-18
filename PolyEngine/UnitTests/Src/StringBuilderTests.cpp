#include "catch.hpp"

#include "StringBuilder.hpp"

using namespace Poly;

TEST_CASE("Basic", "[StringBuilder]")
{
	StringBuilder sb;
	CHECK(sb.StealString() == "");

	sb.Append('a');
	CHECK(sb.StealString() == "a");

	sb.Append("abc");
	CHECK(sb.StealString() == "abc");

	sb.Append(String("abc"));
	CHECK(sb.StealString() == "abc");

	sb.Append(123);
	CHECK(sb.StealString() == "123");

	sb.Append(123L);
	CHECK(sb.StealString() == "123");

	sb.Append(-123);
	CHECK(sb.StealString() == "-123");

	sb.Append(-123L);
	CHECK(sb.StealString() == "-123");

	sb.Append(1.5f, 6);
	CHECK(sb.StealString() == "1.500000");

	sb.Append(1.5f, 1);
	CHECK(sb.StealString() == "1.5");

	sb.Append(1.5, 6);
	CHECK(sb.StealString() == "1.500000");

	sb.Append(1.5, 1);
	CHECK(sb.StealString() == "1.5");
}