#include <catch.hpp>

#include <ctime>

#include <PriorityQueue.hpp>

using namespace Poly;

TEST_CASE("PriorityQueue basic tests", "[PriorityQueue]")
{
	PriorityQueue<int> q;

	srand(time(nullptr));

	int allocSize = 100;
	for (int i = 0; i < allocSize; ++i)
	{
		q.Push(rand() % 10000);
		CHECK(q.GetSize() == i+1);
	}

	int max = 0;
	while (q.GetSize() > 0)
	{
		int val = q.Pop();
		--allocSize;
		CHECK(q.GetSize() == allocSize);
		CHECK(max <= val);
		max = std::max(val, max);
	}
}

TEST_CASE("PriorityQueue with custom comparator tests", "[PriorityQueue]")
{
	PriorityQueue<int> q([](int a, int b) { return a > b; });

	srand(time(nullptr));

	int allocSize = 100;
	for (int i = 0; i < allocSize; ++i)
	{
		q.Push(rand() % 10000);
		CHECK(q.GetSize() == i + 1);
	}

	int min = 10000;
	while (q.GetSize() > 0)
	{
		int val = q.Pop();
		--allocSize;
		CHECK(q.GetSize() == allocSize);
		CHECK(min >= val);
		min = std::min(val, min);
	}
}