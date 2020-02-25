#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/storage/String.hpp>

TEST_CASE("String.From", "[String]") {
	::pe::core::storage::String sInt = ::pe::core::storage::String("-23");
	int i = -23;
	::pe::core::storage::String sInt2 = ::pe::core::storage::String::From(i);
	REQUIRE(sInt == sInt2);

	::pe::core::storage::String sFloat = ::pe::core::storage::String("-23.580000");
	float f = -23.58f;
	::pe::core::storage::String sFloat2 = ::pe::core::storage::String::From(f);
	REQUIRE(sFloat == sFloat2);

	::pe::core::storage::String sDouble = ::pe::core::storage::String("-23.580000");
	double d = -23.58;
	::pe::core::storage::String sDouble2 = ::pe::core::storage::String::From(d);
	REQUIRE(sDouble == sDouble2);

	::pe::core::storage::String s4Float = ::pe::core::storage::String("-23.5800");
	float f4 = -23.58f;
	::pe::core::storage::String s4Float2 = ::pe::core::storage::String::From(f4, 4);
	REQUIRE(s4Float == s4Float2);

	::pe::core::storage::String s5Double = ::pe::core::storage::String("-23.580");
	double d5 = -23.58;
	::pe::core::storage::String s5Double2 = ::pe::core::storage::String::From(d5, 3);
	REQUIRE(s5Double == s5Double2);

	::pe::core::storage::String sChar = ::pe::core::storage::String("c");
	char c = 'c';
	::pe::core::storage::String sChar2 = ::pe::core::storage::String::From(c);
	REQUIRE(sChar == sChar2);

	::pe::core::storage::String sCstr = ::pe::core::storage::String("aSdf_ 15");
	::pe::core::storage::String sCstr2 = ::pe::core::storage::String::From("aSdf_ 15");
	REQUIRE(sCstr == sCstr2);
}

TEST_CASE("String operations", "[String]") {
	::pe::core::storage::String test = ::pe::core::storage::String("@ALZ[allz'{");

	
	::pe::core::storage::String lower = ::pe::core::storage::String("@alz[allz'{");
	::pe::core::storage::String upper = ::pe::core::storage::String("@ALZ[ALLZ'{");
	::pe::core::storage::String lowerTest = test.ToLower();
	::pe::core::storage::String upperTest = test.ToUpper();
	REQUIRE(lower == lowerTest);
	REQUIRE(upper == upperTest);
	

	::pe::core::storage::String empty = ::pe::core::storage::String("");
	REQUIRE(!test.IsEmpty());
	REQUIRE(empty.IsEmpty());

	::pe::core::storage::String replace = ::pe::core::storage::String("@ALZ[aWWzD{");
	::pe::core::storage::String replaced = test.Replace('l', 'W').Replace('\'', 'D');
	REQUIRE(replace == replaced);
	::pe::core::storage::String replaceString = ::pe::core::storage::String("@ATeStllz'{");
	::pe::core::storage::String replacedString = test.Replace("LZ[a", "TeSt");
	REQUIRE(replaceString == replacedString);

	REQUIRE(test.StartsWith('@'));
	REQUIRE(!test.StartsWith('!'));
	REQUIRE(test.EndsWith('{'));
	REQUIRE(!test.EndsWith('!'));

	::pe::core::storage::String substring1 = ::pe::core::storage::String("@ALZ");
	::pe::core::storage::String substring2 = ::pe::core::storage::String("LZ[all");
	REQUIRE(test.Substring(4) == substring1);
	REQUIRE(test.Substring(2, 8) == substring2);

	::pe::core::storage::String trimTest0 = ::pe::core::storage::String(" \nsda\t \r");
	::pe::core::storage::String trimmed0 = ::pe::core::storage::String("sda");
	REQUIRE(trimTest0.GetTrimmed() == trimmed0);

	::pe::core::storage::String trimTest1 = ::pe::core::storage::String("");
	::pe::core::storage::String trimmed1 = ::pe::core::storage::String("");
	REQUIRE(trimTest1.GetTrimmed() == trimmed1);

	::pe::core::storage::String trimTest2 = ::pe::core::storage::String("\r");
	::pe::core::storage::String trimmed2 = ::pe::core::storage::String("");
	REQUIRE(trimTest2.GetTrimmed() == trimmed2);

	::pe::core::storage::String trimTest3 = ::pe::core::storage::String(" \t  \r\n \r \n");
	::pe::core::storage::String trimmed3 = ::pe::core::storage::String("");
	REQUIRE(trimTest3.GetTrimmed() == trimmed3);

	::pe::core::storage::String strArr[3];
	strArr[0] = ::pe::core::storage::String("String0");
	strArr[1] = ::pe::core::storage::String("String1");
	strArr[2] = ::pe::core::storage::String("String2");
	::pe::core::storage::String separator = ::pe::core::storage::String("__separator__");
	char charSeparator = 's';
	::pe::core::storage::String joined = strArr[0] + separator + strArr[1] + separator + strArr[2];
	::pe::core::storage::String charJoined = strArr[0] + charSeparator + strArr[1] + charSeparator + strArr[2];
	REQUIRE(::pe::core::storage::String::Join(strArr, 3, separator) == joined);
	REQUIRE(::pe::core::storage::String::Join(strArr, 3, charSeparator) == charJoined);

	::pe::core::storage::String containsTest = ::pe::core::storage::String("al");
	REQUIRE(test.Contains(containsTest) == true);

	::pe::core::storage::String notContainsTest = ::pe::core::storage::String("Z[allz'/");
	REQUIRE(test.Contains(notContainsTest) == false);
}

