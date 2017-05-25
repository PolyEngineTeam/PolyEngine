#include <catch.hpp>

#include <Queue.hpp>

using namespace Poly;

TEST_CASE("Queue tests", "[Queue]")
{
	Queue<int> q;
	Queue<int> r(q);

	REQUIRE(q.Empty() == true);
	REQUIRE(r.Empty() == true);

	q.PushBack(5);
	Queue<int> s(q);

	REQUIRE(r.Empty() == true);
	REQUIRE(q.Empty() == false);
	REQUIRE(s.Empty() == false);
	REQUIRE(q.Back() == 5);
	REQUIRE(s.Back() == 5);
	REQUIRE(q.Front() == 5);
	REQUIRE(s.Front() == 5);

	q.PushBack(8);
	Queue<int> t(q);

	REQUIRE(t.Empty() == false);
	REQUIRE(q.Back() == 8);
	REQUIRE(t.Back() == 8);
	REQUIRE(s.Back() == 5);
	REQUIRE(q.Front() == 5);
	REQUIRE(t.Front() == 5);
	REQUIRE(s.Front() == 5);

	q.PopBack();
	q.PopBack();
	REQUIRE(q.Empty() == true);

	q.PushBack(1);
	q.PushBack(2);
	q.PushBack(3);
	q.PushBack(4);
	q.PushBack(5);

	REQUIRE(q.Back() == 5);
	REQUIRE(q.Front() == 1);

	q.PopBack();
	q.PopFront();

	REQUIRE(q.Back() == 4);
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

	Test a = Test();
	q.PushBack(a);
	q.PushBack(a);
	q.PushBack(a);
	q.PushBack(Test());
	q.PopBack();

}