#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/storage/CString.hpp>

using pe::core::storage::CString;

TEST_CASE("Constructor", "[CString]")
{
	const char* str = "My test string";
	CString cstr(str);
	REQUIRE(cstr.get() == str);

	const char* str2 = cstr;
	REQUIRE(str2 == str);
}

TEST_CASE("Indexing operator", "[CString]")
{
	CString cstr("Test string");

	CHECK(cstr[0] == 'T');
	CHECK(cstr[1] == 'e');
	CHECK(cstr[2] == 's');
	CHECK(cstr[3] == 't');
	CHECK(cstr[4] == ' ');
	CHECK(cstr[5] == 's');
	CHECK(cstr[6] == 't');
	CHECK(cstr[7] == 'r');
	CHECK(cstr[8] == 'i');
	CHECK(cstr[9] == 'n');
	CHECK(cstr[10] == 'g');
	CHECK(cstr[11] == '\0');
}

TEST_CASE("Length calculation", "[CString]")
{
	CString cstr1("Test string");
	CString cstr2("Other test string");

	REQUIRE(cstr1.length() == 11);
	REQUIRE(cstr2.length() == 17);
}


TEST_CASE("Equal comparison", "[CString]")
{
	CString cstr1("TEST");
	CString cstr2("TEST");
	CString cstr3("DIFFERENT");

	REQUIRE(cstr1 == cstr2);
	REQUIRE(cstr1 != cstr3);
	REQUIRE(cstr2 != cstr3);
}

TEST_CASE("Less/Greater comparison", "[CString]")
{
	CString cstr1("ABCD");
	CString cstr2("ABCE");

	REQUIRE(cstr1 < cstr2);
	REQUIRE(cstr1 <= cstr2);
	REQUIRE(cstr1 <= cstr1);

	REQUIRE(cstr2 > cstr1);
	REQUIRE(cstr2 >= cstr1);
	REQUIRE(cstr2 >= cstr2);
}