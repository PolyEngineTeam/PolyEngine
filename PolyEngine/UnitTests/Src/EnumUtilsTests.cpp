#include <catch.hpp>

#include <EnumUtils.hpp>


using namespace Poly;

enum class eTestEnum
{
	VAL_1,
	VAL_2,
	VAL_3,
	_COUNT
};

REGISTER_ENUM_NAMES(eTestEnum, "Val1", "Val2", "Val3" );

enum class eTestFlagsEnum
{
	FLAG_1 = 0x01,
	FLAG_2 = 0x02,
	FLAG_3 = 0x04
};


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

TEST_CASE("EnumFlags tests", "[EnumFlags]")
{
	EnumFlags<eTestFlagsEnum> a1;
	REQUIRE(a1.IsSet(eTestFlagsEnum::FLAG_1) == false);
	REQUIRE(a1.IsSet(eTestFlagsEnum::FLAG_2) == false);
	REQUIRE(a1.IsSet(eTestFlagsEnum::FLAG_3) == false);
	REQUIRE(a1.IsSet(eTestFlagsEnum::FLAG_1 | eTestFlagsEnum::FLAG_2 | eTestFlagsEnum::FLAG_3) == false);

	EnumFlags<eTestFlagsEnum> a2 = eTestFlagsEnum::FLAG_1 | eTestFlagsEnum::FLAG_2;
	REQUIRE(a2.IsSet(eTestFlagsEnum::FLAG_1) == true);
	REQUIRE(a2.IsSet(eTestFlagsEnum::FLAG_2) == true);
	REQUIRE(a2.IsSet(eTestFlagsEnum::FLAG_3) == false);
	REQUIRE(a2.IsSet(eTestFlagsEnum::FLAG_1 | eTestFlagsEnum::FLAG_2) == true);
	REQUIRE(a2.IsSet(eTestFlagsEnum::FLAG_1 | eTestFlagsEnum::FLAG_2 | eTestFlagsEnum::FLAG_3) == false);
	REQUIRE(a2.AnySet(eTestFlagsEnum::FLAG_1 | eTestFlagsEnum::FLAG_3) == true);
	REQUIRE(a2.AnySet(eTestFlagsEnum::FLAG_3) == false);
	REQUIRE(a2.NoneSet(eTestFlagsEnum::FLAG_1 | eTestFlagsEnum::FLAG_3) == false);
	REQUIRE(a2.NoneSet(eTestFlagsEnum::FLAG_3) == true);

	a2 |= eTestFlagsEnum::FLAG_3;
	REQUIRE(a2.IsSet(eTestFlagsEnum::FLAG_1 | eTestFlagsEnum::FLAG_2 | eTestFlagsEnum::FLAG_3) == true);
}

TEST_CASE("EnumIterator tests", "[EnumIterator]")
{
	int i = 0;
	for (auto e : IterateEnum<eTestEnum>())
	{
		REQUIRE(i++ == (int)e);
	}
	REQUIRE(i == (int)eTestEnum::_COUNT);
}
