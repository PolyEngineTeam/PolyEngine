#include <catch.hpp>

#include <pe/core/utils/Range.hpp>
#include <vector>
#include <map>

using namespace Poly;

template<typename T>
void ValidateVectorRangeHelper(const Range<T>& range, int expectedValueAtEnd)
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
void ValidateVectorRangeReversedHelper(const Range<T>& range, int startValue)
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
void ValidateMapRangeHelper(const Range<T>& range, int expectedValueAtEnd)
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
void ValidateMapRangeReversedHelper(const Range<T>& range, int startVal, char startKey)
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
	    auto range = Range(testVec.begin(), testVec.end());
		ValidateVectorRangeHelper(range, 4);
	}

    SECTION("Const vector iterator pair constructor")
    {
	    auto range = Range(constTestVec.begin(), constTestVec.end());
		ValidateVectorRangeHelper(range, 4);
	}

    SECTION("Mutable map iterator pair constructor")
    {
		auto range = Range(testMap.begin(), testMap.end());
		ValidateMapRangeHelper(range, 3);
	}

    SECTION("Const map iterator pair constructor")
    {
		auto range = Range(constTestMap.begin(), constTestMap.end());
		ValidateMapRangeHelper(range, 3);
	}
}

TEST_CASE("Range comparison operators", "Range")
{
	std::vector<int> testVec = { 1, 2, 3, 4 };

	auto range1 = Range(testVec.begin(), testVec.end());
	auto range2 = Range(testVec.begin() + 1, testVec.end());
	
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
		auto range = MakeRange(testVec);
		ValidateVectorRangeHelper(range, 4);
    }

    SECTION("Const vector MakeRange")
    {
		auto range = MakeRange(constTestVec);
		ValidateVectorRangeHelper(range, 4);
	}

	SECTION("Const vector MakeRangeConst")
	{
		auto range = MakeRangeConst(constTestVec);
		ValidateVectorRangeHelper(range, 4);
	}

	SECTION("Mutable vector MakeRangeReversed")
	{
		auto range = MakeRangeReversed(testVec);
		ValidateVectorRangeReversedHelper(range, 4);
	}

	SECTION("Const vector MakeRangeReversed")
	{
		auto range = MakeRangeReversed(constTestVec);
		ValidateVectorRangeReversedHelper(range, 4);
	}

	SECTION("Const vector MakeRangeReversedConst")
	{
		auto range = MakeRangeReversedConst(constTestVec);
		ValidateVectorRangeReversedHelper(range, 4);
	}

    SECTION("Mutable map MakeRange")
    {
		auto range = MakeRange(testMap);
		ValidateMapRangeHelper(range, 3);
	}

    SECTION("Const map MakeRange")
    {
		auto range = MakeRange(constTestMap);
		ValidateMapRangeHelper(range, 3);
	}

	SECTION("Const map MakeRangeConst")
	{
		auto range = MakeRangeConst(constTestMap);
		ValidateMapRangeHelper(range, 3);
	}

	SECTION("Mutable map MakeRangeReversed")
	{
		auto range = MakeRangeReversed(testMap);
		ValidateMapRangeReversedHelper(range, 3, 'c');
	}

	SECTION("Const map MakeRangeReversed")
	{
		auto range = MakeRangeReversed(constTestMap);
		ValidateMapRangeReversedHelper(range, 3, 'c');
	}

	SECTION("Const map MakeRangeReversedConst")
	{
		auto range = MakeRangeReversedConst(constTestMap);
		ValidateMapRangeReversedHelper(range, 3, 'c');
	}
}
