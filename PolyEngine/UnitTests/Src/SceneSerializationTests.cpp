#include <Defines.hpp>
#include <catch.hpp>

#include <RTTI/RTTI.hpp>
#include <ECS/Scene.hpp>
#include <cstdio>

#include <Movement/FreeFloatMovementComponent.hpp>
#include <ECS/DeferredTaskWorldComponent.hpp>

using namespace Poly;

TEST_CASE("Scene serialization tests", "[Scene]")
{
	Scene s;
	DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(&s);
	
	Entity* e = DeferredTaskSystem::SpawnEntityImmediate(&s);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(&s, e);

	s.SerializeToFile("Scene.test");
	s.DeserializeFromFile("Scene.test");
}