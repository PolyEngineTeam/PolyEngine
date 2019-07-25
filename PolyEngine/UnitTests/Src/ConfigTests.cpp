#include <Defines.hpp>
#include <catch.hpp>

#include <RTTI/RTTI.hpp>
#include <Configs/ConfigBase.hpp>
#include <Math/BasicMath.hpp>
#include <Math/Vector.hpp>
#include <Math/Vector2f.hpp>
#include <Math/Vector2i.hpp>
#include <Math/Matrix.hpp>
#include <Math/Color.hpp>
#include <Math/Quaternion.hpp>
#include <UniqueID.hpp>
#include <cstdio>
#include <Utils/Logger.hpp>
#include <vector>

using namespace Poly;

enum class eConfigTest
{
	VAL_1,
	VAL_2,
	_COUNT
};
REGISTER_ENUM_NAMES(eConfigTest, "val_1", "val_2");

enum class eConfigFlagsTest
{
	VAL_1 = 0x01,
	VAL_2 = 0x02,
	VAL_3 = 0x04,
	VAL_4 = 0x08,
	VAL_5 = 0x10
};

static size_t gFactoryCounterInt = 0;
static size_t gFactoryCounterDynarrayInt = 0;
static size_t gFactoryCounterCustom = 0;


class TestRTTIClass : public RTTIBase
{
	RTTI_DECLARE_TYPE_DERIVED(TestRTTIClass, RTTIBase)
	{
		RTTI_PROPERTY_AUTONAME(Val1, RTTI::ePropertyFlag::NONE);
	}
public:
	TestRTTIClass() {}
	TestRTTIClass(int val) : Val1(val) {}

	bool operator==(const TestRTTIClass& rhs) const { return Val1 == rhs.Val1; }
	bool operator!=(const TestRTTIClass& rhs) const { return !(*this == rhs); }

	friend std::ostream& operator<< (std::ostream& stream, const TestRTTIClass& rhs)
	{
		return stream << "TestRTTIClass[ " << rhs.Val1 << " ]";
	}

	int Val1 = 0;
};
RTTI_DEFINE_TYPE(TestRTTIClass)

class TestRTTIClassPolymorphic : public TestRTTIClass
{
	RTTI_DECLARE_TYPE_DERIVED(TestRTTIClassPolymorphic, TestRTTIClass)
	{
		RTTI_PROPERTY_AUTONAME(Val2, RTTI::ePropertyFlag::NONE);
	}
public:
	TestRTTIClassPolymorphic() {}
	TestRTTIClassPolymorphic(int val, float val2) : TestRTTIClass(val), Val2(val2) {}

