#include <catch.hpp>

#include <Queue.hpp>

using namespace Poly;

TEST_CASE("Queue tests", "[Queue]")
{
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

	REQUIRE(q.Back() == 5);
	REQUIRE(q.Front() == 1);

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
		int i = 0;

	};

	Queue<Test> q;
	Queue<Test> r(q);

	REQUIRE(q.Empty() == true);
	REQUIRE(r.Empty() == true);

	q.Push(Test());
	q.Pop();

}