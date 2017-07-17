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

TEST_CASE("String operations", "[STRING]") {
	String test = String("@ALZ[allz'{");

	String lower = String("@alz[allz'{");
	String upper = String("@ALZ[ALLZ'{");
	String* lowerTest = test.ToLower();
	String* upperTest = test.ToUpper();
	REQUIRE(lower == *lowerTest);
	REQUIRE(upper == *upperTest);

	String empty = String("");
	REQUIRE(test.IsEmpty());
	REQUIRE(!empty.IsEmpty());

	String replace = String("@ALZ[aWWzD{");
	String* replaced = test.Replace('l', 'W')->Replace('\'', 'D');
	REQUIRE(replace == *replaced);

	REQUIRE(test.StartsWith('@'));
	REQUIRE(!test.StartsWith('!'));
	REQUIRE(test.EndsWith('{'));
	REQUIRE(!test.EndsWith('!'));

	String substring1 = String("@ALZ");
	String substring2 = String("LZ[all");
	REQUIRE(*(test.Substring(4)) == substring1);
	REQUIRE(*(test.Substring(2, 8)) == substring2);

	String trimTest = String(" \nsda\t \r");
	String trimmed = String("sda");
	REQUIRE(*(trimTest.GetTrimmed()) == trimmed);
}