	float Val2 = 0.f;
};
RTTI_DEFINE_TYPE(TestRTTIClassPolymorphic)

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
		RTTI_PROPERTY_AUTONAME(PropStdVectorInt, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropStdVectorStdVectorInt, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropStdVectorString, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropStdVectorCustom, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropOMapIntInt, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropOMapStrDynarray, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropOMapStrCustom, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropVector, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropVector2f, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropVector2i, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropQuat, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropAngle, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropColor, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropMatrix, RTTI::ePropertyFlag::NONE);

		RTTI_PROPERTY_AUTONAME(PropEnumArrayInt, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropEnumArrayDynarray, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropEnumArrayCustom, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropEnumFlags, RTTI::ePropertyFlag::NONE);

		RTTI_PROPERTY_AUTONAME(PropUUID, RTTI::ePropertyFlag::NONE);

		RTTI_PROPERTY_FACTORY_AUTONAME(PropUniquePtrInt, [](Poly::RTTI::TypeInfo info) { ++gFactoryCounterInt; return new int; }, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_FACTORY_AUTONAME(PropUniquePtrDynarrayInt, [](Poly::RTTI::TypeInfo info) { ++gFactoryCounterDynarrayInt; return new Dynarray<int>; }, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_FACTORY_AUTONAME(PropUniquePtrCustom, [](Poly::RTTI::TypeInfo info) { ++gFactoryCounterCustom; return info.CreateInstance(); }, RTTI::ePropertyFlag::NONE);
		RTTI_PROPERTY_AUTONAME(PropUniquePtrPolymorphicCustom, RTTI::ePropertyFlag::NONE);

		RTTI_PROPERTY_AUTONAME(PropRawPtrCustom, RTTI::ePropertyFlag::NONE);
	}
public:
	TestConfig() : ConfigBase("Test", eResourceSource::NONE) 
	{
		PropOMapIntInt.insert({1, 2});
		PropOMapIntInt.insert({2, 3});

		PropOMapStrDynarray.insert({"Val1", { 1, 2, 3 }});
		PropOMapStrDynarray.insert({"Val2", { 4, 5, 6 }});

		PropOMapStrCustom.insert({"Val1", 1});
		PropOMapStrCustom.insert({"Val2", 2});

		PropMatrix.SetRotationZ(60_deg);

		PropUniquePtrInt = std::make_unique<int>(5);
		PropUniquePtrDynarrayInt = std::make_unique<Dynarray<int>>(Dynarray<int>{ 1,2,3 });
		PropUniquePtrCustom = std::make_unique<TestRTTIClass>(3);
		PropUniquePtrPolymorphicCustom = std::make_unique<TestRTTIClass>(12);

		PropRawPtrCustom = PropUniquePtrCustom.get();
	}

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

	std::vector<int> PropStdVectorInt = { 1, 2, 3 };
	std::vector<std::vector<int>> PropStdVectorStdVectorInt = { {1}, { 2, 3 } };
	std::vector<String> PropStdVectorString = { "abc", "efg" };
	std::vector<TestRTTIClass> PropStdVectorCustom = { 1, 2 };

	std::map<int, int> PropOMapIntInt;
	std::map<String, Dynarray<int>> PropOMapStrDynarray;
	std::map<String, TestRTTIClass> PropOMapStrCustom;

	Vector PropVector = Vector(1,2,3);
	Vector2f PropVector2f = Vector2f(1, 2);
	Vector2i PropVector2i = Vector2i(1, 2);
	Quaternion PropQuat = Quaternion(Vector::UNIT_Z, 60_deg);
	Angle PropAngle = 60_deg;
	Color PropColor = Color(1, 0.5f, 0.3f, 1);
	Matrix PropMatrix;

	EnumArray<int, eConfigTest> PropEnumArrayInt = { { eConfigTest::VAL_1, 1 },{ eConfigTest::VAL_2, 2 } };
	EnumArray<Dynarray<int>, eConfigTest> PropEnumArrayDynarray = { { eConfigTest::VAL_1, {1, 2} },{ eConfigTest::VAL_2, {2, 3} } };
	EnumArray<TestRTTIClass, eConfigTest> PropEnumArrayCustom = { { eConfigTest::VAL_1, 1}, { eConfigTest::VAL_2, 2} };
	EnumFlags<eConfigFlagsTest> PropEnumFlags = (eConfigFlagsTest::VAL_1 | eConfigFlagsTest::VAL_3 | eConfigFlagsTest::VAL_5);

	UniqueID PropUUID = UniqueID::FromString("01234567-89ab-cdef-0123-456789abcdef").Value();

	std::unique_ptr<int> PropUniquePtrInt;
	std::unique_ptr<Dynarray<int>> PropUniquePtrDynarrayInt;
	std::unique_ptr<TestRTTIClass> PropUniquePtrCustom;
	std::unique_ptr<RTTIBase> PropUniquePtrPolymorphicCustom;

	TestRTTIClass* PropRawPtrCustom;
};
RTTI_DEFINE_TYPE(TestConfig)

