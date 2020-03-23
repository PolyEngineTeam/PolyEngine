#include <pe/Defines.hpp>
#include <catch.hpp>
#include <pe/core/storage/PriorityQueue.hpp>

TEST_CASE("PriorityQueue sorted push test", "[PriorityQueue]")
{
	::pe::core::storage::PriorityQueue<int> q;
	const int testSize = 10;

	for (int i = 0; i < testSize; ++i)
	{
		q.push(i);
		CHECK(q.size() == i+1);
	}
		
	for (int i = 0; i < testSize; ++i)
	{
		CHECK(q.top() == i);
		q.pop();
	}
}

TEST_CASE("PriorityQueue reverse sorted push test", "[PriorityQueue]")
{
	::pe::core::storage::PriorityQueue<int> q;
	const int testSize = 100;

	for (int i = 0; i < testSize; ++i)
	{
		q.push(testSize - i - 1);
		CHECK(q.size() == i + 1);
	}

	for (int i = 0; i < testSize; ++i)
	{
		CHECK(q.top() == i);
		q.pop();
	}
}

TEST_CASE("PriorityQueue random push test", "[PriorityQueue]")
{
	::pe::core::storage::PriorityQueue<int> q;
	const size_t testSize = 100;

	for (size_t i = 0; i < testSize; ++i)
	{
		q.push(rand());
		CHECK(q.size() == i + 1);
	}

	for (size_t i = 0; i < testSize - 1; ++i)
	{
		size_t v1 = q.top();
		q.pop();
		CHECK(v1 <= q.top());
	}
}

struct CustomTestComparator
{
	bool operator()(int a, int b) const { return a < b; }
};

TEST_CASE("PriorityQueue custom comparator test", "[PriorityQueue]")
{
	::pe::core::storage::PriorityQueue<int, CustomTestComparator> q;
	const size_t testSize = 100;

	for (size_t i = 0; i < testSize; ++i)
	{
		q.push(rand());
		CHECK(q.size() == i + 1);
	}

	for (size_t i = 0; i < testSize - 1; ++i)
	{
		size_t v1 = q.top();
		q.pop();
		CHECK(v1 >= q.top());
	}
}

TEST_CASE("PriorityQueue heap sort", "[PriorityQueue]")
{
	const size_t testSize = 100;

	std::vector<int> table(testSize);
	for (size_t i = 0; i < testSize; ++i)
		table.push_back(rand());

	::pe::core::storage::PriorityQueue<int> q(std::move(table));

	for (size_t i = 0; i < testSize - 1; ++i)
	{
		size_t v1 = q.top();
		q.pop();
		CHECK(v1 <= q.top());
	}
}