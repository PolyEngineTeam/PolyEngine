#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/storage/Queue.hpp>

TEST_CASE("Queue tests", "[Queue]")
{
	::pe::core::storage::Queue<int> q;
	::pe::core::storage::Queue<int> r(q);

	REQUIRE(q.isEmpty() == true);
	REQUIRE(r.isEmpty() == true);

	q.pushBack(5);
	REQUIRE(q.isEmpty() == false);
	::pe::core::storage::Queue<int> s(q);

	REQUIRE(r.isEmpty() == true);
	REQUIRE(q.isEmpty() == false);
	REQUIRE(s.isEmpty() == false);
	REQUIRE(q.back() == 5);
	REQUIRE(s.back() == 5);
	REQUIRE(q.front() == 5);
	REQUIRE(s.front() == 5);

	q.pushBack(8);
	::pe::core::storage::Queue<int> t(q);

	REQUIRE(t.isEmpty() == false);
	REQUIRE(q.back() == 8);
	REQUIRE(t.back() == 8);
	REQUIRE(s.back() == 5);
	REQUIRE(q.front() == 5);
	REQUIRE(t.front() == 5);
	REQUIRE(s.front() == 5);

	q.popBack();
	q.popBack();
	REQUIRE(q.isEmpty() == true);

	q.pushBack(1);
	q.pushBack(2);
	q.pushBack(3);
	q.pushBack(4);
	q.pushBack(5);

	REQUIRE(q.back() == 5);
	REQUIRE(q.front() == 1);

	q.popBack();
	q.popFront();

	REQUIRE(q.back() == 4);
	REQUIRE(q.front() == 2);

}

TEST_CASE("Queue tests for emplace", "[Queue]")
{
	::pe::core::storage::Queue<std::tuple<int, int ,int>> q;
	q.emplaceBack(1, 10, 100);
	q.emplaceFront(2, 20, 200);
	q.popFront();
	q.popFront();
	REQUIRE(q.getSize() == 0);
}

TEST_CASE("Queue tests for STL forward iterators", "[Queue]")
{
	::pe::core::storage::Queue<int> q;

	q.pushBack(1);
	q.pushBack(2);
	q.pushBack(3);
	q.pushBack(4);
	q.pushBack(5);

	size_t idx = 0;
	for(auto& e : q)
	{
		++idx;
		REQUIRE(e == idx);
		if(idx == 2)
			e = 20;
	}

	REQUIRE(idx == q.getSize());
	REQUIRE(q.contains(20));

	idx = 0;
	for(auto e : q)
	{
		++idx;
		e = -idx;
		REQUIRE(!q.contains(e));
	}

	auto it = std::find(q.begin(), q.end(), 20);
	REQUIRE(it != q.end());
	*it = 30;
	REQUIRE(q.contains(30));
	*it = 2;
	q.popBack();
	q.popBack();
	q.popFront();
	q.popFront();
	for(auto e : q)
	{
		REQUIRE(e == 3);
	}
	q.pushBack(4);
	q.pushBack(5);
	q.pushFront(2);
	q.pushFront(1);
	q.pushBack(6);
	q.pushBack(7);
	q.pushBack(8);
	idx = 0;
	for(auto& e : q)
	{
		++idx;
		REQUIRE(e == idx);
	}
}

TEST_CASE("Queue tests for STL reverse iterators", "[Queue]")
{
	::pe::core::storage::Queue<int> q;

	q.pushBack(1);
	q.pushBack(2);
	q.pushBack(3);
	q.pushBack(4);
	q.pushBack(5);

	size_t idx = q.getSize();
	
	for(auto it = q.rbegin(); it != q.rend(); ++it)
	{
		REQUIRE(*it == idx);
		--idx;
	}
	REQUIRE(idx == 0);
	q.popFront();
	q.popFront();
	q.popFront();
	q.popBack();
	for(auto it = q.rbegin(); it != q.rend(); ++it)
		REQUIRE(*it == 4);
}

TEST_CASE("Queue tests (with BaseObject)", "[Queue]")
{

	class Test : ::pe::core::BaseObject<>
	{
	public:
		Test() {}
		~Test() {}
	};

	::pe::core::storage::Queue<Test> q;
	::pe::core::storage::Queue<Test> r(q);

	REQUIRE(q.isEmpty() == true);
	REQUIRE(r.isEmpty() == true);

	Test a = Test();
	q.pushBack(a);
	q.pushBack(a);
	q.pushBack(a);
	q.pushBack(Test());
	q.popBack();

}