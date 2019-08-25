#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/Defines.hpp>
#include <pe/core/memory/SafePtrRoot.hpp>
#include <pe/core/memory/SafePtr.hpp>

using namespace Poly;

namespace Poly
{
	class Test : public ::pe::core::memory::SafePtrRoot
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

	core::memory::SafePtr<Test> p1 = core::memory::SafePtr<Test>(&obj1);
	core::memory::SafePtr<Test> p2 = core::memory::SafePtr<Test>(&obj2);

	REQUIRE(p1 == &obj1);
	REQUIRE(p2 == &obj2);

	REQUIRE(p1 != p2);
	REQUIRE(p1 != &obj2);

	REQUIRE(obj1.Debug == p1->Debug);
}

TEST_CASE("Double pointer registration", "[SafePtr]") {
	Test obj = Test(1);

	core::memory::SafePtr<Test> p1 = core::memory::SafePtr<Test>(&obj);
	core::memory::SafePtr<Test> p2 = core::memory::SafePtr<Test>(&obj);

	REQUIRE(p1 == p2);
}

TEST_CASE("Object deletation", "[SafePtr]") {
	Test *obj = new Test(1);
	core::memory::SafePtr<Test> p = core::memory::SafePtr<Test>(obj);

	delete obj;

	REQUIRE(p == nullptr);
}

TEST_CASE("* and -> operators", "[SafePtr]") {
	Test obj = Test(1);
	core::memory::SafePtr<Test> p = core::memory::SafePtr<Test>(&obj);

	REQUIRE(p->Debug == obj.Debug);
	REQUIRE(*p == &obj);
}

TEST_CASE("Initializing core::memory::SafePtr with nullptr", "[SafePtr]") {
	Test *ptr = nullptr;
	core::memory::SafePtr<Test> p = core::memory::SafePtr<Test>(ptr);

	REQUIRE(p == nullptr);
}


