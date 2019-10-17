#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/api/deps/DependencyManager.hpp>
#include <pe/api/deps/DependencyPtr.hpp>
#include <pe/api/deps/DependencyBase.hpp>

struct TestDependency : public pe::api::deps::DependencyBase<TestDependency>
{
	using Base = pe::api::deps::DependencyBase<TestDependency>;

	TestDependency() : Base("TestDependency") {}
};

struct OtherTestDependency : public pe::api::deps::DependencyBase<OtherTestDependency>
{
	using Base = pe::api::deps::DependencyBase<OtherTestDependency>;

	OtherTestDependency() : Base("OtherTestDependency") {}
};


TEST_CASE("No injection", "[DependencyPtr]")
{
	pe::api::deps::DependencyPtr<TestDependency> ptr;
	REQUIRE(ptr == nullptr);
}

TEST_CASE("Normal injection", "[DependencyPtr]")
{
	TestDependency dep;
	pe::api::deps::DependencyPtr<TestDependency> ptr;
	REQUIRE(ptr == &dep);
}

TEST_CASE("Reversed injection", "[DependencyPtr]")
{
	pe::api::deps::DependencyPtr<TestDependency> ptr;
	TestDependency dep;
	REQUIRE(ptr == &dep);
}

TEST_CASE("Cleanup", "[DependencyPtr]")
{
	pe::api::deps::DependencyPtr<TestDependency> ptr;
		
	{
		TestDependency dep;
		CHECK(ptr == &dep);
	}
	REQUIRE(ptr == nullptr);
}

TEST_CASE("Multi injection", "[DependencyPtr]")
{
	pe::api::deps::DependencyPtr<TestDependency> ptr1;
	
	TestDependency dep;
	OtherTestDependency otherDep;

	pe::api::deps::DependencyPtr<TestDependency> ptr2;
	
	pe::api::deps::DependencyPtr<OtherTestDependency> otherPtr1;
	pe::api::deps::DependencyPtr<OtherTestDependency> otherPtr2;

	REQUIRE(ptr1 == &dep);
	REQUIRE(ptr2 == &dep);

	REQUIRE(otherPtr1 == &otherDep);
	REQUIRE(otherPtr2 == &otherDep);
}