void baseValueCheck(const TestConfig& config)
{
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

	REQUIRE(config.PropStdVectorInt.size() == 3);
	CHECK(config.PropStdVectorInt[0] == 1);
	CHECK(config.PropStdVectorInt[1] == 2);
	CHECK(config.PropStdVectorInt[2] == 3);

	REQUIRE(config.PropStdVectorStdVectorInt.size() == 2);
	REQUIRE(config.PropStdVectorStdVectorInt[0].size() == 1);
	CHECK(config.PropStdVectorStdVectorInt[0][0] == 1);
	REQUIRE(config.PropStdVectorStdVectorInt[1].size() == 2);
	CHECK(config.PropStdVectorStdVectorInt[1][0] == 2);
	CHECK(config.PropStdVectorStdVectorInt[1][1] == 3);

	REQUIRE(config.PropStdVectorString.size() == 2);
	CHECK(config.PropStdVectorString[0] == "abc");
	CHECK(config.PropStdVectorString[1] == "efg");

	REQUIRE(config.PropStdVectorCustom.size() == 2);
	CHECK(config.PropStdVectorCustom[0].Val1 == 1);
	CHECK(config.PropStdVectorCustom[1].Val1 == 2);

	REQUIRE(config.PropOMapIntInt.size() == 2);
	REQUIRE(config.PropOMapIntInt.find(1) != config.PropOMapIntInt.end());
	CHECK(config.PropOMapIntInt.at(1) == 2);
	REQUIRE(config.PropOMapIntInt.find(2) != config.PropOMapIntInt.end());
	CHECK(config.PropOMapIntInt.at(2) == 3);

	REQUIRE(config.PropOMapStrDynarray.size() == 2);
	REQUIRE(config.PropOMapStrDynarray.find("Val1") != config.PropOMapStrDynarray.end());
	REQUIRE(config.PropOMapStrDynarray.at("Val1").GetSize() == 3);
	CHECK(config.PropOMapStrDynarray.at("Val1")[0] == 1);
	CHECK(config.PropOMapStrDynarray.at("Val1")[1] == 2);
	CHECK(config.PropOMapStrDynarray.at("Val1")[2] == 3);
	REQUIRE(config.PropOMapStrDynarray.find("Val2") != config.PropOMapStrDynarray.end());
	REQUIRE(config.PropOMapStrDynarray.at("Val2").GetSize() == 3);
	CHECK(config.PropOMapStrDynarray.at("Val2")[0] == 4);
	CHECK(config.PropOMapStrDynarray.at("Val2")[1] == 5);
	CHECK(config.PropOMapStrDynarray.at("Val2")[2] == 6);

	REQUIRE(config.PropOMapStrCustom.size() == 2);
	REQUIRE(config.PropOMapStrCustom.find("Val1") != config.PropOMapStrCustom.end());
	CHECK(config.PropOMapStrCustom.at("Val1").Val1 == 1);
	REQUIRE(config.PropOMapStrCustom.find("Val2") != config.PropOMapStrCustom.end());
	CHECK(config.PropOMapStrCustom.at("Val2").Val1 == 2);

	CHECK(config.PropVector == Vector(1, 2, 3));
	CHECK(config.PropVector2f == Vector2f(1, 2));
	CHECK(config.PropVector2i == Vector2i(1, 2));
	CHECK(config.PropQuat == Quaternion(Vector::UNIT_Z, 60_deg));
	CHECK(config.PropAngle == 60_deg);
	CHECK(config.PropColor == Color(1, 0.5f, 0.3f, 1));
	Matrix testMat;
	testMat.SetRotationZ(60_deg);
	CHECK(config.PropMatrix == testMat);

	CHECK(config.PropEnumArrayInt == EnumArray<int, eConfigTest>{ { eConfigTest::VAL_1, 1 },{ eConfigTest::VAL_2, 2 } });
	CHECK(config.PropEnumArrayDynarray == EnumArray<Dynarray<int>, eConfigTest>{ { eConfigTest::VAL_1,{ 1, 2 } },{ eConfigTest::VAL_2,{ 2, 3 } } });
	CHECK(config.PropEnumArrayCustom == EnumArray<TestRTTIClass, eConfigTest>{ { eConfigTest::VAL_1, 1 },{ eConfigTest::VAL_2, 2 } });
	CHECK(config.PropEnumFlags == (eConfigFlagsTest::VAL_1 | eConfigFlagsTest::VAL_3 | eConfigFlagsTest::VAL_5));

	CHECK(config.PropUUID == UniqueID::FromString("01234567-89ab-cdef-0123-456789abcdef").Value());

	CHECK(*config.PropUniquePtrInt == 5);
	CHECK(*config.PropUniquePtrDynarrayInt == Dynarray<int>{1,2,3});
	CHECK(config.PropUniquePtrCustom->Val1 == 3);
	TestRTTIClass* ptr = rtti_cast<TestRTTIClass*>(config.PropUniquePtrPolymorphicCustom.get());
	REQUIRE(ptr != nullptr);
	CHECK(ptr->Val1 == 12);

	CHECK(config.PropRawPtrCustom == config.PropUniquePtrCustom.get());
}

