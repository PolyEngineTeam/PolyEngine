#include "ALSoundDevice.hpp"

#include <al.h>

#include <World.hpp>
#include <TransformComponent.hpp>
#include <SoundEmitterComponent.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
ALSoundDevice::ALSoundDevice()
{
}

//------------------------------------------------------------------------------
ALSoundDevice::~ALSoundDevice()
{
}

//---------------------------------------------------------------------------------------------------
void SOUND_DEVICE_DLLEXPORT ALSoundDevice::Init()
{
	Device = alcOpenDevice(nullptr);
}

//---------------------------------------------------------------------------------------------------
void SOUND_DEVICE_DLLEXPORT ALSoundDevice::Close()
{
	alcCloseDevice(Device);
}

//---------------------------------------------------------------------------------------------------
void SOUND_DEVICE_DLLEXPORT ALSoundDevice::RenderWorld(World* world)
{
	Dynarray<UniqueID> newlyCreated;

	for (auto& emitterTuple : world->IterateComponents<SoundEmitterComponent, TransformComponent>())
	{
		SoundEmitterComponent* emitterCmp = std::get<SoundEmitterComponent*>(emitterTuple);
		TransformComponent* transCmp = std::get<TransformComponent*>(emitterTuple);

		if (emitterCmp->GetFlags().IsSet(eComponentBaseFlags::NEWLY_CREATED))
		{
			unsigned int emitterID;
			alGenSources(1, &emitterID);
			OwnerToEmitterMap[emitterCmp->GetOwnerID()] =  emitterID;
		}

		if (emitterCmp->GetFlags().IsSet(eComponentBaseFlags::ABOUT_TO_BE_REMOVED))
		{
			alDeleteSources(1, &OwnerToEmitterMap[emitterCmp->GetOwnerID()]);
			OwnerToEmitterMap.erase(emitterCmp->GetOwnerID());
		}

		unsigned int emitterID = OwnerToEmitterMap[emitterCmp->GetOwnerID()];

		alSourcef(emitterID, AL_PITCH, emitterCmp->Pitch);
		alSourcef(emitterID, AL_GAIN, emitterCmp->Gain);
		alSourcef(emitterID, AL_MAX_DISTANCE, emitterCmp->MaxDistance);
		alSourcef(emitterID, AL_ROLLOFF_FACTOR, emitterCmp->RolloffFactor);
		alSourcef(emitterID, AL_REFERENCE_DISTANCE, emitterCmp->RefferenceDistance);
		alSourcef(emitterID, AL_MIN_GAIN, emitterCmp->MinGain);
		alSourcef(emitterID, AL_MAX_GAIN, emitterCmp->MaxGain);
		alSourcef(emitterID, AL_CONE_OUTER_GAIN, emitterCmp->ConeOuterGain);
		alSourcef(emitterID, AL_CONE_INNER_ANGLE, emitterCmp->ConeInnerAngle);
		alSourcef(emitterID, AL_CONE_OUTER_ANGLE, emitterCmp->ConeOuterAngle);
		alSourcef(emitterID, AL_CONE_OUTER_GAIN, emitterCmp->Direction);

		Vector pos = transCmp->GetGlobalTranslation();
		alSource3f(emitterID, AL_POSITION, pos.X, pos.Y, pos.Z);

		if (emitterCmp->Looping)
			alSourcei(emitterID, AL_LOOPING, AL_TRUE);
		else
			alSourcei(emitterID, AL_LOOPING, AL_FALSE);

		if (emitterCmp->Paused)
			alSourcei(emitterID, AL_SOURCE_STATE, AL_PAUSED);
		else
			alSourcei(emitterID, AL_SOURCE_STATE, AL_PLAYING);

		// set velocity
	}
}

//---------------------------------------------------------------------------------------------------
void SOUND_DEVICE_DLLEXPORT ALSoundDevice::SetDevice(const String& device)
{
	alcCloseDevice(Device);
	Device = alcOpenDevice(device.GetCStr());
	HEAVY_ASSERTE(Device, "OpenAL device creation failed");
}

//---------------------------------------------------------------------------------------------------
String SOUND_DEVICE_DLLEXPORT ALSoundDevice::GetCurrentDevice()
{
	return String(alcGetString(Device, ALC_DEVICE_SPECIFIER));
}

//---------------------------------------------------------------------------------------------------
Dynarray<String> SOUND_DEVICE_DLLEXPORT ALSoundDevice::GetAvailableDevices()
{
	Dynarray<String> availableDevices;

	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const ALCchar* devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

		int i = 0;
		while (true)
		{
			availableDevices.PushFront(String(&devices[i]));

			while (devices[i] != 0) ++i;
			if (devices[i] == 0 && devices[i + 1] == 0) break;
			++i;
		}
	}
	else
	{
		availableDevices.PushFront(String(alcGetString(Device, ALC_DEVICE_SPECIFIER)));
	}

	return availableDevices;
}

//------------------------------------------------------------------------------
Poly::ISoundDevice* PolyCreateSoundDevice()
{
	return new ALSoundDevice();
}
