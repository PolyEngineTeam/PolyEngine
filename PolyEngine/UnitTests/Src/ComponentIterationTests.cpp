#include <Defines.hpp>
#include <catch.hpp>

#include <ECS/World.hpp>
#include <ECS/DeferredTaskSystem.hpp>
// test components
#include <Audio/SoundListenerComponent.hpp>
#include <Movement/FreeFloatMovementComponent.hpp>
#include <Rendering/PostprocessSettingsComponent.hpp>

using namespace Poly;

TEST_CASE("World component iteration tests.", "ComponentIterator")
{
	World* w = new World();
	DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(w);
	Entity* e[10];

	e[0] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[0]);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[0]);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[0]);

	e[1] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[1]);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[1]);

	e[2] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[2]);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[2]);

	e[3] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[3]);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[3]);

	e[4] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<SoundListenerComponent>(w, e[4]);

	e[5] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[5]);

	e[6] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[6]);

	e[7] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(w, e[7]);

	e[8] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[8]);

	e[9] = DeferredTaskSystem::SpawnEntityImmediate(w);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, e[9]);

	//	
	//	SoundListenerComponent			|	0 1 3 4
	//	FreeFloatMovementComponent		|	0 1 2 5 7
	//	PostprocessSettingsComponent	|	0 2 3 6 8 9 
	//

	int i = 0;
	for (auto c : w->IterateComponents<SoundListenerComponent>())
		++i;
	REQUIRE(i == 4);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent>())
		++i;
	REQUIRE(i == 5);

	i = 0;
	for (auto c : w->IterateComponents<PostprocessSettingsComponent>())
		++i;
	REQUIRE(i == 6);

	i = 0;
	for (auto c : w->IterateComponents<SoundListenerComponent, FreeFloatMovementComponent>())
		++i;
	REQUIRE(i == 2);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent, SoundListenerComponent>())
		++i;
	REQUIRE(i == 2);

	i = 0;
	for (auto c : w->IterateComponents<SoundListenerComponent, PostprocessSettingsComponent>())
		++i;
	REQUIRE(i == 2);

	i = 0;
	for (auto c : w->IterateComponents<PostprocessSettingsComponent, SoundListenerComponent>())
		++i;
	REQUIRE(i == 2);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent, PostprocessSettingsComponent>())
		++i;
	REQUIRE(i == 2);

	i = 0;
	for (auto c : w->IterateComponents<PostprocessSettingsComponent, FreeFloatMovementComponent>())
		++i;
	REQUIRE(i == 2);

	i = 0;
	for (auto c : w->IterateComponents<SoundListenerComponent, FreeFloatMovementComponent, PostprocessSettingsComponent>())
		++i;
	REQUIRE(i == 1);

	i = 0;
	for (auto c : w->IterateComponents<FreeFloatMovementComponent, PostprocessSettingsComponent, SoundListenerComponent>())
		++i;
	REQUIRE(i == 1);

	i = 0;
	for (auto c : w->IterateComponents<PostprocessSettingsComponent, SoundListenerComponent, FreeFloatMovementComponent>())
		++i;
	REQUIRE(i == 1);
}