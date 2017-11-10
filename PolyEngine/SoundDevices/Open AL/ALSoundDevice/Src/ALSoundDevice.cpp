#include "ALSoundDevice.hpp"

// third party
#include <al.h>

// engine
#include <World.hpp>
#include <TransformComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <DeferredTaskSystem.hpp>

#include <SoundListenerComponent.hpp>
#include <SoundEmitterComponent.hpp>
#include <SoundResource.hpp>

// sound device
#include "SoundDataComponent.hpp"

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

	for (auto& emitterTuple : world->IterateComponents<SoundEmitterComponent>())
	{
		SoundEmitterComponent* emitterCmp = std::get<SoundEmitterComponent*>(emitterTuple);

		// create new sound emitter helpers
		if (emitterCmp->GetFlags().IsSet(eComponentBaseFlags::NEWLY_CREATED))
		{
			unsigned int emitterID;
			alGenSources(1, &emitterID);
			DeferredTaskSystem::AddComponent<SoundDataComponent>(world, emitterCmp->GetOwnerID(), emitterID);
			continue;
		}

		SoundDataComponent* dataHolder = emitterCmp->GetSibling<SoundDataComponent>();

		// delete sound emitter helpers
		if (emitterCmp->GetFlags().IsSet(eComponentBaseFlags::ABOUT_TO_BE_REMOVED))
		{
			alDeleteSources(1, &dataHolder->EmitterID);
			DeferredTaskSystem::RemoveComponent<SoundDataComponent>(world, emitterCmp->GetOwnerID());
			continue;
		}

		unsigned int emitterID = dataHolder->EmitterID;

		// get queued buffers count
		int queuedBuffersCount = 2;
		alGetSourcei(emitterID, AL_BUFFERS_QUEUED, &queuedBuffersCount);

		if (emitterCmp->PlaylistChanged)
		{
			// set all buffers as processed
			alSourceStop(emitterID);
			// unqueue all processed buffers
			alSourceUnqueueBuffers(emitterID, (int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());
			// delete all buffers queued in source
			alDeleteBuffers((int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());

			// establish immediate buffer count
			queuedBuffersCount = emitterCmp->GetBufferCount() >= dataHolder->MaxBuffersInQueue ? (int)dataHolder->MaxBuffersInQueue : (int)emitterCmp->GetBufferCount();
			// create new al buffers
			dataHolder->QueuedBuffers.Resize(queuedBuffersCount);
			alGenBuffers(queuedBuffersCount, dataHolder->QueuedBuffers.GetData());

			// copy certain number of queued buffers into immediate buffers
			for (int i = 0; i < queuedBuffersCount; i++)
			{
				const SoundResource* res = emitterCmp->GetBuffer(i);

				// fill immediate buffer with data
				//alBufferData(dataHolder->QueuedBuffers[i], FormatMap[res->GetSampleFormat()], res->GetRawData()->GetData(), res->GetRawData()->GetSize(), res->GetFrequency());
			}
		}
		else while ((unsigned int)queuedBuffersCount < dataHolder->MaxBuffersInQueue && queuedBuffersCount < emitterCmp->GetBufferCount())
		{
			// remove resource from emitter
			emitterCmp->PopSoundResource();
			// remove resource fromom OpenAL source
			alSourceUnqueueBuffers(emitterID, 1, dataHolder->QueuedBuffers.Begin().operator->());
			alDeleteBuffers(1, dataHolder->QueuedBuffers.Begin().operator->());
			// remove resource from sound data component
			dataHolder->QueuedBuffers.PopFront();

			// get next buffer from emitter
			const SoundResource* res = emitterCmp->GetBuffer(queuedBuffersCount);
			// create new OpenAL buffer
			unsigned int bufferID;
			alGenBuffers(1, &bufferID);
			// fill buffer with data
			//alBufferData(bufferID, FormatMap[res->GetSampleFormat()], res->GetRawData()->GetData(), res->GetRawData()->GetSize(), res->GetFrequency());
			// add buffer to sound data component
			dataHolder->QueuedBuffers.PushBack(bufferID);

			queuedBuffersCount++;
		}

		emitterCmp->PlaylistChanged = false;

		alSourcef(emitterID, AL_PITCH, emitterCmp->Pitch);
		alSourcef(emitterID, AL_GAIN, emitterCmp->Gain);
		alSourcef(emitterID, AL_MAX_DISTANCE, emitterCmp->MaxDistance);
		alSourcef(emitterID, AL_ROLLOFF_FACTOR, emitterCmp->RolloffFactor);
		alSourcef(emitterID, AL_REFERENCE_DISTANCE, emitterCmp->RefferenceDistance);
		alSourcef(emitterID, AL_MIN_GAIN, emitterCmp->MinGain);
		alSourcef(emitterID, AL_MAX_GAIN, emitterCmp->MaxGain);
		alSourcef(emitterID, AL_CONE_INNER_ANGLE, emitterCmp->ConeInnerAngle);
		alSourcef(emitterID, AL_CONE_OUTER_ANGLE, emitterCmp->ConeOuterAngle);
		alSourcef(emitterID, AL_CONE_OUTER_GAIN, emitterCmp->ConeOuterGain);

		// set position
		TransformComponent* transCmp = emitterCmp->GetSibling<TransformComponent>();
		Vector pos = transCmp->GetGlobalTranslation();
		alSource3f(emitterID, AL_POSITION, pos.X, pos.Y, pos.Z);

		// set direction
		// set velocity

		if (emitterCmp->Looping)
			alSourcei(emitterID, AL_LOOPING, AL_TRUE);
		else
			alSourcei(emitterID, AL_LOOPING, AL_FALSE);

		if (emitterCmp->Paused)
			alSourcei(emitterID, AL_SOURCE_STATE, AL_PAUSED);
		else
			alSourcei(emitterID, AL_SOURCE_STATE, AL_PLAYING);

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
