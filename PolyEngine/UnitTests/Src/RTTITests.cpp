#include <Defines.hpp>
#include <catch.hpp>

#include <RTTI/RTTI.hpp>
#include <Configs/DebugConfig.hpp>

using namespace Poly;

enum class eRTTITestEnum
{
	VAL_1,
	VAL_2,
	_COUNT
};
REGISTER_ENUM_NAMES(eRTTITestEnum, "val_1", "val_2");

class TestClass : public RTTIBase {
	RTTI_DECLARE_TYPE_DERIVED(TestClass, RTTIBase)
	{
		RTTI_PROPERTY(val1, "Val1", RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY(val2, "Val2", RTTI::ePropertyFlag::NONE);
	}
public:
	bool val1 = true;
	eRTTITestEnum val2 = eRTTITestEnum::VAL_1;
};

RTTI_DEFINE_TYPE(TestClass)

class TestClass2 : public RTTIBase {
	RTTI_DECLARE_TYPE_DERIVED(TestClass2, RTTIBase) { NO_RTTI_PROPERTY(); }
public:
};
RTTI_DEFINE_TYPE(TestClass2)

TEST_CASE("RTTI basics", "[RTTI]") {
	TestClass* a = new TestClass();
	RTTIBase* b = a;
	CHECK(rtti_cast<TestClass2*>(b) == nullptr);
	CHECK(rtti_cast<TestClass*>(b) == a);
	CHECK(IsOfType<TestClass>(b) == true);
	CHECK(IsOfType<TestClass2>(b) == false);
	CHECK(rtti_cast<TestClass*>(a) == a);
	CHECK(IsOfType<TestClass>(a) == true);
	CHECK(IsOfType<TestClass2>(a) == false);
	CHECK(b->GetTypeInfo() == RTTI::TypeInfo::Get<TestClass>());
	CHECK(b->GetTypeInfo() != RTTI::TypeInfo::Get<TestClass2>());
	delete a;

	TestClass2* a2 = new TestClass2();
	RTTIBase* b2 = a2;
	CHECK(rtti_cast<TestClass*>(b2) == nullptr);
	CHECK(rtti_cast<TestClass2*>(b2) == a2);
	CHECK(IsOfType<TestClass>(b2) == false);
	CHECK(IsOfType<TestClass2>(b2) == true);
	CHECK(rtti_cast<TestClass2*>(a2) == a2);
	CHECK(IsOfType<TestClass>(a2) == false);
	CHECK(IsOfType<TestClass2>(a2) == true);
	CHECK(b2->GetTypeInfo() != RTTI::TypeInfo::Get<TestClass>());
	CHECK(b2->GetTypeInfo() == RTTI::TypeInfo::Get<TestClass2>());
	delete a2;
}

TEST_CASE("RTTI property", "[RTTI]") {
	TestClass* a = new TestClass();
	RTTIBase* b = a;
	
	auto propMgr = b->GetPropertyManager();
	REQUIRE(propMgr != nullptr);
	
	const auto& properties = propMgr->GetPropertyList();
	REQUIRE(properties.GetSize() == 2+1); // Val1, Val2 + UUID
	
	CHECK(properties[0].Type == RTTI::TypeInfo::INVALID);
	CHECK(properties[0].Name == "UUID");

	CHECK(properties[1].Type == RTTI::TypeInfo::INVALID);
	CHECK(properties[1].Name == "Val1");
	CHECK((char*)b + properties[1].Offset == (char*)&(a->val1));

	CHECK(properties[2].Type == RTTI::TypeInfo::INVALID);
	CHECK(properties[2].Name == "Val2");
	CHECK((char*)b + properties[2].Offset == (char*)&(a->val2));
}
