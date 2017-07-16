#include "catch.hpp"

#include "String.hpp"

using namespace Poly;

TEST_CASE("String.From", "[String]") {
	String sInt = String("-23");
	int i = -23;
	String* sInt2 = String::From(i);
	REQUIRE(sInt == *sInt2);

	String sFloat = String("-23.580000");
	float f = -23.58f;
	String* sFloat2 = String::From(f);
	REQUIRE(sFloat == *sFloat2);

	String sDouble = String("-23.580000");
	double d = -23.58;
	String* sDouble2 = String::From(d);
	REQUIRE(sDouble == *sDouble2);

	String sChar = String("c");
	char c = 'c';
	String* sChar2 = String::From(c);
	REQUIRE(sChar == *sChar2);

	String sCstr = String("aSdf_ 15");
	String* sCstr2 = String::From("aSdf_ 15");
	REQUIRE(sCstr == *sCstr2);

}