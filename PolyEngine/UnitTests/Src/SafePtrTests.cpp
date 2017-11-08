#include <catch.hpp>

#include <Defines.hpp>
#include <SafePtrRoot.hpp>
#include <SafePtr.hpp>

using namespace Poly;

TEST_CASE("Safe pointer tests", "[SafePtr]") {
	SafePtrRoot safePtrObject = SafePtrRoot();
	SafePtr<SafePtrRoot> safePtr = SafePtr<SafePtrRoot>(&safePtrObject);

	REQUIRE(safePtr->Debug() == safePtrObject.Debug());
	REQUIRE(*safePtr == &safePtrObject);

	SafePtrRoot *object2 = new SafePtrRoot();
	SafePtr<SafePtrRoot> ptr2 = SafePtr<SafePtrRoot>(object2);
	delete object2;
	REQUIRE(*ptr2 == nullptr);
}