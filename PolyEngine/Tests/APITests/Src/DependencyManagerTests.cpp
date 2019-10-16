#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/api/deps/DependencyManager.hpp>
#include <pe/api/deps/DependencyPtr.hpp>

class TestDependency : public pe::api::deps::IDependency
{
	
}


TEST_CASE("DependencyPtr basics", "[DependencyPtr]")
{
	pe::api::deps::DependencyPtr<TestDependency> ptr;
}
