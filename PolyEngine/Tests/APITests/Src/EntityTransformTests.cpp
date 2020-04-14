#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/api/ecs/Scene.hpp>
#include <pe/api/ecs/Entity.hpp>

using namespace ::pe::api::ecs;

TEST_CASE("Checking equality of transforms after applying global and local translation rotation and scale.", "[GlobalLocalTransform]")
{
	Scene s;
	const int entitiesCount = 6;
	Entity* e[entitiesCount];

	e[0] = s.spawnEntity();	// root
		e[1] = s.spawnEntity(e[0]);
			e[2] = s.spawnEntity(e[1]);
		e[3] = s.spawnEntity(e[1]);
		e[4] = s.spawnEntity(e[1]);
		e[5] = s.spawnEntity(e[1]);

	for (int a = 0; a < entitiesCount; ++a)
		for (int b = 0; b < entitiesCount; ++b)
		{
			REQUIRE(e[a]->getTransform().getGlobalTranslation() == e[b]->getTransform().getGlobalTranslation());
			REQUIRE(e[a]->getTransform().getGlobalRotation() == e[b]->getTransform().getGlobalRotation());
			REQUIRE(e[a]->getTransform().getGlobalScale() == e[b]->getTransform().getGlobalScale());

			REQUIRE(e[a]->getTransform().getLocalTranslation() == e[b]->getTransform().getLocalTranslation());
			REQUIRE(e[a]->getTransform().getLocalRotation() == e[b]->getTransform().getLocalRotation());
			REQUIRE(e[a]->getTransform().getLocalScale() == e[b]->getTransform().getLocalScale());
		}

	SECTION("Translation.")
	{
		e[1]->getTransform().setLocalTranslation({ 5, 7, 8 });
		REQUIRE(e[1]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ 5, 7, 8 });
		REQUIRE(e[1]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 5, 7, 8 });
		REQUIRE(e[2]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ 0, 0, 0 });
		REQUIRE(e[2]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 5, 7, 8 });

		e[1]->getTransform().setGlobalTranslation({ 56, 34, 98 });
		REQUIRE(e[1]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[1]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[2]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ 0, 0, 0 });
		REQUIRE(e[2]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });

		e[2]->getTransform().setLocalTranslation({ 1, 2, 3 });
		REQUIRE(e[1]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[1]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[2]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ 1, 2, 3 });
		REQUIRE(e[2]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 57, 36, 101 });

		e[2]->getTransform().setGlobalTranslation({ 20, 50, 80 });
		REQUIRE(e[1]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[1]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 56, 34, 98 });
		REQUIRE(e[2]->getTransform().getLocalTranslation() == ::pe::core::math::Vector{ -36, 16, -18 });
		REQUIRE(e[2]->getTransform().getGlobalTranslation() == ::pe::core::math::Vector{ 20, 50, 80 });
	}

	SECTION("Rotation.")
	{
		e[1]->getTransform().setLocalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[1]->getTransform().getLocalRotation() == ::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[1]->getTransform().getGlobalRotation() == ::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[2]->getTransform().getLocalRotation() == ::pe::core::math::EulerAngles{ 0_deg, 0_deg, 0_deg });
		REQUIRE(e[2]->getTransform().getGlobalRotation() == ::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });

		e[1]->getTransform().setGlobalRotation(::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->getTransform().getLocalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->getTransform().getGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[2]->getTransform().getLocalRotation() == ::pe::core::math::EulerAngles{ 0_deg, 0_deg, 0_deg });
		REQUIRE(e[2]->getTransform().getGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		
		e[2]->getTransform().setLocalRotation(::pe::core::math::EulerAngles{ 1_deg, 2_deg, 3_deg });
		REQUIRE(e[1]->getTransform().getLocalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->getTransform().getGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[2]->getTransform().getLocalRotation() == ::pe::core::math::EulerAngles{ 1_deg, 2_deg, 3_deg });

		e[2]->getTransform().setGlobalRotation(::pe::core::math::EulerAngles{ 20_deg, 50_deg, 80_deg });
		REQUIRE(e[1]->getTransform().getLocalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[1]->getTransform().getGlobalRotation() == ::pe::core::math::EulerAngles{ 56_deg, 34_deg, 98_deg });
		REQUIRE(e[2]->getTransform().getGlobalRotation() == ::pe::core::math::EulerAngles{ 20_deg, 50_deg, 80_deg });
	}

	SECTION("Local and global set translation with the same pareint in {0, 0, 0}.")
	{
		e[1]->getTransform().setLocalTranslation({ 5, 7, 8 });
		e[3]->getTransform().setGlobalTranslation({ 5, 7, 8 });
		REQUIRE(e[1]->getTransform().getGlobalTranslation() == e[3]->getTransform().getGlobalTranslation());
		REQUIRE(e[1]->getTransform().getGlobalRotation() == e[3]->getTransform().getGlobalRotation());
		REQUIRE(e[1]->getTransform().getGlobalScale() == e[3]->getTransform().getGlobalScale());
	
		e[4]->getTransform().setLocalTranslation({ 5, 7, 5 });
		e[5]->getTransform().setGlobalTranslation({ 5, 7, 8 });
		REQUIRE(e[4]->getTransform().getGlobalTranslation() != e[5]->getTransform().getGlobalTranslation());
		REQUIRE(e[4]->getTransform().getGlobalRotation() == e[5]->getTransform().getGlobalRotation());
		REQUIRE(e[4]->getTransform().getGlobalScale() == e[5]->getTransform().getGlobalScale());
	}
	
	SECTION("Local and global set rotation with the same pareint in {0, 0, 0}.")
	{
		e[1]->getTransform().setLocalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		e[3]->getTransform().setGlobalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[1]->getTransform().getGlobalTranslation() == e[3]->getTransform().getGlobalTranslation());
		REQUIRE(e[1]->getTransform().getGlobalRotation() == e[3]->getTransform().getGlobalRotation());
		REQUIRE(e[1]->getTransform().getGlobalScale() == e[3]->getTransform().getGlobalScale());
	
		e[4]->getTransform().setLocalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 5_deg });
		e[5]->getTransform().setGlobalRotation(::pe::core::math::EulerAngles{ 5_deg, 7_deg, 8_deg });
		REQUIRE(e[4]->getTransform().getGlobalTranslation() == e[5]->getTransform().getGlobalTranslation());
		REQUIRE(e[4]->getTransform().getGlobalRotation() != e[5]->getTransform().getGlobalRotation());
		REQUIRE(e[4]->getTransform().getGlobalScale() == e[5]->getTransform().getGlobalScale());
	}
	
	SECTION("Local and global set scale with the same pareint in {0, 0, 0}.")
	{
		e[1]->getTransform().setLocalScale({ 5, 7, 8 });
		e[3]->getTransform().setGlobalScale({ 5, 7, 8 });
		REQUIRE(e[1]->getTransform().getGlobalTranslation() == e[3]->getTransform().getGlobalTranslation());
		REQUIRE(e[1]->getTransform().getGlobalRotation() == e[3]->getTransform().getGlobalRotation());
		REQUIRE(e[1]->getTransform().getGlobalScale() == e[3]->getTransform().getGlobalScale());
	
		e[4]->getTransform().setLocalScale({ 5, 7, 5 });
		e[5]->getTransform().setGlobalScale({ 5, 7, 8 });
		REQUIRE(e[4]->getTransform().getGlobalTranslation() == e[5]->getTransform().getGlobalTranslation());
		REQUIRE(e[4]->getTransform().getGlobalRotation() == e[5]->getTransform().getGlobalRotation());
		REQUIRE(e[4]->getTransform().getGlobalScale() != e[5]->getTransform().getGlobalScale());
	}
}