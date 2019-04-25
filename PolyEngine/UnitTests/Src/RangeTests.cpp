#include <catch.hpp>

#include <Utils/Range.hpp>
#include <vector>
#include <map>

using namespace Poly;

TEST_CASE("Iterator pair constructors", "Range") {
   std::vector<int> testVec = {1, 2, 3, 4};
   std::map<char, int> testMap = { {'a', 1}, {'b', 2}, {'c', 3} };
   const std::vector<int>& constTestVec = testVec;
   const std::map<char, int>& constTestMap = testMap;

	SECTION("Mutable vector iterator pair constructor") {
		auto range = Range(testVec.begin(), testVec.end());
      
      int counter = 0;
      for(auto val : range)
      {
         ++counter;
         CHECK(val == counter);
      }
      CHECK(counter == 4);
	}

   SECTION("Const vector iterator pair constructor") {
		auto range = Range(constTestVec.begin(), constTestVec.end());
      
      int counter = 0;
      for(auto val : range)
      {
         ++counter;
         CHECK(val == counter);
      }
      CHECK(counter == 4);
	}

   SECTION("Mutable map iterator pair constructor") {
		auto range = Range(testMap.begin(), testMap.end());
      
      int counter = 0;
      char keyCounter = 'a';
      for(auto val : range)
      {
         ++counter;
         CHECK(val.second == counter);
         CHECK(val.first == keyCounter);
         ++keyCounter;
      }
      CHECK(counter == 3);
	}

   SECTION("Const map iterator pair constructor") {
		auto range = Range(constTestMap.begin(), constTestMap.end());
      
      int counter = 0;
      char keyCounter = 'a';
      for(auto val : range)
      {
         ++counter;
         CHECK(val.second == counter);
         CHECK(val.first == keyCounter);
         ++keyCounter;
      }
      CHECK(counter == 3);
	}
}

TEST_CASE("Collection constructors", "Range") {
   std::vector<int> testVec = {1, 2, 3, 4};
   std::map<char, int> testMap = { {'a', 1}, {'b', 2}, {'c', 3} };
   const std::vector<int>& constTestVec = testVec;
   const std::map<char, int>& constTestMap = testMap;

	SECTION("Mutable vector collection constructor") {
		auto range = Range<std::vector<int>::iterator>(testVec);
      
      int counter = 0;
      for(auto val : range)
      {
         ++counter;
         CHECK(val == counter);
      }
      CHECK(counter == 4);
	}

   SECTION("Const vector collection constructor") {
		auto range = Range<std::vector<int>::const_iterator>(constTestVec);
      
      int counter = 0;
      for(auto val : range)
      {
         ++counter;
         CHECK(val == counter);
      }
      CHECK(counter == 4);
	}

   SECTION("Mutable map collection constructor") {
		auto range = Range<std::map<char, int>::iterator>(testMap);
      
      int counter = 0;
      char keyCounter = 'a';
      for(auto val : range)
      {
         ++counter;
         CHECK(val.second == counter);
         CHECK(val.first == keyCounter);
         ++keyCounter;
      }
      CHECK(counter == 3);
	}

   SECTION("Const map collection constructor") {
		auto range = Range<std::map<char, int>::const_iterator>(constTestMap);
      
      int counter = 0;
      char keyCounter = 'a';
      for(auto val : range)
      {
         ++counter;
         CHECK(val.second == counter);
         CHECK(val.first == keyCounter);
         ++keyCounter;
      }
      CHECK(counter == 3);
	}
}

TEST_CASE("MakeRange", "Range") {
   std::vector<int> testVec = {1, 2, 3, 4};
   std::map<char, int> testMap = { {'a', 1}, {'b', 2}, {'c', 3} };
   const std::vector<int>& constTestVec = testVec;
   const std::map<char, int>& constTestMap = testMap;

	SECTION("Mutable vector MakeRange") {
		auto range = MakeRange(testVec);
      
      int counter = 0;
      for(auto val : range)
      {
         ++counter;
         CHECK(val == counter);
      }
      CHECK(counter == 4);
	}

   SECTION("Const vector MakeRange") {
		auto range = MakeRange(constTestVec);
      
      int counter = 0;
      for(auto val : range)
      {
         ++counter;
         CHECK(val == counter);
      }
      CHECK(counter == 4);
	}

   SECTION("Mutable map MakeRange") {
		auto range = MakeRange(testMap);
      
      int counter = 0;
      char keyCounter = 'a';
      for(auto val : range)
      {
         ++counter;
         CHECK(val.second == counter);
         CHECK(val.first == keyCounter);
         ++keyCounter;
      }
      CHECK(counter == 3);
	}

   SECTION("Const map MakeRange") {
		auto range = MakeRange(constTestMap);
      
      int counter = 0;
      char keyCounter = 'a';
      for(auto val : range)
      {
         ++counter;
         CHECK(val.second == counter);
         CHECK(val.first == keyCounter);
         ++keyCounter;
      }
      CHECK(counter == 3);
	}
}
