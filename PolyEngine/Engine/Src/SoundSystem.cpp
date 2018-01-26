#include "EnginePCH.hpp"

#include "SoundSystem.hpp"

#include <al.h>
#include <alc.h>

using namespace Poly;

//---------------------------------------------------------------------------------------------------
void SoundSystem::SoundPhase(World* world)
{
	for (auto it : world->IterateComponents<SoundEmitterComponent>())
	{
		ALint state;
		SoundEmitterComponent* emitter = std::get<SoundEmitterComponent*>(it);

		alGetSourcei(emitter->GetEmitterID(), AL_SOURCE_STATE, &state);

		//if (state != AL_PLAYING)
			//alSourcePlay(emitter->GetEmitterID());
	}
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetDevice(const String& name)
{
	alcCloseDevice(gEngine->GetAudioDevice().Device);
	gEngine->GetAudioDevice().Device = alcOpenDevice(name.GetCStr());
	HEAVY_ASSERTE(gEngine->GetAudioDevice().Device, "OpenAL device creation failed");
}

//---------------------------------------------------------------------------------------------------
const String& SoundSystem::GetDeviceName()
{
	return *(gEngine->GetAudioDevice().AvailableDevices.Begin());
}

//---------------------------------------------------------------------------------------------------
const Dynarray<String>& SoundSystem::GetAvailableDevices()
{
	return gEngine->GetAudioDevice().AvailableDevices;
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetWorldCurrent(World* world)
{
	alcMakeContextCurrent(world->GetWorldComponent<SoundWorldComponent>()->Context);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::PlayEmitter(World* world, Entity* entity)
{
	int state;
	alGetSourcei(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_SOURCE_STATE, &state);

	if(state != AL_PLAYING)
		alSourcePlay(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::ReplayEmitter(World* world, Entity* entity)
{
	alSourcePlay(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::PauseEmitter(World* world, Entity* entity)
{
	alSourcePause(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::StopEmitter(World* world, Entity* entity)
{
	alSourceStop(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::LoopEmitter(World* world, Entity* entity)
{
	alSourcei(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_LOOPING, true);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::UnLoopEmitter(World* world, Entity* entity)
{
	alSourcei(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_LOOPING, false);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterGain(World* world, Entity* entity, float gain)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_GAIN, gain);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterFrequency(World* world, Entity* entity, float frequency)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_PITCH, frequency);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInSeconds(World* world, Entity* entity, float offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_SEC_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInSamples(World* world, Entity* entity, size_t offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_SAMPLE_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInBytes(World* world, Entity* entity, size_t offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_BYTE_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterSource(World* world, Entity* entity, const String& path, eResourceSource source)
{
	UNUSED(world);
	SoundEmitterComponent* emitter = entity->GetComponent<SoundEmitterComponent>();

	alDeleteSources(1, &emitter->EmitterID);
	ResourceManager<SoundResource>::Release(emitter->Resource);

	emitter->Resource = ResourceManager<SoundResource>::Load(path, source);

	alGenSources(1, &emitter->EmitterID);
	alSourcei(emitter->EmitterID, AL_BUFFER, emitter->Resource->GetBufferID());
}

//---------------------------------------------------------------------------------------------------
/*void SoundSystem::QueueEmitterSource(World* world, Entity* entity, const String&)
{
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::DequeueEmitterSource(World* world, Entity* entity, const String&)
{
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterDistanceModel(World* world, Entity* entity, void (*fun)(SoundEmitterComponent, SoundListenerComponent))
{
}*/

void SoundSystem::ActivateListener(World* /*world*/, Entity* /*entity*/)
{
}

void SoundSystem::DeactivateListener(World* /*world*/, Entity* /*entity*/)
{
}

void SoundSystem::SetListenerGain(World* /*world*/, Entity* /*entity*/, float /*gain*/)
{
}

bool ENGINE_DLLEXPORT Poly::SoundSystem::IsEmmiterActive(World* world, Entity* entity)
{
	ALint state;
	SoundEmitterComponent* emitter = world->GetComponent<SoundEmitterComponent>(entity);
	alGetSourcei(emitter->GetEmitterID(), AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}
