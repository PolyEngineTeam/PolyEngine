#include <catch.hpp>

#include <pe/core/utils/Range.hpp>
#include <vector>
#include <map>

template<typename T>
void ValidateVectorRangeHelper(const ::pe::core::utils::Range<T>& range, int expectedValueAtEnd)
{
	int counter = 0;
	for (int val : range)
	{
		++counter;
		CHECK(val == counter);
	}
	CHECK(counter == expectedValueAtEnd);
}

template<typename T>
void ValidateVectorRangeReversedHelper(const ::pe::core::utils::Range<T>& range, int startValue)
{
	int counter = startValue;
	for (int val : range)
	{
		CHECK(val == counter);
		--counter;
	}
	CHECK(counter == 0);
}

template<typename T>
void ValidateMapRangeHelper(const ::pe::core::utils::Range<T>& range, int expectedValueAtEnd)
{
	int counter = 0;
	char keyCounter = 'a';
	for (auto val : range)
	{
		++counter;
		CHECK(val.second == counter);
		CHECK(val.first == keyCounter);
		++keyCounter;
	}
	CHECK(counter == expectedValueAtEnd);
}

template<typename T>
void ValidateMapRangeReversedHelper(const ::pe::core::utils::Range<T>& range, int startVal, char startKey)
{
	int counter = startVal;
	char keyCounter = startKey;
	for (auto val : range)
	{
		CHECK(val.second == counter);
		CHECK(val.first == keyCounter);
		--keyCounter;
		--counter;
	}
	CHECK(counter == 0);
}

TEST_CASE("Iterator pair constructors", "Range")
{
    std::vector<int> testVec = {1, 2, 3, 4};
    std::map<char, int> testMap = { {'a', 1}, {'b', 2}, {'c', 3} };
    const std::vector<int>& constTestVec = testVec;
    const std::map<char, int>& constTestMap = testMap;

    SECTION("Mutable vector iterator pair constructor")
    {
	    auto range = ::pe::core::utils::Range(testVec.begin(), testVec.end());
		ValidateVectorRangeHelper(range, 4);
	}

    SECTION("Const vector iterator pair constructor")
    {
	    auto range = ::pe::core::utils::Range(constTestVec.begin(), constTestVec.end());
		ValidateVectorRangeHelper(range, 4);
	}

    SECTION("Mutable map iterator pair constructor")
    {
		auto range = ::pe::core::utils::Range(testMap.begin(), testMap.end());
		ValidateMapRangeHelper(range, 3);
	}

    SECTION("Const map iterator pair constructor")
    {
		auto range = ::pe::core::utils::Range(constTestMap.begin(), constTestMap.end());
		ValidateMapRangeHelper(range, 3);
	}
}

TEST_CASE("Range comparison operators", "Range")
{
	std::vector<int> testVec = { 1, 2, 3, 4 };

	auto range1 = ::pe::core::utils::Range(testVec.begin(), testVec.end());
	auto range2 = ::pe::core::utils::Range(testVec.begin() + 1, testVec.end());
	
	CHECK(range1 == range1);
	CHECK(range2 == range2);
	
	CHECK_FALSE(range1 != range1);
	CHECK_FALSE(range2 != range2);
	
	CHECK(range1 != range2);
}

TEST_CASE("MakeRange", "Range")
{
    std::vector<int> testVec = {1, 2, 3, 4};
    std::map<char, int> testMap = { {'a', 1}, {'b', 2}, {'c', 3} };
    const std::vector<int>& constTestVec = testVec;
    const std::map<char, int>& constTestMap = testMap;

	SECTION("Mutable vector MakeRange")
    {
		auto range = ::pe::core::utils::MakeRange(testVec);
		ValidateVectorRangeHelper(range, 4);
    }

    SECTION("Const vector MakeRange")
    {
		auto range = ::pe::core::utils::MakeRange(constTestVec);
		ValidateVectorRangeHelper(range, 4);
	}

	SECTION("Const vector ::pe::core::utils::MakeRangeConst")
	{
		auto range = ::pe::core::utils::MakeRangeConst(constTestVec);
		ValidateVectorRangeHelper(range, 4);
	}

	SECTION("Mutable vector MakeRangeReversed")
	{
		auto range = ::pe::core::utils::MakeRangeReversed(testVec);
		ValidateVectorRangeReversedHelper(range, 4);
	}

	SECTION("Const vector MakeRangeReversed")
	{
		auto range = ::pe::core::utils::MakeRangeReversed(constTestVec);
		ValidateVectorRangeReversedHelper(range, 4);
	}

	SECTION("Const vector MakeRangeReversedConst")
	{
		auto range = ::pe::core::utils::MakeRangeReversedConst(constTestVec);
		ValidateVectorRangeReversedHelper(range, 4);
	}

    SECTION("Mutable map MakeRange")
    {
		auto range = ::pe::core::utils::MakeRange(testMap);
		ValidateMapRangeHelper(range, 3);
	}

    SECTION("Const map MakeRange")
    {
		auto range = ::pe::core::utils::MakeRange(constTestMap);
		ValidateMapRangeHelper(range, 3);
	}

	SECTION("Const map MakeRangeConst")
	{
		auto range = ::pe::core::utils::MakeRangeConst(constTestMap);
		ValidateMapRangeHelper(range, 3);
	}

	SECTION("Mutable map MakeRangeReversed")
	{
		auto range = ::pe::core::utils::MakeRangeReversed(testMap);
		ValidateMapRangeReversedHelper(range, 3, 'c');
	}

	SECTION("Const map MakeRangeReversed")
	{
		auto range = ::pe::core::utils::MakeRangeReversed(constTestMap);
		ValidateMapRangeReversedHelper(range, 3, 'c');
	}

	SECTION("Const map MakeRangeReversedConst")
	{
		auto range = ::pe::core::utils::MakeRangeReversedConst(constTestMap);
		ValidateMapRangeReversedHelper(range, 3, 'c');
	}
}
