#include "EnginePCH.hpp"

#include "ParticleUpdateSystem.hpp"
#include "ParticleComponent.hpp"

using namespace Poly;

void ParticleUpdateSystem::ParticleUpdatePhase(World* world)
{
	// Render meshes
	for (auto compTuple : world->IterateComponents<ParticleComponent>())
	{
		ParticleComponent* particle = std::get<ParticleComponent*>(compTuple);
		particle->GetEmitter()->Update(world);
	}
}