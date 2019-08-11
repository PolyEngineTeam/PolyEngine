#include <pe/Defines.hpp>
#include <catch.hpp>

#include <UniqueID.hpp>

using namespace Poly;

TEST_CASE("String conversions", "[UniqueID]")
{
	UniqueID a = UniqueID::FromString("abcdef01-2345-6789-abcd-ef0123456789").value();
	CHECK(a.ToString() == String("abcdef01-2345-6789-abcd-ef0123456789"));
}
