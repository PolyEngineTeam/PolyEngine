#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/storage/Queue.hpp>

TEST_CASE("Queue tests", "[Queue]")
{
	::pe::core::storage::Queue<int> q;
	::pe::core::storage::Queue<int> r(q);

	REQUIRE(q.IsEmpty() == true);
	REQUIRE(r.IsEmpty() == true);

	q.PushBack(5);
	REQUIRE(q.IsEmpty() == false);
	::pe::core::storage::Queue<int> s(q);

	REQUIRE(r.IsEmpty() == true);
	REQUIRE(q.IsEmpty() == false);
	REQUIRE(s.IsEmpty() == false);
	REQUIRE(q.Back() == 5);
	REQUIRE(s.Back() == 5);
	REQUIRE(q.Front() == 5);
	REQUIRE(s.Front() == 5);

	q.PushBack(8);
	::pe::core::storage::Queue<int> t(q);

	REQUIRE(t.IsEmpty() == false);
	REQUIRE(q.Back() == 8);
	REQUIRE(t.Back() == 8);
	REQUIRE(s.Back() == 5);
	REQUIRE(q.Front() == 5);
	REQUIRE(t.Front() == 5);
	REQUIRE(s.Front() == 5);

	q.PopBack();
	q.PopBack();
	REQUIRE(q.IsEmpty() == true);

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

	class Test : ::pe::core::BaseObject<>
	{
	public:
		Test() {}
		~Test() {}
	};

	::pe::core::storage::Queue<Test> q;
	::pe::core::storage::Queue<Test> r(q);

	REQUIRE(q.IsEmpty() == true);
	REQUIRE(r.IsEmpty() == true);

	Test a = Test();
	q.PushBack(a);
	q.PushBack(a);
	q.PushBack(a);
	q.PushBack(Test());
	q.PopBack();

}