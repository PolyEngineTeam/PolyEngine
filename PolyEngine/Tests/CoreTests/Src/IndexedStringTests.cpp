#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/storage/String.hpp>
#include <pe/core/storage/IndexedString.hpp>

using pe::core::storage::String;
using pe::core::storage::IndexedString;

TEST_CASE("Construction", "[IndexedString]")
{
	IndexedString str("TEST1");
	IndexedString str2("TEST1");
	REQUIRE(str.get() == String("TEST1"));
	CHECK(str == str2);
}

TEST_CASE("Copy construction", "[IndexedString]")
{
	IndexedString str("TEST1");
	IndexedString str2(str);
	REQUIRE(str.get() == str2.get());
	CHECK(str == str2);
}

TEST_CASE("Assignment", "[IndexedString]")
{
	IndexedString str("TEST1");
	IndexedString str2("TEST2");
	REQUIRE(str.get() != str2.get());
	REQUIRE(str != str2);

	str2 = str;
	REQUIRE(str.get() == str2.get());
	CHECK(str == str2);
}

TEST_CASE("Comparison", "[IndexedString]")
{
	IndexedString str1("TEST1");
	IndexedString str2("TEST1");
	IndexedString str3("TEST2");
	
	REQUIRE(str1.get() == str2.get());
	CHECK(str1 == str2);

	REQUIRE(str1.get() != str3.get());
	CHECK(str1 != str3);

	REQUIRE(str2.get() != str3.get());
	CHECK(str2 != str3);
}