#include <catch.hpp>

#include <Defines.hpp>
#include <SafePtrRoot.hpp>
#include <SafePtr.hpp>

using namespace Poly;

namespace Poly
{
	class Test : public SafePtrRoot
	{
	public:
		Test(int a)
		{
			Debug = a;
		}

		int Debug;
	};
}

TEST_CASE("Registering and comparing pointers", "[SafePtr]") {
	Test obj1 = Test(1);
	Test obj2 = Test(2);

	SafePtr<Test> p1 = SafePtr<Test>(&obj1);
	SafePtr<Test> p2 = &obj2;

	REQUIRE(p1 == obj1);
	REQUIRE(p2 == obj2);

	REQUIRE(p1 != p2);
	REQUIRE(p1 != &obj2);

	REQUIRE(obj1.Debug == p1->Debug);
}

TEST_CASE("Double pointer registration", "[SafePtr]") {
	Test obj = Test(1);

	SafePtr<Test> p1 = SafePtr<Test>(&obj);
	SafePtr<Test> p2 = SafePtr<Test>(&obj);

	REQUIRE(p1 == p2);
}

TEST_CASE("Object deletation", "[SafePtr]") {
	Test *obj = new Test(1);
	SafePtr<Test> p = SafePtr<Test>(obj);

	delete obj;

	REQUIRE(p == nullptr);
}

TEST_CASE("* and -> operators", "[SafePtr]") {
	Test obj = Test(1);
	SafePtr<Test> p = SafePtr<Test>(&obj);

	REQUIRE(p->Debug == obj.Debug);
	REQUIRE(*p == &obj);
}

TEST_CASE("Initializing SafePtr with nullptr", "[SafePtr]") {
	Test *ptr = nullptr;
	SafePtr<Test> p = SafePtr<Test>(ptr);

	REQUIRE(p == nullptr);
}


