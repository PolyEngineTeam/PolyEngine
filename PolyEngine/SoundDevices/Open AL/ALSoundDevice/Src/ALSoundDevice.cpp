#include "ALSoundDevice.hpp"

// third party

// engine
#include <World.hpp>
#include <TransformComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <DeferredTaskSystem.hpp>

#include <SoundListenerComponent.hpp>
#include <SoundEmitterComponent.hpp>
#include <SoundResource.hpp>

// sound device
#include "SoundDataHolder.hpp"

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

	if (Device) {
		Context = alcCreateContext(Device, nullptr);
		alcMakeContextCurrent(Context);
	}
	else
		throw new ALSoundDeviceInitializationException();
}

//---------------------------------------------------------------------------------------------------
void SOUND_DEVICE_DLLEXPORT ALSoundDevice::Close()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
}

//---------------------------------------------------------------------------------------------------
void SOUND_DEVICE_DLLEXPORT ALSoundDevice::RenderWorld(World* world)
{
	for (auto& emitterTuple : world->IterateComponents<SoundEmitterComponent>())
	{
		alGetError();
		SoundEmitterComponent* emitterCmp = std::get<SoundEmitterComponent*>(emitterTuple);

		// create new sound emitter helpers
		if (emitterCmp->GetFlags().IsSet(eComponentBaseFlags::NEWLY_CREATED))
		{
			unsigned int emitterID;
			alGenSources(1, &emitterID);
			emitterCmp->DataHolder = new SoundDataHolder(emitterID);
		}
		
		// delete sound emitter helpers
		if (emitterCmp->GetFlags().IsSet(eComponentBaseFlags::ABOUT_TO_BE_REMOVED))
		{
			alDeleteSources(1, &reinterpret_cast<SoundDataHolder*>(emitterCmp->DataHolder)->EmitterID);
			delete emitterCmp->DataHolder;
			continue;
		}
		
		SoundDataHolder* dataHolder = reinterpret_cast<SoundDataHolder*>(emitterCmp->DataHolder);
		unsigned int emitterID = reinterpret_cast<SoundDataHolder*>(emitterCmp->DataHolder)->EmitterID;
		
		// get queued buffers count
		int queuedBuffersCount;
		int processedBuffersCount;
		alGetSourcei(emitterID, AL_BUFFERS_QUEUED, &queuedBuffersCount);
		alGetSourcei(emitterID, AL_BUFFERS_PROCESSED, &processedBuffersCount);
		
		if (emitterCmp->StateChanged) 
		{
			emitterCmp->StateChanged = false;
		
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
			if (transCmp)
			{
				Vector pos = transCmp->GetGlobalTranslation();
				alSource3f(emitterID, AL_POSITION, pos.X, pos.Y, pos.Z);
			}
		
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

		if (emitterCmp->Looping && queuedBuffersCount - processedBuffersCount > 0)
			continue;
		else if (emitterCmp->PlaylistChanged)
		{
			emitterCmp->PlaylistChanged = false;

			// set all buffers as processed
			alSourceStop(emitterID);
			// unqueue all processed buffers
			alSourceUnqueueBuffers(emitterID, (int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());
			// delete all buffers queued in source
			alDeleteBuffers((int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());
			
			// establish OpenAL buffer count
			queuedBuffersCount = emitterCmp->GetBufferCount() >= dataHolder->MaxBuffersInQueue ? (int)dataHolder->MaxBuffersInQueue : (int)emitterCmp->GetBufferCount();

			if (queuedBuffersCount == 0)
				continue;

			// enlarge bufferID collection if needed
			if (dataHolder->QueuedBuffers.GetCapacity() < queuedBuffersCount)
				dataHolder->QueuedBuffers.Reserve(queuedBuffersCount);

			// create new al buffers
			alGenBuffers(queuedBuffersCount, dataHolder->QueuedBuffers.GetData());
			
			// copy certain number of queued buffers into immediate buffers
			for (int i = 0; i < queuedBuffersCount; i++)
			{
				const SoundResource* res = emitterCmp->GetBuffer(i);
			
				// fill OpenAL buffer with data
				unsigned int* buffersBegin = dataHolder->QueuedBuffers.GetData();
				alBufferData(buffersBegin[i], FormatMap[res->GetSampleFormat()], res->GetRawData()->GetData(), (int)res->GetRawData()->GetSize(), (int)res->GetFrequency());
			}

			// queue buffers
			alSourceQueueBuffers(emitterID, queuedBuffersCount, dataHolder->QueuedBuffers.GetData());

			alSourcePlay(emitterID);
		}
		//else if (queuedBuffersCount - processedBuffersCount <= dataHolder->MaxBuffersInQueue)
		//{
		//
		//}

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
ISoundDevice* __stdcall PolyCreateSoundDevice()
{
	return new ALSoundDevice();
}
