#include <pe/Defines.hpp>
#include <catch.hpp>

#include <ECS/Scene.hpp>
#include <ECS/DeferredTaskSystem.hpp>
// test components
#include <Audio/SoundListenerComponent.hpp>
#include <Movement/FreeFloatMovementComponent.hpp>
#include <Rendering/PostprocessSettingsComponent.hpp>

using namespace Poly;

TEST_CASE("Scene component iteration tests.", "ComponentIterator")
{
	Scene* w = new Scene();
	DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(w);
	Entity* e[10];

	e[0] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[0]);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[0]);

	e[1] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[1]);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[1]);

	e[2] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[2]);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[2]);

	e[3] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[3]);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[3]);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[3]);

	e[4] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[4]);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[4]);

	e[5] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[5]);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[5]);

	e[6] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[6]);

	e[7] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[7]);

	e[8] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[8]);

	e[9] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[9]);

	//	
	//	SoundListenerComponent			|	0 1 2 3      6 7 8 9
	//	FreeFloatMovementComponent		|	  1 2 3 4 5
	//	PostprocessSettingsComponent	|	0 	  3 4 5
	//

	int i = 0;
	for (auto c : w->IterateComponents<SoundListenerComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 8);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 5);

	i = 0;
	for (auto c : w->IterateComponents<PostprocessSettingsComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 4);

	i = 0;
	for (auto c : w->IterateComponents<SoundListenerComponent, FreeFloatMovementComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 3);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent, SoundListenerComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 3);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent, PostprocessSettingsComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 3);

	i = 0;
	for (auto c : w->IterateComponents<PostprocessSettingsComponent, FreeFloatMovementComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 3);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent, SoundListenerComponent, PostprocessSettingsComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 1);

	i = 0;
	for (auto c : w->IterateComponents<PostprocessSettingsComponent, FreeFloatMovementComponent, SoundListenerComponent>())
	{
		UNUSED(c);
		++i;
	}
	REQUIRE(i == 1);

}