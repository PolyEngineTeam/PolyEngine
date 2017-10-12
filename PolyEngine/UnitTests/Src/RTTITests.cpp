#include <catch.hpp>

#include <RTTI.hpp>

using namespace Poly;

enum class eTestEnum
{
	VAL_1,
	VAL_2,
	_COUNT
};

class TestClass : public RTTIBase {
	RTTI_DECLARE_TYPE_DERIVED(TestClass, RTTIBase)
	{
		RTTI_PROPERTY(bool, val1, "Val1", RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_ENUM(eTestEnum, val2, "Val2", RTTI::ePropertyFlag::NONE);
	}
public:
	bool val1 = true;
	eTestEnum val2 = eTestEnum::VAL_1;
};

RTTI_DEFINE_TYPE(TestClass)

class TestClass2 : public RTTIBase {
	RTTI_DECLARE_TYPE_DERIVED(TestClass2, RTTIBase) {}
public:
};
RTTI_DEFINE_TYPE(TestClass2)

TEST_CASE("RTTI basics", "[RTTI]") {
	TestClass* a = new TestClass();
	RTTIBase* b = a;
	REQUIRE(rtti_cast<TestClass2*>(b) == nullptr);
	REQUIRE(rtti_cast<TestClass*>(b) == a);
	REQUIRE(IsOfType<TestClass>(b) == true);
	REQUIRE(IsOfType<TestClass2>(b) == false);
	REQUIRE(rtti_cast<TestClass*>(a) == a);
	REQUIRE(IsOfType<TestClass>(a) == true);
	REQUIRE(IsOfType<TestClass2>(a) == false);
	delete a;

	TestClass2* a2 = new TestClass2();
	RTTIBase* b2 = a2;
	REQUIRE(rtti_cast<TestClass*>(b2) == nullptr);
	REQUIRE(rtti_cast<TestClass2*>(b2) == a2);
	REQUIRE(IsOfType<TestClass>(b2) == false);
	REQUIRE(IsOfType<TestClass2>(b2) == true);
	REQUIRE(rtti_cast<TestClass2*>(a2) == a2);
	REQUIRE(IsOfType<TestClass>(a2) == false);
	REQUIRE(IsOfType<TestClass2>(a2) == true);
	delete a2;
}
