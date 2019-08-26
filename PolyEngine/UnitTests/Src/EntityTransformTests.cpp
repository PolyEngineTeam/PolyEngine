#include <pe/Defines.hpp>
#include <catch.hpp>

#include <ECS/Entity.hpp>

using namespace Poly;

TEST_CASE("Checking equality of transforms after applying global and local translation rotation and scale.", "[GlobalLocalTransform]")
{
	const int entitiesCount = 6;
	Entity* e[entitiesCount];

	e[0] = new Entity();	// root
		(e[1] = new Entity())->SetParent(e[0]);
			(e[2] = new Entity())->SetParent(e[1]);
		(e[3] = new Entity())->SetParent(e[1]);
		(e[4] = new Entity())->SetParent(e[1]);
		(e[5] = new Entity())->SetParent(e[1]);

	for (int a = 0; a < entitiesCount; ++a)
		for (int b = 0; b < entitiesCount; ++b)
		{
			REQUIRE(e[a]->GetTransform().GetGlobalTranslation() == e[b]->GetTransform().GetGlobalTranslation());
			REQUIRE(e[a]->GetTransform().GetGlobalRotation() == e[b]->GetTransform().GetGlobalRotation());
			REQUIRE(e[a]->GetTransform().GetGlobalScale() == e[b]->GetTransform().GetGlobalScale());

			REQUIRE(e[a]->GetTransform().GetLocalTranslation() == e[b]->GetTransform().GetLocalTranslation());
			REQUIRE(e[a]->GetTransform().GetLocalRotation() == e[b]->GetTransform().GetLocalRotation());
			REQUIRE(e[a]->GetTransform().GetLocalScale() == e[b]->GetTransform().GetLocalScale());
		}

	SECTION("Translation.")
	{
		e[1]->GetTransform().SetLocalTranslation({ 5, 7, 8 });
		REQUIRE(e[1]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ 5, 7, 8 });
		REQUIRE(e[1]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 5, 7, 8 });
		REQUIRE(e[2]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ 0, 0, 0 });
		REQUIRE(e[2]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 5, 7, 8 });

		e[1]->GetTransform().SetGlobalTranslation({ 56, 34, 98 });
		REQUIRE(e[1]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[1]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[2]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ 0, 0, 0 });
		REQUIRE(e[2]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });

		e[2]->GetTransform().SetLocalTranslation({ 1, 2, 3 });
		REQUIRE(e[1]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[1]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[2]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ 1, 2, 3 });
		REQUIRE(e[2]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 57, 36, 101 });

		e[2]->GetTransform().SetGlobalTranslation({ 20, 50, 80 });
		REQUIRE(e[1]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[1]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[2]->GetTransform().GetLocalTranslation() == ::pe::core::math::Vector{ -36, 16, -18 });
		REQUIRE(e[2]->GetTransform().GetGlobalTranslation() == ::pe::core::math::Vector{ 20, 50, 80 });
	}

	SECTION("Rotation.")
	{
		e[1]->GetTransform().SetLocalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[1]->GetTransform().GetLocalRotation() == ::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[1]->GetTransform().GetGlobalRotation() == ::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[2]->GetTransform().GetLocalRotation() == ::pe::core::math::EulerAngles{ 0_deg, 0_deg, 0_deg });
		REQUIRE(e[2]->GetTransform().GetGlobalRotation() == ::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });

		e[1]->GetTransform().SetGlobalRotation(::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->GetTransform().GetLocalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->GetTransform().GetGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[2]->GetTransform().GetLocalRotation() == ::pe::core::math::EulerAngles{ 0_deg, 0_deg, 0_deg });
		REQUIRE(e[2]->GetTransform().GetGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		
		e[2]->GetTransform().SetLocalRotation(::pe::core::math::EulerAngles{ 1_deg, 2_deg, 3_deg });
		REQUIRE(e[1]->GetTransform().GetLocalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->GetTransform().GetGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[2]->GetTransform().GetLocalRotation() == ::pe::core::math::EulerAngles{ 1_deg, 2_deg, 3_deg });

		e[2]->GetTransform().SetGlobalRotation(::pe::core::math::EulerAngles{ 20_deg, 50_deg, 80_deg });
		REQUIRE(e[1]->GetTransform().GetLocalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->GetTransform().GetGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[2]->GetTransform().GetGlobalRotation() == ::pe::core::math::EulerAngles{ 20_deg, 50_deg, 80_deg });
	}

	SECTION("Local and global set translation with the same pareint in {0, 0, 0}.")
	{
		e[1]->GetTransform().SetLocalTranslation({ 5, 7, 8 });
		e[3]->GetTransform().SetGlobalTranslation({ 5, 7, 8 });
		REQUIRE(e[1]->GetTransform().GetGlobalTranslation() == e[3]->GetTransform().GetGlobalTranslation());
		REQUIRE(e[1]->GetTransform().GetGlobalRotation() == e[3]->GetTransform().GetGlobalRotation());
		REQUIRE(e[1]->GetTransform().GetGlobalScale() == e[3]->GetTransform().GetGlobalScale());
	
		e[4]->GetTransform().SetLocalTranslation({ 5, 7, 5 });
		e[5]->GetTransform().SetGlobalTranslation({ 5, 7, 8 });
		REQUIRE(e[4]->GetTransform().GetGlobalTranslation() != e[5]->GetTransform().GetGlobalTranslation());
		REQUIRE(e[4]->GetTransform().GetGlobalRotation() == e[5]->GetTransform().GetGlobalRotation());
		REQUIRE(e[4]->GetTransform().GetGlobalScale() == e[5]->GetTransform().GetGlobalScale());
	}
	
	SECTION("Local and global set rotation with the same pareint in {0, 0, 0}.")
	{
		e[1]->GetTransform().SetLocalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		e[3]->GetTransform().SetGlobalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[1]->GetTransform().GetGlobalTranslation() == e[3]->GetTransform().GetGlobalTranslation());
		REQUIRE(e[1]->GetTransform().GetGlobalRotation() == e[3]->GetTransform().GetGlobalRotation());
		REQUIRE(e[1]->GetTransform().GetGlobalScale() == e[3]->GetTransform().GetGlobalScale());
	
		e[4]->GetTransform().SetLocalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 5_deg });
		e[5]->GetTransform().SetGlobalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[4]->GetTransform().GetGlobalTranslation() == e[5]->GetTransform().GetGlobalTranslation());
		REQUIRE(e[4]->GetTransform().GetGlobalRotation() != e[5]->GetTransform().GetGlobalRotation());
		REQUIRE(e[4]->GetTransform().GetGlobalScale() == e[5]->GetTransform().GetGlobalScale());
	}
	
	SECTION("Local and global set scale with the same pareint in {0, 0, 0}.")
	{
		e[1]->GetTransform().SetLocalScale({ 5, 7, 8 });
		e[3]->GetTransform().SetGlobalScale({ 5, 7, 8 });
		REQUIRE(e[1]->GetTransform().GetGlobalTranslation() == e[3]->GetTransform().GetGlobalTranslation());
		REQUIRE(e[1]->GetTransform().GetGlobalRotation() == e[3]->GetTransform().GetGlobalRotation());
		REQUIRE(e[1]->GetTransform().GetGlobalScale() == e[3]->GetTransform().GetGlobalScale());
	
		e[4]->GetTransform().SetLocalScale({ 5, 7, 5 });
		e[5]->GetTransform().SetGlobalScale({ 5, 7, 8 });
		REQUIRE(e[4]->GetTransform().GetGlobalTranslation() == e[5]->GetTransform().GetGlobalTranslation());
		REQUIRE(e[4]->GetTransform().GetGlobalRotation() == e[5]->GetTransform().GetGlobalRotation());
		REQUIRE(e[4]->GetTransform().GetGlobalScale() != e[5]->GetTransform().GetGlobalScale());
	}
}