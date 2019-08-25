#include <pe/Defines.hpp>
#include <catch.hpp>

#include <core::UniqueID.hpp>

using namespace Poly;

TEST_CASE("String conversions", "[UniqueID]")
{
	core::UniqueID a = core::UniqueID::FromString("abcdef01-2345-6789-abcd-ef0123456789").value();
	CHECK(a.ToString() == core::storage::String("abcdef01-2345-6789-abcd-ef0123456789"));
}