TEST_CASE("UTF-8 string normalization and collation tests", "[String]")
{
	::pe::core::storage::String utf8Literal("śląsk");
	::pe::core::storage::String uft8Escaped("\xC5\x9B\x6C\xC4\x85\x73\x6B");
	REQUIRE(utf8Literal == uft8Escaped);

	auto codepointUTF8One = ::pe::core::storage::String::fromUTF8("\u4eba\u53e3\u3058\u3093\u3053\u3046\u306b\u81be\u7099\u304b\u3044\u3057\u3083\u3059\u308b");
	auto codepointUTF8Two = ::pe::core::storage::String::fromUTF8("\u4eba\u53e3\u3058\u3093\u3053\u3046\u306b\u81be\uf9fb\u304b\u3044\u3057\u3083\u3059\u308b");
	REQUIRE(codepointUTF8One == codepointUTF8Two);
	REQUIRE(codepointUTF8One.CmpBytes(codepointUTF8Two) == true);

	auto normalizedUTF8One = ::pe::core::storage::String::fromUTF8("\xC3\xA4\x00");
	auto normalizedUTF8Two = ::pe::core::storage::String::fromUTF8("\x61\xCC\x88\x00");
	REQUIRE(normalizedUTF8One == normalizedUTF8Two);
	REQUIRE(normalizedUTF8One.CmpBytes(normalizedUTF8Two) == true);

	::pe::core::storage::String regularStringOne("\xC3\xA4\x00"); // from regular string they will not have proper representation as bytes, no normalization
	::pe::core::storage::String regularStringTwo("\x61\xCC\x88\x00");
	REQUIRE(regularStringTwo == regularStringTwo);
	REQUIRE(regularStringOne.CmpBytes(regularStringTwo) == false);

	//auto invalidASCII = ::pe::core::storage::String::fromASCII("\xC3\xA4\x00");
}

TEST_CASE("UTF-8 string conversion tests", "[String]")
{
	::pe::core::storage::String utf8polish("śląsk");
	auto asciiConverted = utf8polish.toASCII();
	REQUIRE(asciiConverted == "slask");

	::pe::core::storage::String utf8capital("ŚPIĄC");
	asciiConverted = utf8capital.toASCII();
	REQUIRE(asciiConverted == "SPIAC");

	::pe::core::storage::String hangulTest("김, 국삼");
	asciiConverted = hangulTest.toASCII();
	REQUIRE(asciiConverted == "gim, gugsam"); // does not preserve capital letters
	
	::pe::core::storage::String kanjiTest("たけだ, まさゆき");
	asciiConverted = kanjiTest.toASCII();
	REQUIRE(asciiConverted == "takeda, masayuki"); // does not preserve capital letters

	::pe::core::storage::String cyrilicTest("Θεοδωράτου, Ελένη");
	asciiConverted = cyrilicTest.toASCII();
	REQUIRE(asciiConverted == "Theodoratou, Elene"); // this preserves it somehow
}