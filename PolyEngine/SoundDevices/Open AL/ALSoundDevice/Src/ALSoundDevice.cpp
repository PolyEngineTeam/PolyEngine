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

		if ((emitterCmp->PlaylistChanged || queuedBuffersCount == processedBuffersCount) && emitterCmp->GetBufferCount() > 0)
		{
			// set all buffers as processed
			alSourceStop(emitterID);
			// unqueue all processed buffers
			alSourceUnqueueBuffers(emitterID, (int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());
			// delete all buffers queued in source
			alDeleteBuffers((int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());
			
			// establish immediate buffer count
			queuedBuffersCount = 1; // emitterCmp->GetBufferCount() >= dataHolder->MaxBuffersInQueue ? (int)dataHolder->MaxBuffersInQueue : (int)emitterCmp->GetBufferCount();
			// resize if needed
			if (queuedBuffersCount != dataHolder->QueuedBuffers.GetCapacity())
				dataHolder->QueuedBuffers.Resize(queuedBuffersCount);
			// create new al buffers
			alGenBuffers(queuedBuffersCount, dataHolder->QueuedBuffers.GetData());

			
			// copy certain number of queued buffers into immediate buffers
			const SoundResource* res = emitterCmp->GetBuffer(0);
			// fill OpenAL buffer with data
			alBufferData(dataHolder->QueuedBuffers[0], FormatMap[res->GetSampleFormat()], res->GetRawData()->GetData(), (int)res->GetRawData()->GetSize(), (int)res->GetFrequency());

			// attach buffer to source
			alSourcei(emitterID, AL_BUFFER, dataHolder->QueuedBuffers[0]);

			//if (emitterCmp->Paused)
			//	alSourcei(emitterID, AL_SOURCE_STATE, AL_PAUSED);
			///else
			alSourcePlay(emitterID);

			emitterCmp->PlaylistChanged = false;
		}
		
		//if (emitterCmp->PlaylistChanged)
		//{
		//	// set all buffers as processed
		//	alSourceStop(emitterID);
		//	// unqueue all processed buffers
		//	alSourceUnqueueBuffers(emitterID, (int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());
		//	// delete all buffers queued in source
		//	alDeleteBuffers((int)dataHolder->QueuedBuffers.GetSize(), dataHolder->QueuedBuffers.GetData());
		//
		//	// establish immediate buffer count
		//	queuedBuffersCount = emitterCmp->GetBufferCount() >= dataHolder->MaxBuffersInQueue ? (int)dataHolder->MaxBuffersInQueue : (int)emitterCmp->GetBufferCount();
		//	// create new al buffers
		//	dataHolder->QueuedBuffers.Resize(queuedBuffersCount);
		//	alGenBuffers(queuedBuffersCount, dataHolder->QueuedBuffers.GetData());
		//
		//	// copy certain number of queued buffers into immediate buffers
		//	for (int i = 0; i < queuedBuffersCount; i++)
		//	{
		//		const SoundResource* res = emitterCmp->GetBuffer(i);
		//
		//		// fill immediate buffer with data
		//		alBufferData(dataHolder->QueuedBuffers[i], FormatMap[res->GetSampleFormat()], res->GetRawData()->GetData(), (int)res->GetRawData()->GetSize(), (int)res->GetFrequency());
		//	}
		//}
		//else while ((unsigned int)queuedBuffersCount < dataHolder->MaxBuffersInQueue && queuedBuffersCount < emitterCmp->GetBufferCount())
		//{
		//	// remove resource from emitter
		//	emitterCmp->PopSoundResource();
		//	// remove resource fromom OpenAL source
		//	alSourceUnqueueBuffers(emitterID, 1, dataHolder->QueuedBuffers.Begin().operator->());
		//	alDeleteBuffers(1, dataHolder->QueuedBuffers.Begin().operator->());
		//	// remove resource from sound data component
		//	dataHolder->QueuedBuffers.PopFront();
		//
		//	// get next buffer from emitter
		//	const SoundResource* res = emitterCmp->GetBuffer(queuedBuffersCount);
		//	// create new OpenAL buffer
		//	unsigned int bufferID;
		//	alGenBuffers(1, &bufferID);
		//	// fill buffer with data
		//	alBufferData(bufferID, FormatMap[res->GetSampleFormat()], res->GetRawData()->GetData(), (int)res->GetRawData()->GetSize(), (int)res->GetFrequency());
		//	// add buffer to sound data component
		//	dataHolder->QueuedBuffers.PushBack(bufferID);
		//
		//	queuedBuffersCount++;
		//}
		
		if (emitterCmp->StateChanged) 
		{
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
