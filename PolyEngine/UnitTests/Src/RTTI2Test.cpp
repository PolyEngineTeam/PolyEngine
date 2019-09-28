//#include <pe/Defines.hpp>
#include <catch.hpp>
#include <pe/core/rtti/RTTI2.hpp>

using namespace pe::core::rtti;

struct TestAttr {} constexpr testAttr;

struct TestClass : public virtual RTTI2::RTTIBase {
	TestClass() : RTTI2::RTTIBase(RTTI2::TypeManager::get().registerOrGetType<TestClass>()) {}
};
template <> struct RTTI2::RTTIinfo<TestClass> {
	constexpr static auto info = List(testAttr, testAttr, RTTI2::classname{ "testname" });
};

TEST_CASE("RTTI2 simple attribute", "[RTTI2]") {
	TestClass tc{};
	CHECK(tc.typeInfo.id != std::type_index{ typeid(void) });
	auto ta = tc.typeInfo.get<TestAttr>();
	CHECK(ta);
	CHECK(ta->size() == 2);
	CHECK(tc.typeInfo.get<int>() == nullptr);
	auto x = tc.typeInfo.get<RTTI2::classname>();
	CHECK(x);
	CHECK(std::string("testname") == x->name);
}

struct TestAttrUniq {} constexpr testAttrUniq;
template <> struct RTTI2::AttrType<TestAttrUniq> {
	using type = RTTI2::UniqueAttribute<TestAttrUniq>;
};
struct TestClass2 : public virtual RTTI2::RTTIBase {
	TestClass2() : RTTI2::RTTIBase(RTTI2::TypeManager::get().registerOrGetType<TestClass2>()) {}
};
template <> struct RTTI2::RTTIinfo<TestClass2> {
	constexpr static auto info = List(testAttrUniq, testAttrUniq);
};

TEST_CASE("RTTI2 uniq attribute", "[RTTI2]")
{
	REQUIRE_THROWS(TestClass2{});
}

struct TestAttrBase { const char* foo; };
template <> struct RTTI2::AttrType<TestAttrBase> {
	using type = RTTI2::DerivedAttribute<TestAttrBase>;
};
struct TestAttrDerived : TestAttrBase {};
template <> struct RTTI2::AttrType<TestAttrDerived> {
	using type = RTTI2::DerivedAttribute<TestAttrBase>;
};

struct TestClassA : public virtual RTTI2::RTTIBase {
	TestClassA() : RTTI2::RTTIBase(RTTI2::TypeManager::get().registerOrGetType<TestClassA>()) {}
};
template <> struct RTTI2::RTTIinfo<TestClassA> {
	constexpr static auto info = List(TestAttrDerived{ "foo" });
};

struct TestClassB : public virtual RTTI2::RTTIBase, public TestClassA {
	TestClassB() : RTTI2::RTTIBase(RTTI2::TypeManager::get().registerOrGetType<TestClassB>()) {}
};
template <> struct RTTI2::RTTIinfo<TestClassB> {
	constexpr static auto info = List(RTTI2::baseclass<TestClassA>{});
};

TEST_CASE("RTTI2 derived", "[RTTI2]") {
	TestClassA tca{};
	auto attr = tca.typeInfo.get<TestAttrBase>();
	CHECK(attr);
	CHECK(std::string("foo") == attr->at(0)->foo);
	TestClassB tcb{};
	CHECK(tcb.typeInfo.bases[0].get().id == tca.typeInfo.id);
	CHECK(RTTI2::isSame<TestClassA>(tca.typeInfo));
	CHECK(RTTI2::isDerived<TestClassA>(tcb.typeInfo));
}