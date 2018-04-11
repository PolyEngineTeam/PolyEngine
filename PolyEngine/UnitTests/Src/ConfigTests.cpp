#include <Defines.hpp>
#include <catch.hpp>

#include <RTTI/RTTI.hpp>
#include <Configs/ConfigBase.hpp>
#include <Math/BasicMath.hpp>
#include <cstdio>

using namespace Poly;

enum class eConfigTest
{
	VAL_1,
	VAL_2,
	_COUNT
};
REGISTER_ENUM_NAMES(eConfigTest, "val_1", "val_2");

class TestRTTIClass : public RTTIBase
{
	RTTI_DECLARE_TYPE_DERIVED(TestRTTIClass, RTTIBase)
	{
		RTTI_PROPERTY_AUTONAME(Val1, RTTI::ePropertyFlag::NONE);
	}
public:
	TestRTTIClass() {}
	TestRTTIClass(int val) : Val1(val) {}

	int Val1 = 0;
};
RTTI_DEFINE_TYPE(TestRTTIClass)

class TestConfig : public ConfigBase
{
	RTTI_DECLARE_TYPE_DERIVED(TestConfig, ConfigBase)
	{
		RTTI_PROPERTY_AUTONAME(PropBool, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropI8, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropI16, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropI32, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropI64, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropU8, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropU16, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropU32, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropU64, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(Propf32, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(Propf64, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropStr, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropEnum, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropDynarrayInt, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropDynarrayDynarrayInt, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropDynarrayString, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropDynarrayCustom, RTTI::ePropertyFlag::NONE);
	}
public:
	TestConfig() : ConfigBase("Test", eResourceSource::NONE) {}

	// Rendering
	bool PropBool = true;
	i8 PropI8 = -1;
	i16 PropI16 = -2;
	i32 PropI32 = -3;
	i64 PropI64 = -4;
	u8 PropU8 = 1;
	u16 PropU16 = 2;
	u32 PropU32 = 3;
	u64 PropU64 = 4;
	float Propf32 = 0.32f;
	double Propf64 = 0.32;
	String PropStr = "Test string";
	eConfigTest PropEnum = eConfigTest::VAL_2;
	Dynarray<int> PropDynarrayInt = { 1, 2, 3 };
	Dynarray<Dynarray<int>> PropDynarrayDynarrayInt = { {1}, { 2, 3 } };
	Dynarray<String> PropDynarrayString = { "abc", "efg" };
	Dynarray<TestRTTIClass> PropDynarrayCustom = { 1, 2 };
};
RTTI_DEFINE_TYPE(TestConfig)

TEST_CASE("Config serialization tests", "[ConfigBase]")
{
	{
		TestConfig config;
		// perform first basic save, to ensure file is created, and with proper values
		config.Save();
		// load it
		config.Load();

		CHECK(config.PropBool == true);
		CHECK(config.PropI8 == -1);
		CHECK(config.PropI16 == -2);
		CHECK(config.PropI32 == -3);
		CHECK(config.PropI64 == -4);
		CHECK(config.PropU8 == 1);
		CHECK(config.PropU16 == 2);
		CHECK(config.PropU32 == 3);
		CHECK(config.PropU64 == 4);
		CHECK(config.Propf32 == Approx(0.32f));
		CHECK(config.Propf64 == Approx(0.32));
		CHECK(config.PropStr == "Test string");
		CHECK(config.PropEnum == eConfigTest::VAL_2);
		REQUIRE(config.PropDynarrayInt.GetSize() == 3);
		CHECK(config.PropDynarrayInt[0] == 1);
		CHECK(config.PropDynarrayInt[1] == 2);
		CHECK(config.PropDynarrayInt[2] == 3);

		REQUIRE(config.PropDynarrayDynarrayInt.GetSize() == 2);
		REQUIRE(config.PropDynarrayDynarrayInt[0].GetSize() == 1);
		CHECK(config.PropDynarrayDynarrayInt[0][0] == 1);
		REQUIRE(config.PropDynarrayDynarrayInt[1].GetSize() == 2);
		CHECK(config.PropDynarrayDynarrayInt[1][0] == 2);
		CHECK(config.PropDynarrayDynarrayInt[1][1] == 3);

		REQUIRE(config.PropDynarrayString.GetSize() == 2);
		CHECK(config.PropDynarrayString[0] == "abc");
		CHECK(config.PropDynarrayString[1] == "efg");

		REQUIRE(config.PropDynarrayCustom.GetSize() == 2);
		CHECK(config.PropDynarrayCustom[0].Val1 == 1);
		CHECK(config.PropDynarrayCustom[1].Val1 == 2);


		// Change values
		config.PropBool = false;
		config.PropI8 = -5;
		config.PropI16 = -6;
		config.PropI32 = -7;
		config.PropI64 = -8;
		config.PropU8 = 5;
		config.PropU16 = 6;
		config.PropU32 = 7;
		config.PropU64 = 8;
		config.Propf32 = 0.64f;
		config.Propf64 = 0.64f;
		config.PropStr = "Test string2";
		config.PropEnum = eConfigTest::VAL_1;
		config.PropDynarrayInt = {4, 5, 6, 7};
		config.PropDynarrayDynarrayInt = { { 1, 2 }, { 3 }, {4, 5, 6} };
		config.PropDynarrayString = { "123", "456", "789" };
		config.PropDynarrayCustom = { 3, 4, 5, 6 };

		// save them
		config.Save();
	}

	{
		TestConfig config;

		// check values again
		CHECK(config.PropBool == true);
		CHECK(config.PropI8 == -1);
		CHECK(config.PropI16 == -2);
		CHECK(config.PropI32 == -3);
		CHECK(config.PropI64 == -4);
		CHECK(config.PropU8 == 1);
		CHECK(config.PropU16 == 2);
		CHECK(config.PropU32 == 3);
		CHECK(config.PropU64 == 4);
		CHECK(config.Propf32 == Approx(0.32f));
		CHECK(config.Propf64 == Approx(0.32));
		CHECK(config.PropStr == "Test string");
		CHECK(config.PropEnum == eConfigTest::VAL_2);
		REQUIRE(config.PropDynarrayInt.GetSize() == 3);
		CHECK(config.PropDynarrayInt[0] == 1);
		CHECK(config.PropDynarrayInt[1] == 2);
		CHECK(config.PropDynarrayInt[2] == 3);

		REQUIRE(config.PropDynarrayDynarrayInt.GetSize() == 2);
		REQUIRE(config.PropDynarrayDynarrayInt[0].GetSize() == 1);
		CHECK(config.PropDynarrayDynarrayInt[0][0] == 1);
		REQUIRE(config.PropDynarrayDynarrayInt[1].GetSize() == 2);
		CHECK(config.PropDynarrayDynarrayInt[1][0] == 2);
		CHECK(config.PropDynarrayDynarrayInt[1][1] == 3);

		REQUIRE(config.PropDynarrayString.GetSize() == 2);
		CHECK(config.PropDynarrayString[0] == "abc");
		CHECK(config.PropDynarrayString[1] == "efg");

		REQUIRE(config.PropDynarrayCustom.GetSize() == 2);
		CHECK(config.PropDynarrayCustom[0].Val1 == 1);
		CHECK(config.PropDynarrayCustom[1].Val1 == 2);

		// load old values
		config.Load();

		// check values again after load
		CHECK(config.PropBool == false);
		CHECK(config.PropI8 == -5);
		CHECK(config.PropI16 == -6);
		CHECK(config.PropI32 == -7);
		CHECK(config.PropI64 == -8);
		CHECK(config.PropU8 == 5);
		CHECK(config.PropU16 == 6);
		CHECK(config.PropU32 == 7);
		CHECK(config.PropU64 == 8);
		CHECK(config.Propf32 == Approx(0.64f));
		CHECK(config.Propf64 == Approx(0.64));
		CHECK(config.PropStr == "Test string2");
		CHECK(config.PropEnum == eConfigTest::VAL_1);
		REQUIRE(config.PropDynarrayInt.GetSize() == 4);
		CHECK(config.PropDynarrayInt[0] == 4);
		CHECK(config.PropDynarrayInt[1] == 5);
		CHECK(config.PropDynarrayInt[2] == 6);
		CHECK(config.PropDynarrayInt[3] == 7);

		REQUIRE(config.PropDynarrayDynarrayInt.GetSize() == 3);
		REQUIRE(config.PropDynarrayDynarrayInt[0].GetSize() == 2);
		CHECK(config.PropDynarrayDynarrayInt[0][0] == 1);
		CHECK(config.PropDynarrayDynarrayInt[0][1] == 2);
		REQUIRE(config.PropDynarrayDynarrayInt[1].GetSize() == 1);
		CHECK(config.PropDynarrayDynarrayInt[1][0] == 3);
		REQUIRE(config.PropDynarrayDynarrayInt[2].GetSize() == 3);
		CHECK(config.PropDynarrayDynarrayInt[2][0] == 4);
		CHECK(config.PropDynarrayDynarrayInt[2][1] == 5);
		CHECK(config.PropDynarrayDynarrayInt[2][2] == 6);

		REQUIRE(config.PropDynarrayString.GetSize() == 3);
		CHECK(config.PropDynarrayString[0] == "123");
		CHECK(config.PropDynarrayString[1] == "456");
		CHECK(config.PropDynarrayString[2] == "789");

		REQUIRE(config.PropDynarrayCustom.GetSize() == 4);
		CHECK(config.PropDynarrayCustom[0].Val1 == 3);
		CHECK(config.PropDynarrayCustom[1].Val1 == 4);
		CHECK(config.PropDynarrayCustom[2].Val1 == 5);
		CHECK(config.PropDynarrayCustom[3].Val1 == 6);
	}

	// remove the config file
	remove("TestConfig.json");
}