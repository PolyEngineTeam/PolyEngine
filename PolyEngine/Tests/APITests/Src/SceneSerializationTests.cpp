#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/rtti/RTTI.hpp>
#include <pe/api/ecs/Scene.hpp>
#include <cstdio>

//#include <Movement/FreeFloatMovementComponent.hpp>
//#include <pe/api/ecs/DeferredTaskWorldComponent.hpp>

using namespace pe::api::ecs;

TEST_CASE("Scene serialization tests", "[Scene]")
{
	{
		Scene s;
		//DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(&s);

		DISCARD s.spawnEntity();
		//DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(&s, e);

		s.SerializeToFile("Scene.test");
	}
	{
		Scene s;
		s.DeserializeFromFile("Scene.test");
	}

   // @fixme this can be uncomented when all issues with engine closing are fixed.
   //delete engine;
}