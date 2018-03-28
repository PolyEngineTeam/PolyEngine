#include <Defines.hpp>
#include <catch.hpp>
#include <Collections/PriorityQueue.hpp>

using namespace Poly;

TEST_CASE("PriorityQueue sorted push test", "[PriorityQueue]")
{
	PriorityQueue<int> q;
	const int testSize = 10;

	for (int i = 0; i < testSize; ++i)
	{
		q.Push(i);
		CHECK(q.GetSize() == i+1);
	}
		
	for (int i = 0; i < testSize; ++i)
	{
		CHECK(q.Head() == i);
		CHECK(q.Pop() == i);
	}
}

TEST_CASE("PriorityQueue reverrse sorted push test", "[PriorityQueue]")
{
	PriorityQueue<int> q;
	const int testSize = 100;

	for (int i = 0; i < testSize; ++i)
	{
		q.Push(testSize - i - 1);
		CHECK(q.GetSize() == i + 1);
	}

	for (int i = 0; i < testSize; ++i)
	{
		CHECK(q.Head() == i);
		CHECK(q.Pop() == i);
	}
}

TEST_CASE("PriorityQueue random push test", "[PriorityQueue]")
{
	PriorityQueue<int> q;
	const size_t testSize = 100;

	for (size_t i = 0; i < testSize; ++i)
	{
		q.Push(rand());
		CHECK(q.GetSize() == i + 1);
	}

	for (size_t i = 0; i < testSize - 1; ++i)
	{
		size_t v1 = q.Pop();
		CHECK(v1 <= q.Head());
	}
}

struct CustomTestComparator
{
	bool operator()(int a, int b) const { return a > b; }
};

TEST_CASE("PriorityQueue custom comparator test", "[PriorityQueue]")
{
	PriorityQueue<int, CustomTestComparator> q;
	const size_t testSize = 100;

	for (size_t i = 0; i < testSize; ++i)
	{
		q.Push(rand());
		CHECK(q.GetSize() == i + 1);
	}

	for (size_t i = 0; i < testSize - 1; ++i)
	{
		size_t v1 = q.Pop();
		CHECK(v1 >= q.Head());
	}
}

TEST_CASE("PriorityQueue heap sort", "[PriorityQueue]")
{
	const size_t testSize = 100;

	Dynarray<int> table(testSize);
	for (size_t i = 0; i < testSize; ++i)
		table.PushBack(rand());

	PriorityQueue<int> q(std::move(table));

	for (size_t i = 0; i < testSize - 1; ++i)
	{
		size_t v1 = q.Pop();
		CHECK(v1 <= q.Head());
	}
}