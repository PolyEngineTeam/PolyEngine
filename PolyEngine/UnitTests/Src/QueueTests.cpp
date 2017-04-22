#include <catch.hpp>

#include <Queue.hpp>

using namespace Poly;

TEST_CASE("Queue tests", "[Queue]")
{
	Queue<int> a(5);

	REQUIRE(a.Empty() == true);

	a.Push(1);
	a.Push(2);
	a.Push(3);
	a.Push(4);
	a.Push(5);

	REQUIRE(a.Front() == 1);
	REQUIRE(a.Back() == 5);

	a.Push(6);
	a.Push(7);
		
	REQUIRE(a.Front() == 1);
	REQUIRE(a.Back() == 7);

	Queue<int> q;
	Queue<int> r(q);

	REQUIRE(q.Empty() == true);
	REQUIRE(r.Empty() == true);

	q.Push(5);
	Queue<int> s(q);

	REQUIRE(r.Empty() == true);
	REQUIRE(q.Empty() == false);
	REQUIRE(s.Empty() == false);
	REQUIRE(q.Back() == 5);
	REQUIRE(s.Back() == 5);
	REQUIRE(q.Front() == 5);
	REQUIRE(s.Front() == 5);

	q.Push(8);
	Queue<int> t(q);

	REQUIRE(t.Empty() == false);
	REQUIRE(q.Back() == 8);
	REQUIRE(t.Back() == 8);
	REQUIRE(s.Back() == 5);
	REQUIRE(q.Front() == 5);
	REQUIRE(t.Front() == 5);
	REQUIRE(s.Front() == 5);

	q.Pop();
	q.Pop();
	REQUIRE(q.Empty() == true);

	q.Push(1);
	q.Push(2);
	q.Push(3);
	q.Push(4);
	q.Push(5);

	REQUIRE(q.Front() == 1);
	REQUIRE(q.Back() == 5);
	

	q.Pop();

	REQUIRE(q.Back() == 5);
	REQUIRE(q.Front() == 2);

}

TEST_CASE("Queue tests (with BaseObject)", "[Queue]")
{

	class Test : BaseObject<>
	{
	public:
		Test() {}
		~Test() {}

	private:
		int i;

	};

	Queue<Test> q;
	Queue<Test> r(q);

	REQUIRE(q.Empty() == true);
	REQUIRE(r.Empty() == true);

	q.Push(Test());
	q.Pop();

	REQUIRE(q.Empty() == true);

}