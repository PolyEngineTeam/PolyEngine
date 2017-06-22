#include "EnginePCH.hpp"

#include <al.h> 
#include <alc.h>

#include "SoundSystem.hpp"

using namespace Poly;

void SoundSystem::SoundPhase(World* world)
{
	SoundEmitterComponent* emitter;
	ALint state;

	for (auto it : world->IterateComponents<SoundEmitterComponent>())
	{
		emitter = std::get<SoundEmitterComponent*>(it);
		alGetSourcei(emitter->GetEmitterID(), AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING)
			alSourcePlay(emitter->GetEmitterID());
	}
}


void SoundSystem::SetCurrentWorld(World* world)
{
	alcMakeContextCurrent(world->GetWorldComponent<SoundWorldComponent>()->Context);
}