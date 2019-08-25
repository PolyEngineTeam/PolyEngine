#include <EnginePCH.hpp>

#include <Audio/SoundSystem.hpp>
#include <Audio/SoundEmitterComponent.hpp>
#include <Audio/SoundWorldComponent.hpp>
#include <ECS/Scene.hpp>
#include <Resources/ResourceBase.hpp>
#include <Resources/SoundResource.hpp>
#include <Resources/ResourceManager.hpp>

using namespace Poly;

//---------------------------------------------------------------------------------------------------
void SoundSystem::SoundPhase(Scene* world)
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
void SoundSystem::SetDevice(const core::storage::String& name)
{
	alcCloseDevice(gEngine->GetAudioDevice().Device);
	gEngine->GetAudioDevice().Device = alcOpenDevice(name.GetCStr());
	HEAVY_ASSERTE(gEngine->GetAudioDevice().Device, "OpenAL device creation failed");
}

//---------------------------------------------------------------------------------------------------
const core::storage::String& SoundSystem::GetDeviceName()
{
	return *(gEngine->GetAudioDevice().AvailableDevices.begin());
}

//---------------------------------------------------------------------------------------------------
const std::vector<core::storage::String>& SoundSystem::GetAvailableDevices()
{
	return gEngine->GetAudioDevice().AvailableDevices;
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetWorldCurrent(Scene* world)
{
	alcMakeContextCurrent(world->GetWorldComponent<SoundWorldComponent>()->Context);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::PlayEmitter(Scene* world, Entity* entity)
{
	int state;
	alGetSourcei(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_SOURCE_STATE, &state);

	if(state != AL_PLAYING)
		alSourcePlay(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::ReplayEmitter(Scene* world, Entity* entity)
{
	alSourcePlay(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::PauseEmitter(Scene* world, Entity* entity)
{
	alSourcePause(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::StopEmitter(Scene* world, Entity* entity)
{
	alSourceStop(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID());
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::LoopEmitter(Scene* world, Entity* entity)
{
	alSourcei(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_LOOPING, true);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::UnLoopEmitter(Scene* world, Entity* entity)
{
	alSourcei(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_LOOPING, false);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterGain(Scene* world, Entity* entity, float gain)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_GAIN, gain);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterFrequency(Scene* world, Entity* entity, float frequency)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_PITCH, frequency);
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInSeconds(Scene* world, Entity* entity, float offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_SEC_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInSamples(Scene* world, Entity* entity, size_t offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_SAMPLE_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterOffsetInBytes(Scene* world, Entity* entity, size_t offset)
{
	alSourcef(world->GetComponent<SoundEmitterComponent>(entity)->GetEmitterID(), AL_BYTE_OFFSET, static_cast<float>(offset));
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterSource(Scene* world, Entity* entity, const core::storage::String& path, eResourceSource source)
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
/*void SoundSystem::QueueEmitterSource(Scene* world, Entity* entity, const core::storage::String&)
{
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::DequeueEmitterSource(Scene* world, Entity* entity, const core::storage::String&)
{
}

//---------------------------------------------------------------------------------------------------
void SoundSystem::SetEmitterDistanceModel(Scene* world, Entity* entity, void (*fun)(SoundEmitterComponent, SoundListenerComponent))
{
}*/

void SoundSystem::ActivateListener(Scene* /*world*/, Entity* /*entity*/)
{
}

void SoundSystem::DeactivateListener(Scene* /*world*/, Entity* /*entity*/)
{
}

void SoundSystem::SetListenerPosition(core::math::Vector vec)
{
	alListener3f(AL_POSITION, vec.X, vec.Y, vec.Z);
}

bool ENGINE_DLLEXPORT Poly::SoundSystem::IsEmmiterActive(Scene* world, Entity* entity)
{
	ALint state;
	SoundEmitterComponent* emitter = world->GetComponent<SoundEmitterComponent>(entity);
	alGetSourcei(emitter->GetEmitterID(), AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}
