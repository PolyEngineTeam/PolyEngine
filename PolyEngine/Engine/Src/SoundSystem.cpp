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
	alcCloseDevice(gEngine->GetSoundDevice().Device);
	gEngine->GetSoundDevice().Device = alcOpenDevice(name.GetCStr());
	HEAVY_ASSERTE(gEngine->GetSoundDevice().Device, "OpenAL device creation failed");
}

//---------------------------------------------------------------------------------------------------
const String& SoundSystem::GetDeviceName()
{
	return *(gEngine->GetSoundDevice().AvailableDevices.Begin());
}

//---------------------------------------------------------------------------------------------------
const Dynarray<String>& SoundSystem::GetAvailableDevices()
{
	return gEngine->GetSoundDevice().AvailableDevices;
}