TEST_CASE("Config serialization tests", "[ConfigBase]")
{
	REQUIRE(gFactoryCounterInt == 0);
	REQUIRE(gFactoryCounterDynarrayInt == 0);
	REQUIRE(gFactoryCounterCustom == 0);

	{
		TestConfig config;
		// perform first basic save, to ensure file is created, and with proper values
		config.Save();
		// load it
		config.Load();

		REQUIRE(gFactoryCounterInt == 1);
		REQUIRE(gFactoryCounterDynarrayInt == 1);
		REQUIRE(gFactoryCounterCustom == 1);

		baseValueCheck(config);

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

		config.PropStdVectorInt = { 4, 5, 6, 7 };
		config.PropStdVectorStdVectorInt = { { 1, 2 }, { 3 }, {4, 5, 6} };
		config.PropStdVectorString = { "123", "456", "789" };
		config.PropStdVectorCustom = { 3, 4, 5, 6 };

		config.PropOMapIntInt.erase(1);
		config.PropOMapIntInt[2] = 4;
		config.PropOMapIntInt.insert({3, 5});
		config.PropOMapIntInt.insert({4, 6});

		config.PropOMapStrDynarray.erase("Val1");
		config.PropOMapStrDynarray.at("Val2").PushBack(9);
		config.PropOMapStrDynarray.insert({"Val3", { 7,8 }});

		config.PropOMapStrCustom.erase("Val1");
		config.PropOMapStrCustom.at("Val2").Val1 = 5;
		config.PropOMapStrCustom.insert({"Val3", 3});
		config.PropOMapStrCustom.insert({"Val4", 4});

		config.PropVector = Vector(4, 5, 6);
		config.PropVector2f = Vector2f(3, 4);
		config.PropVector2i = Vector2i(3, 4);
		config.PropQuat = Quaternion(Vector::UNIT_X, 75_deg);
		config.PropAngle = 75_deg;
		config.PropColor = Color(0.5f, 1, 0.8f, 1);
		config.PropMatrix.SetRotationX(75_deg);

		config.PropEnumArrayInt = { { eConfigTest::VAL_1, 3 },{ eConfigTest::VAL_2, 4 } };
		config.PropEnumArrayDynarray = { { eConfigTest::VAL_1,{ 3, 4 } },{ eConfigTest::VAL_2,{ 4, 5 } } };
		config.PropEnumArrayCustom = { { eConfigTest::VAL_1, 3 },{ eConfigTest::VAL_2, 4 } };
		config.PropEnumFlags = (eConfigFlagsTest::VAL_1 | eConfigFlagsTest::VAL_2 | eConfigFlagsTest::VAL_4);

		config.PropUUID = UniqueID::FromString("abcdef01-2345-6789-abcd-ef0123456789").Value();

		config.PropUniquePtrInt = std::make_unique<int>(7);
		config.PropUniquePtrDynarrayInt = std::make_unique<Dynarray<int>>(Dynarray<int>{ 4, 5, 6 });
		config.PropUniquePtrCustom = std::make_unique<TestRTTIClass>(8);
		config.PropUniquePtrPolymorphicCustom = std::make_unique<TestRTTIClassPolymorphic>(17, 1.5f);

		config.PropRawPtrCustom = config.PropUniquePtrCustom.get();

		// save them
		config.Save();
	}

	{
		TestConfig config;

		// check values again
		baseValueCheck(config);

		// load old values
		config.Load();

		REQUIRE(gFactoryCounterInt == 2);
		REQUIRE(gFactoryCounterDynarrayInt == 2);
		REQUIRE(gFactoryCounterCustom == 2);

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

		REQUIRE(config.PropStdVectorInt.size() == 4);
		CHECK(config.PropStdVectorInt[0] == 4);
		CHECK(config.PropStdVectorInt[1] == 5);
		CHECK(config.PropStdVectorInt[2] == 6);
		CHECK(config.PropStdVectorInt[3] == 7);

		REQUIRE(config.PropStdVectorStdVectorInt.size() == 3);
		REQUIRE(config.PropStdVectorStdVectorInt[0].size() == 2);
		CHECK(config.PropStdVectorStdVectorInt[0][0] == 1);
		CHECK(config.PropStdVectorStdVectorInt[0][1] == 2);
		REQUIRE(config.PropStdVectorStdVectorInt[1].size() == 1);
		CHECK(config.PropStdVectorStdVectorInt[1][0] == 3);
		REQUIRE(config.PropStdVectorStdVectorInt[2].size() == 3);
		CHECK(config.PropStdVectorStdVectorInt[2][0] == 4);
		CHECK(config.PropStdVectorStdVectorInt[2][1] == 5);
		CHECK(config.PropStdVectorStdVectorInt[2][2] == 6);

		REQUIRE(config.PropStdVectorString.size() == 3);
		CHECK(config.PropStdVectorString[0] == "123");
		CHECK(config.PropStdVectorString[1] == "456");
		CHECK(config.PropStdVectorString[2] == "789");

		REQUIRE(config.PropStdVectorCustom.size() == 4);
		CHECK(config.PropStdVectorCustom[0].Val1 == 3);
		CHECK(config.PropStdVectorCustom[1].Val1 == 4);
		CHECK(config.PropStdVectorCustom[2].Val1 == 5);
		CHECK(config.PropStdVectorCustom[3].Val1 == 6);

		REQUIRE(config.PropOMapIntInt.size() == 3);
		CHECK(config.PropOMapIntInt.find(1) == config.PropOMapIntInt.end());
		REQUIRE(config.PropOMapIntInt.find(2) != config.PropOMapIntInt.end());
		CHECK(config.PropOMapIntInt.at(2) == 4);
		REQUIRE(config.PropOMapIntInt.find(3) != config.PropOMapIntInt.end());
		CHECK(config.PropOMapIntInt.at(3) == 5);
		REQUIRE(config.PropOMapIntInt.find(4) != config.PropOMapIntInt.end());
		CHECK(config.PropOMapIntInt.at(4) == 6);

		REQUIRE(config.PropOMapStrDynarray.size() == 2);
		CHECK(config.PropOMapStrDynarray.find("Val1") == config.PropOMapStrDynarray.end());
		REQUIRE(config.PropOMapStrDynarray.find("Val2") != config.PropOMapStrDynarray.end());
		REQUIRE(config.PropOMapStrDynarray.at("Val2").GetSize() == 4);
		CHECK(config.PropOMapStrDynarray.at("Val2")[0] == 4);
		CHECK(config.PropOMapStrDynarray.at("Val2")[1] == 5);
		CHECK(config.PropOMapStrDynarray.at("Val2")[2] == 6);
		CHECK(config.PropOMapStrDynarray.at("Val2")[3] == 9);
		REQUIRE(config.PropOMapStrDynarray.find("Val3") != config.PropOMapStrDynarray.end());
		REQUIRE(config.PropOMapStrDynarray.at("Val3").GetSize() == 2);
		CHECK(config.PropOMapStrDynarray.at("Val3")[0] == 7);
		CHECK(config.PropOMapStrDynarray.at("Val3")[1] == 8);

		REQUIRE(config.PropOMapStrCustom.size() == 3);
		CHECK(config.PropOMapStrCustom.find("Val1") == config.PropOMapStrCustom.end());
		REQUIRE(config.PropOMapStrCustom.find("Val2") != config.PropOMapStrCustom.end());
		CHECK(config.PropOMapStrCustom.at("Val2").Val1 == 5);
		REQUIRE(config.PropOMapStrCustom.find("Val3") != config.PropOMapStrCustom.end());
		CHECK(config.PropOMapStrCustom.at("Val3").Val1 == 3);
		REQUIRE(config.PropOMapStrCustom.find("Val4") != config.PropOMapStrCustom.end());
		CHECK(config.PropOMapStrCustom.at("Val4").Val1 == 4);

		CHECK(config.PropVector == Vector(4, 5, 6));
		CHECK(config.PropVector2f == Vector2f(3, 4));
		CHECK(config.PropVector2i == Vector2i(3, 4));
		CHECK(config.PropQuat == Quaternion(Vector::UNIT_X, 75_deg));
		CHECK(config.PropAngle == 75_deg);
		CHECK(config.PropColor == Color(0.5f, 1, 0.8f, 1));
		Matrix testMat;
		testMat.SetRotationX(75_deg);
		CHECK(config.PropMatrix == testMat);

		CHECK(config.PropEnumArrayInt == EnumArray<int, eConfigTest>{ { eConfigTest::VAL_1, 3 }, { eConfigTest::VAL_2, 4 } });
		CHECK(config.PropEnumArrayDynarray == EnumArray<Dynarray<int>, eConfigTest>{ { eConfigTest::VAL_1, { 3, 4 } }, { eConfigTest::VAL_2,{ 4, 5 } } });
		CHECK(config.PropEnumArrayCustom == EnumArray<TestRTTIClass, eConfigTest>{ { eConfigTest::VAL_1, 3 }, { eConfigTest::VAL_2, 4 } });
		CHECK(config.PropEnumFlags == (eConfigFlagsTest::VAL_1 | eConfigFlagsTest::VAL_2 | eConfigFlagsTest::VAL_4));

		CHECK(config.PropUUID == UniqueID::FromString("abcdef01-2345-6789-abcd-ef0123456789").Value());

		CHECK(*config.PropUniquePtrInt == 7);
		CHECK(*config.PropUniquePtrDynarrayInt == Dynarray<int>{4,5,6});
		CHECK(config.PropUniquePtrCustom->Val1 == 8);
		TestRTTIClassPolymorphic* ptr = rtti_cast<TestRTTIClassPolymorphic*>(config.PropUniquePtrPolymorphicCustom.get());
		REQUIRE(ptr != nullptr);
		CHECK(ptr->Val1 == 17);
		CHECK(ptr->Val2 == 1.5f);

		CHECK(config.PropRawPtrCustom == config.PropUniquePtrCustom.get());
	}

	// remove the config file
	remove("TestConfig.json");
}