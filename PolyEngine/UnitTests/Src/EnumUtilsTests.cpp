#include <catch.hpp>

#include <EnumUtils.hpp>


using namespace Poly;

enum class eTestEnum : uint8_t
{
	VAL_1,
	VAL_2,
	VAL_3,
	_COUNT
};

REGISTER_ENUM_NAMES(eTestEnum, "Val1", "Val2", "Val3" );


TEST_CASE("EnumArray tests", "[EnumArray]")
{
	EnumArray<int, eTestEnum> a1;
	REQUIRE(a1.GetSize() == (int)eTestEnum::_COUNT);
	EnumArray<int, eTestEnum, 2> a2;
	REQUIRE(a2.GetSize() == 2);

	EnumArray<int, eTestEnum> a3{1, 2, 3};
	REQUIRE(a3.GetSize() == (int)eTestEnum::_COUNT);
	REQUIRE(a3[eTestEnum::VAL_1] == 1);
	REQUIRE(a3[eTestEnum::VAL_2] == 2);
	REQUIRE(a3[eTestEnum::VAL_3] == 3);

	REQUIRE(a1 != a3);

	a1[eTestEnum::VAL_1] = 1;
	a1[eTestEnum::VAL_2] = 2;
	a1[eTestEnum::VAL_3] = 3;
	REQUIRE(a1 == a3);

	REQUIRE(strcmp(GetEnumName(eTestEnum::VAL_1), "Val1") == 0);
	REQUIRE(strcmp(GetEnumName(eTestEnum::VAL_2), "Val2") == 0);
	REQUIRE(strcmp(GetEnumName(eTestEnum::VAL_3), "Val3") == 0);
}
