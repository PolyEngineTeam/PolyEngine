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
		alGetSourcei(emitter->SourceID, AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING)
			alSourcePlay(emitter->SourceID);
	}
}

void SoundSystem::CreateContext(World* world)
{
	world->GetWorldComponent<SoundWorldComponent>()->Context = alcCreateContext(world->GetEngine()->GetAudioRenderingContext().Device, NULL);
}

void SoundSystem::SetCurrentWorld(World* world)
{
	ALCcontext *context;
	context = world->GetWorldComponent<SoundWorldComponent>()->Context;
	alcMakeContextCurrent(context);
}

void SoundSystem::BindSoundResourceToEmitter(World *, UniqueID, SoundResource *)
{
}
