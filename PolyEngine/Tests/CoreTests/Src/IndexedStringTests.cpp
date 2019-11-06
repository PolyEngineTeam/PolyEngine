#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/storage/String.hpp>
#include <pe/core/storage/IndexedString.hpp>
#include <pe/core/storage/impl/IndexedStringManager.hpp>

using pe::core::storage::String;
using pe::core::storage::IndexedString;
using pe::core::storage::impl::IndexedStringManager;

constexpr char TEST_STR1[] = "TEST1";
constexpr char TEST_STR2[] = "TEST2";

TEST_CASE("Construction", "[IndexedString]")
{
	IndexedStringManager::get().setTTLMode(false);
	IndexedString str(TEST_STR1);
	IndexedString str2(TEST_STR1);
	REQUIRE(str.get() == String(TEST_STR1));
	CHECK(str == str2);
}

TEST_CASE("Copy construction", "[IndexedString]")
{
	IndexedStringManager::get().setTTLMode(false);
	IndexedString str(TEST_STR1);
	IndexedString str2(str);
	REQUIRE(str.get() == str2.get());
	CHECK(str == str2);
}

TEST_CASE("Assignment", "[IndexedString]")
{
	IndexedStringManager::get().setTTLMode(false);
	IndexedString str(TEST_STR1);
	IndexedString str2(TEST_STR2);
	REQUIRE(str.get() != str2.get());
	REQUIRE(str != str2);

	str2 = str;
	REQUIRE(str.get() == str2.get());
	CHECK(str == str2);
}

TEST_CASE("Comparison", "[IndexedString]")
{
	IndexedStringManager::get().setTTLMode(false);
	IndexedString str1(TEST_STR1);
	IndexedString str2(TEST_STR1);
	IndexedString str3(TEST_STR2);
	
	REQUIRE(str1.get() == str2.get());
	CHECK(str1 == str2);

	REQUIRE(str1.get() != str3.get());
	CHECK(str1 != str3);

	REQUIRE(str2.get() != str3.get());
	CHECK(str2 != str3);
}

TEST_CASE("isRegistered", "[IndexedString]")
{
	IndexedStringManager::get().setTTLMode(false);
	REQUIRE(!IndexedStringManager::get().isRegistered(TEST_STR1));
	IndexedString str1(TEST_STR1);
	REQUIRE(IndexedStringManager::get().isRegistered(TEST_STR1));
	
	REQUIRE(!IndexedStringManager::get().isRegistered(TEST_STR2));
	IndexedString str2(TEST_STR2);
	REQUIRE(IndexedStringManager::get().isRegistered(TEST_STR2));
}

TEST_CASE("TTL", "[IndexedString]")
{
	IndexedStringManager::get().setTTLMode(true);

	{
		IndexedString str1(TEST_STR1);
		IndexedStringManager::get().tickTTL(1);
		{
			IndexedString str2(TEST_STR1);
			IndexedStringManager::get().tickTTL(1);
			REQUIRE(IndexedStringManager::get().isRegistered(TEST_STR1));
		}
		REQUIRE(IndexedStringManager::get().isRegistered(TEST_STR1));
	}
	REQUIRE(IndexedStringManager::get().isRegistered(TEST_STR1));
	
	// Values get removed after 100 ticks, they should be still present after 99
	IndexedStringManager::get().tickTTL(99);
	REQUIRE(IndexedStringManager::get().isRegistered(TEST_STR1));

	IndexedStringManager::get().tickTTL(1);
	CHECK(!IndexedStringManager::get().isRegistered(TEST_STR1));
}