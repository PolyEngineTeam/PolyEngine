#include "EnginePCH.hpp"

#include "ParticleUpdateSystem.hpp"
#include "ParticleComponent.hpp"

using namespace Poly;

void ParticleUpdateSystem::ParticleUpdatePhase(World* world)
{
	// gConsole.LogInfo("ParticleUpdateSystem::ParticleUpdatePhasee");

	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));

	// Render meshes
	for (auto compTuple : world->IterateComponents<ParticleComponent>())
	{
		ParticleComponent* particle = std::get<ParticleComponent*>(compTuple);
		particle->Emitter->Update(world);
	}
}