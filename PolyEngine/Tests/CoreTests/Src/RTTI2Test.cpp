//#include <pe/Defines.hpp>
#include <catch.hpp>
#include <pe/core/rtti/RTTI2.hpp>

namespace rtti2 = pe::core::rtti::RTTI2;

struct TestAttr {} constexpr testAttr;

struct TestClass : public virtual rtti2::RTTIBase {
	TestClass() : rtti2::RTTIBase(rtti2::TypeManager::get().registerOrGetType<TestClass>()) {}
};
template <> struct rtti2::RTTIinfo<TestClass> {
	constexpr static auto info = List(testAttr, testAttr, rtti2::ClassName{ "testname" });
};

TEST_CASE("rtti2 simple attribute", "[rtti2]") {
	TestClass tc{};
	CHECK(tc.typeInfo().id != std::type_index{ typeid(void) });
	auto ta = tc.typeInfo().get<TestAttr>();
	CHECK(ta);
	CHECK(ta->size() == 2);
	CHECK(tc.typeInfo().get<int>() == nullptr);
	auto x = tc.typeInfo().get<rtti2::ClassName>();
	CHECK(x);
	CHECK(std::string("testname") == x->name);
}

struct TestAttrUniq {} constexpr testAttrUniq;
template <> struct rtti2::AttrType<TestAttrUniq> {
	using type = rtti2::UniqueAttribute<TestAttrUniq>;
};
struct TestClass2 : public virtual rtti2::RTTIBase {
	TestClass2() : rtti2::RTTIBase(rtti2::TypeManager::get().registerOrGetType<TestClass2>()) {}
};
template <> struct rtti2::RTTIinfo<TestClass2> {
	constexpr static auto info = List(testAttrUniq, testAttrUniq);
};

TEST_CASE("rtti2 uniq attribute", "[rtti2]")
{
	REQUIRE_THROWS(TestClass2{});
}

struct TestAttrBase { const char* foo; };
template <> struct rtti2::AttrType<TestAttrBase> {
	using type = rtti2::DerivedAttribute<TestAttrBase>;
};
struct TestAttrDerived : TestAttrBase {};
template <> struct rtti2::AttrType<TestAttrDerived> {
	using type = rtti2::DerivedAttribute<TestAttrBase>;
};

struct TestClassA : public virtual rtti2::RTTIBase {
	TestClassA() : rtti2::RTTIBase(rtti2::TypeManager::get().registerOrGetType<TestClassA>()) {}
};
template <> struct rtti2::RTTIinfo<TestClassA> {
	constexpr static auto info = List(TestAttrDerived{ {"foo"} });
};

struct TestClassB : public virtual rtti2::RTTIBase, public TestClassA {
	TestClassB() : rtti2::RTTIBase(rtti2::TypeManager::get().registerOrGetType<TestClassB>()) {}
};
template <> struct rtti2::RTTIinfo<TestClassB> {
	constexpr static auto info = List(rtti2::baseclass<TestClassA>{});
};

TEST_CASE("rtti2 derived", "[rtti2]") {
	TestClassA tca{};
	auto attr = tca.typeInfo().get<TestAttrBase>();
	CHECK(attr);
	CHECK(std::string("foo") == attr->at(0)->foo);
	TestClassB tcb{};
	CHECK(tcb.typeInfo().bases[0].get().id == tca.typeInfo().id);
	CHECK(rtti2::isSame<TestClassA>(tca.typeInfo()));
	CHECK(rtti2::isDerived<TestClassA>(tcb.typeInfo()));
}
