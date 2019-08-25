#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/storage/String.hpp>

using namespace Poly;

TEST_CASE("String.From", "[String]") {
	core::storage::String sInt = core::storage::String("-23");
	int i = -23;
	core::storage::String sInt2 = core::storage::String::From(i);
	REQUIRE(sInt == sInt2);

	core::storage::String sFloat = core::storage::String("-23.580000");
	float f = -23.58f;
	core::storage::String sFloat2 = core::storage::String::From(f);
	REQUIRE(sFloat == sFloat2);

	core::storage::String sDouble = core::storage::String("-23.580000");
	double d = -23.58;
	core::storage::String sDouble2 = core::storage::String::From(d);
	REQUIRE(sDouble == sDouble2);

	core::storage::String s4Float = core::storage::String("-23.5800");
	float f4 = -23.58f;
	core::storage::String s4Float2 = core::storage::String::From(f4, 4);
	REQUIRE(s4Float == s4Float2);

	core::storage::String s5Double = core::storage::String("-23.580");
	double d5 = -23.58;
	core::storage::String s5Double2 = core::storage::String::From(d5, 3);
	REQUIRE(s5Double == s5Double2);

	core::storage::String sChar = core::storage::String("c");
	char c = 'c';
	core::storage::String sChar2 = core::storage::String::From(c);
	REQUIRE(sChar == sChar2);

	core::storage::String sCstr = core::storage::String("aSdf_ 15");
	core::storage::String sCstr2 = core::storage::String::From("aSdf_ 15");
	REQUIRE(sCstr == sCstr2);
}

TEST_CASE("String operations", "[String]") {
	core::storage::String test = core::storage::String("@ALZ[allz'{");

	
	core::storage::String lower = core::storage::String("@alz[allz'{");
	core::storage::String upper = core::storage::String("@ALZ[ALLZ'{");
	core::storage::String lowerTest = test.ToLower();
	core::storage::String upperTest = test.ToUpper();
	REQUIRE(lower == lowerTest);
	REQUIRE(upper == upperTest);
	

	core::storage::String empty = core::storage::String("");
	REQUIRE(!test.IsEmpty());
	REQUIRE(empty.IsEmpty());

	core::storage::String replace = core::storage::String("@ALZ[aWWzD{");
	core::storage::String replaced = test.Replace('l', 'W').Replace('\'', 'D');
	REQUIRE(replace == replaced);
	core::storage::String replaceString = core::storage::String("@ATeStllz'{");
	core::storage::String replacedString = test.Replace("LZ[a", "TeSt");
	REQUIRE(replaceString == replacedString);

	REQUIRE(test.StartsWith('@'));
	REQUIRE(!test.StartsWith('!'));
	REQUIRE(test.EndsWith('{'));
	REQUIRE(!test.EndsWith('!'));

	core::storage::String substring1 = core::storage::String("@ALZ");
	core::storage::String substring2 = core::storage::String("LZ[all");
	REQUIRE(test.Substring(4) == substring1);
	REQUIRE(test.Substring(2, 8) == substring2);

	core::storage::String trimTest0 = core::storage::String(" \nsda\t \r");
	core::storage::String trimmed0 = core::storage::String("sda");
	REQUIRE(trimTest0.GetTrimmed() == trimmed0);

	core::storage::String trimTest1 = core::storage::String("");
	core::storage::String trimmed1 = core::storage::String("");
	REQUIRE(trimTest1.GetTrimmed() == trimmed1);

	core::storage::String trimTest2 = core::storage::String("\r");
	core::storage::String trimmed2 = core::storage::String("");
	REQUIRE(trimTest2.GetTrimmed() == trimmed2);

	core::storage::String trimTest3 = core::storage::String(" \t  \r\n \r \n");
	core::storage::String trimmed3 = core::storage::String("");
	REQUIRE(trimTest3.GetTrimmed() == trimmed3);

	core::storage::String strArr[3];
	strArr[0] = core::storage::String("String0");
	strArr[1] = core::storage::String("String1");
	strArr[2] = core::storage::String("String2");
	core::storage::String separator = core::storage::String("__separator__");
	char charSeparator = 's';
	core::storage::String joined = strArr[0] + separator + strArr[1] + separator + strArr[2];
	core::storage::String charJoined = strArr[0] + charSeparator + strArr[1] + charSeparator + strArr[2];
	REQUIRE(core::storage::String::Join(strArr, 3, separator) == joined);
	REQUIRE(core::storage::String::Join(strArr, 3, charSeparator) == charJoined);

	core::storage::String containsTest = core::storage::String("al");
	REQUIRE(test.Contains(containsTest) == true);

	core::storage::String notContainsTest = core::storage::String("Z[allz'/");
	REQUIRE(test.Contains(notContainsTest) == false);
}