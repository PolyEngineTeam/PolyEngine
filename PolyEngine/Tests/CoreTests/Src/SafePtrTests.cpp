#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/Defines.hpp>
#include <pe/core/memory/SafePtrRoot.hpp>
#include <pe/core/memory/SafePtr.hpp>



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

	::pe::core::memory::SafePtr<Test> p1 = ::pe::core::memory::SafePtr<Test>(&obj1);
	::pe::core::memory::SafePtr<Test> p2 = ::pe::core::memory::SafePtr<Test>(&obj2);

	REQUIRE(p1 == &obj1);
	REQUIRE(p2 == &obj2);

	REQUIRE(p1 != p2);
	REQUIRE(p1 != &obj2);

	REQUIRE(obj1.Debug == p1->Debug);
}

TEST_CASE("Double pointer registration", "[SafePtr]") {
	Test obj = Test(1);

	::pe::core::memory::SafePtr<Test> p1 = ::pe::core::memory::SafePtr<Test>(&obj);
	::pe::core::memory::SafePtr<Test> p2 = ::pe::core::memory::SafePtr<Test>(&obj);

	REQUIRE(p1 == p2);
}

TEST_CASE("Object deletation", "[SafePtr]") {
	Test *obj = new Test(1);
	::pe::core::memory::SafePtr<Test> p = ::pe::core::memory::SafePtr<Test>(obj);

	delete obj;

	REQUIRE(p == nullptr);
}

TEST_CASE("* and -> operators", "[SafePtr]") {
	Test obj = Test(1);
	::pe::core::memory::SafePtr<Test> p = ::pe::core::memory::SafePtr<Test>(&obj);

	REQUIRE(p->Debug == obj.Debug);
	REQUIRE(*p == &obj);
}

TEST_CASE("Initializing ::pe::core::memory::SafePtr with nullptr", "[SafePtr]") {
	Test *ptr = nullptr;
	::pe::core::memory::SafePtr<Test> p = ::pe::core::memory::SafePtr<Test>(ptr);

	REQUIRE(p == nullptr);
}


