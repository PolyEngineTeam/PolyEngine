#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/api/deps/DependencyManager.hpp>
#include <pe/api/deps/DependencyPtr.hpp>

class TestDependency : public pe::api::deps::DependencyBase
{
	std::type_index getType() const override
	{
		return typeid(TestDependency);
	}
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
