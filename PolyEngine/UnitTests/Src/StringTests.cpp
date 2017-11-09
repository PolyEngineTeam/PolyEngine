#include "catch.hpp"

#include "String.hpp"

using namespace Poly;

TEST_CASE("String.From", "[String]") {
	String sInt = String("-23");
	int i = -23;
	String sInt2 = String::From(i);
	REQUIRE(sInt == sInt2);

	String sFloat = String("-23.580000");
	float f = -23.58f;
	String sFloat2 = String::From(f);
	REQUIRE(sFloat == sFloat2);

	String sDouble = String("-23.580000");
	double d = -23.58;
	String sDouble2 = String::From(d);
	REQUIRE(sDouble == sDouble2);

	String s4Float = String("-23.5800");
	float f4 = -23.58f;
	String s4Float2 = String::From(f, 4);
	REQUIRE(s4Float == s4Float2);

	String s5Double = String("-23.580");
	double d5 = -23.58;
	String s5Double2 = String::From(d, 3);
	REQUIRE(s5Double == s5Double2);

	String sChar = String("c");
	char c = 'c';
	String sChar2 = String::From(c);
	REQUIRE(sChar == sChar2);

	String sCstr = String("aSdf_ 15");
	String sCstr2 = String::From("aSdf_ 15");
	REQUIRE(sCstr == sCstr2);
}

TEST_CASE("String operations", "[String]") {
	String test = String("@ALZ[allz'{");

	
	String lower = String("@alz[allz'{");
	String upper = String("@ALZ[ALLZ'{");
	String lowerTest = test.ToLower();
	String upperTest = test.ToUpper();
	REQUIRE(lower == lowerTest);
	REQUIRE(upper == upperTest);
	

	String empty = String("");
	REQUIRE(!test.IsEmpty());
	REQUIRE(empty.IsEmpty());

	String replace = String("@ALZ[aWWzD{");
	String replaced = test.Replace('l', 'W').Replace('\'', 'D');
	REQUIRE(replace == replaced);
	String replaceString = String("@ATeStllz'{");
	String replacedString = test.Replace("LZ[a", "TeSt");
	REQUIRE(replaceString == replacedString);

	REQUIRE(test.StartsWith('@'));
	REQUIRE(!test.StartsWith('!'));
	REQUIRE(test.EndsWith('{'));
	REQUIRE(!test.EndsWith('!'));

	String substring1 = String("@ALZ");
	String substring2 = String("LZ[all");
	REQUIRE(test.Substring(4) == substring1);
	REQUIRE(test.Substring(2, 8) == substring2);

	String trimTest = String(" \nsda\t \r");
	String trimmed = String("sda");
	REQUIRE(trimTest.GetTrimmed() == trimmed);

	String strArr[3];
	strArr[0] = String("String0");
	strArr[1] = String("String1");
	strArr[2] = String("String2");
	String separator = String("__separator__");
	char charSeparator = 's';
	String joined = strArr[0] + separator + strArr[1] + separator + strArr[2];
	String charJoined = strArr[0] + charSeparator + strArr[1] + charSeparator + strArr[2];
	REQUIRE(String::Join(strArr, 3, separator) == joined);
	REQUIRE(String::Join(strArr, 3, charSeparator) == charJoined);

	String containsTest = String("al");
	REQUIRE(test.Contains(containsTest) == true);

	String notContainsTest = String("Z[allz'/");
	REQUIRE(test.Contains(notContainsTest) == false);
}