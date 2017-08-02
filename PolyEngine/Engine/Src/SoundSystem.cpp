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
void SoundSystem::PlayEmitter(World* world, const UniqueID& id)
{
	int state;
	alGetSourcei(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_SOURCE_STATE, &state);

	if(state != AL_PLAYING)
		alSourcePlay(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::ReplayEmitter(World* world, const UniqueID& id)
{
	alSourcePlay(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::PauseEmitter(World* world, const UniqueID& id)
{
	alSourcePause(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::StopEmitter(World* world, const UniqueID& id)
{
	alSourceStop(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::LoopEmitter(World* world, const UniqueID& id)
{
	alSourcei(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_LOOPING, true);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::UnLoopEmitter(World* world, const UniqueID& id)
{
	alSourcei(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_LOOPING, false);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterGain(World* world, const UniqueID& id, float gain)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_GAIN, gain);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterFrequency(World* world, const UniqueID& id, float frequency)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_PITCH, frequency);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInSeconds(World* world, const UniqueID& id, float offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_SEC_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInSamples(World* world, const UniqueID& id, size_t offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_SAMPLE_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInBytes(World* world, const UniqueID& id, size_t offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(id)->GetEmitterID(), AL_BYTE_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterSource(World* world, const UniqueID& id, const String& path, eResourceSource source)
{
	SoundEmitterComponent* emitter = world->GetComponent<SoundEmitterComponent>(id);

	alDeleteSources(1, &emitter->EmitterID);
	ResourceManager<SoundResource>::Release(emitter->Resource);

	emitter->Resource = ResourceManager<SoundResource>::Load(path, source);

	alGenSources(1, &emitter->EmitterID);
	alSourcei(emitter->EmitterID, AL_BUFFER, emitter->Resource->GetBufferID());
}

//---------------------------------------------------------------------------------------------------
/*void SoundSystem::QueueEmitterSource(World* world, const UniqueID& id, const String&)
{
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::DequeueEmitterSource(World* world, const UniqueID& id, const String&)
{
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterDistanceModel(World* world, const UniqueID& id, void (*fun)(SoundEmitterComponent, SoundListenerComponent))
{
}*/

void SoundSystem::ActivateListener(World* world, const UniqueID& id)
{
}

void SoundSystem::DeactivateListener(World* world, const UniqueID& id)
{
}

void SoundSystem::SetListenerGain(World* world, const UniqueID& id, float gain)
{
}

bool ENGINE_DLLEXPORT Poly::SoundSystem::IsEmmiterActive(World* world, const UniqueID& id)
{
	ALint state;
	SoundEmitterComponent* emitter = world->GetComponent<SoundEmitterComponent>(id);
	alGetSourcei(emitter->GetEmitterID(), AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}
