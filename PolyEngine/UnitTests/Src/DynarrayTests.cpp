#include <catch.hpp>

#include <Dynarray.hpp>
//TODO implement

using namespace Poly;

TEST_CASE("Dynarray constructors", "[Dynarray]")
{
	// default constructor
	Dynarray<int> a;
	REQUIRE(a.GetSize() == 0);
	
	// initializer list constructor
	Dynarray<int> b{ 1, 2, 3 };
	REQUIRE(b.GetSize() == 3);
	REQUIRE(b[0] == 1);
	REQUIRE(b[1] == 2);
	REQUIRE(b[2] == 3);

	// reserve constructor
	Dynarray<int> c(5);
	REQUIRE(c.GetSize() == 0);
	REQUIRE(c.GetCapacity() >= 5);

	// copy constructor
	Dynarray<int> d(b);
	REQUIRE(b.GetSize() == 3);
	REQUIRE(d.GetSize() == 3);
	REQUIRE(d[0] == 1);
	REQUIRE(d[1] == 2);
	REQUIRE(d[2] == 3);

	// move constructor
	Dynarray<int> e(std::move(b));
	REQUIRE(b.GetSize() == 0);
	REQUIRE(e.GetSize() == 3);
	REQUIRE(e[0] == 1);
	REQUIRE(e[1] == 2);
	REQUIRE(e[2] == 3);
}

TEST_CASE("Dynarray assign operator", "[Dynarray]")
{
	Dynarray<int> a, b, c;

	// initializer list assign operator
	a = { 1, 2, 3 };
	REQUIRE(a.GetSize() == 3);
	REQUIRE(a[0] == 1);
	REQUIRE(a[1] == 2);
	REQUIRE(a[2] == 3);

	// copy assign operator
	b = a;
	REQUIRE(a.GetSize() == 3);
	REQUIRE(b.GetSize() == 3);
	REQUIRE(b[0] == 1);
	REQUIRE(b[1] == 2);
	REQUIRE(b[2] == 3);

	// move assign operator
	c = std::move(a);
	REQUIRE(a.GetSize() == 0);
	REQUIRE(c.GetSize() == 3);
	REQUIRE(c[0] == 1);
	REQUIRE(c[1] == 2);
	REQUIRE(c[2] == 3);
}

TEST_CASE("Dynarray comparison operators", "[Dynarray]")
{
	Dynarray<int> a{ 1, 2, 3 };
	Dynarray<int> b{ 1, 2, 3 };
	Dynarray<int> c{ 1, 2, 3, 4 };
	Dynarray<int> d{ 1, 2, 4 };
	
	REQUIRE(a == b);
	REQUIRE_FALSE(a == c);
	REQUIRE_FALSE(a == d);
	REQUIRE_FALSE(c == d);

	REQUIRE_FALSE(a != b);
	REQUIRE(a != c);
	REQUIRE(a != d);
	REQUIRE(c != d);
}

TEST_CASE("Dynarray iterators", "[Dynarray]")
{
	Dynarray<int> a{ 1, 2, 3 };
	const Dynarray<int> b{ 1, 2, 3 };
	Dynarray<int> c;
	Dynarray<int> d{ 1 };
	
	SECTION("Begin and end") {
		REQUIRE(c.Begin() == c.Begin());
		REQUIRE(c.Begin() == c.End());
		REQUIRE(c.End() == c.End());
	}

	SECTION("iterator increments and decrements") {
		auto it = d.Begin();
		REQUIRE(it == d.Begin());
		REQUIRE(it++ == d.Begin());
		REQUIRE(it == d.End());
		REQUIRE(it-- == d.End());
		REQUIRE(it == d.Begin());
		REQUIRE(++it == d.End());
		REQUIRE(--it == d.Begin());
	}

	SECTION("iterator sum and subtraction") {
		auto it = d.Begin();
		REQUIRE((it + 1) == d.End());
		REQUIRE((it += 1) == d.End());
		REQUIRE((it - 1) == d.Begin());
		REQUIRE((it -= 1) == d.Begin());
	}

	SECTION("Mutable for each loop") {
		int i = 0;
		for (int x : a)
			REQUIRE(x == ++i);
	}
	
	SECTION("Const for each loop") {
		int i = 0;
		for (int x : b)
			REQUIRE(x == ++i);
	}
}

TEST_CASE("Dynarray basics", "[Dynarray]")
{
	// degree constructor
	Dynarray<int> a;
	a.PushBack(1);
	a.PushBack(1);
	a.PushBack(2);
	REQUIRE(a[0] == a[1]);
	REQUIRE(a[0] != a[2]);
	a.PushFront(2);
	REQUIRE(a[0] == a[3]);
	REQUIRE(a[1] == a[2]);
	REQUIRE(a[0] != a[1]);
	a.PopFront();
	REQUIRE(a[0] == a[1]);
	REQUIRE(a[0] != a[2]);
	a.PopBack();
	REQUIRE(a[0] == a[1]);
}

TEST_CASE("Dynarray with BaseObject", "[Dynarray]")
{

	class Test : BaseObject<>
	{
	public:
		Test() {}
		~Test() {}

	private:
		int i = 0;

	};

	Dynarray<Test> q;

	REQUIRE(q.IsEmpty() == true);

	q.PushBack(Test());
	q.PopFront();

}

TEST_CASE("Dynarray iterators dereferencing operators tests", "[Dynarray]")
{
	struct Test { bool GetVal() const { return true; } };
	
	// degree constructor
	Dynarray<Test> a;
	a.PushBack(Test());
	REQUIRE(a.Begin()->GetVal() == true);
	REQUIRE((*a.Begin()).GetVal() == true);

	const Dynarray<Test>& b = a;
	REQUIRE(b.Begin()->GetVal() == true);
	REQUIRE((*b.Begin()).GetVal() == true);
}