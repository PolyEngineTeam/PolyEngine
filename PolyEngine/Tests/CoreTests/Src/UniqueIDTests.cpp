#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/UniqueID.hpp>

TEST_CASE("String conversions", "[UniqueID]")
{
	::pe::core::UniqueID a = ::pe::core::UniqueID::FromString("abcdef01-2345-6789-abcd-ef0123456789").value();
	CHECK(a.ToString() == ::pe::core::storage::String("abcdef01-2345-6789-abcd-ef0123456789"));
}
