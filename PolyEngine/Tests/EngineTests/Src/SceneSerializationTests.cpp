#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/rtti/RTTI.hpp>
#include <ECS/Scene.hpp>
#include <cstdio>

#include <Movement/FreeFloatMovementComponent.hpp>
#include <ECS/DeferredTaskWorldComponent.hpp>

using namespace Poly;

TEST_CASE("Scene serialization tests", "[Scene]")
{
	// @todo fix this abomination
	Engine* engine = new Engine(true);
   UNUSED(engine);

	{
		Scene s;
		DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(&s);

		Entity* e = DeferredTaskSystem::SpawnEntityImmediate(&s);
		DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(&s, e);

		s.SerializeToFile("Scene.test");
	}
	{
		Scene s;
		s.DeserializeFromFile("Scene.test");
	}

   // @fixme this can be uncomented when all issues with engine closing are fixed.
   //delete engine;
}