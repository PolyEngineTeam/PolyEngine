#include "ALSoundDevice.hpp"

// engine
#include <World.hpp>
#include <TransformComponent.hpp>

#include <SoundResource.hpp>
#include <SoundEmitterComponent.hpp>
#include <SoundListenerComponent.hpp>

// sound device
#include "SoundBuffer.hpp"
#include "SoundEmitterData.hpp"
#include "SoundListenerData.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
ALSoundDevice::ALSoundDevice()
{
}

//------------------------------------------------------------------------------
ALSoundDevice::~ALSoundDevice()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
}

//---------------------------------------------------------------------------------------------------
void ALSoundDevice::Init()
{
	Device = alcOpenDevice(nullptr);

	if (Device) 
	{
		Context = alcCreateContext(Device, nullptr);
		alcMakeContextCurrent(Context);
	}
	else
		throw new ALSoundDeviceInitializationException();
}

//---------------------------------------------------------------------------------------------------
void ALSoundDevice::RenderWorld(World* world)
{
	for (auto& emitterTuple : world->IterateComponents<SoundEmitterComponent>())
	{
		alGetError();
		SoundEmitterComponent* emitterCmp = std::get<SoundEmitterComponent*>(emitterTuple);
		
		// delete sound emitter helpers
		if (emitterCmp->GetFlags().IsSet(eComponentBaseFlags::ABOUT_TO_BE_REMOVED))
		{
			alDeleteSources(1, &reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID);
			delete emitterCmp->SoundEmitterData;
			continue;
		}
		
		SoundEmitterData* emitterData = reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData);
		unsigned int emitterID = emitterData->EmitterID;
		
		int processedBuffersCount;
		alGetSourcei(reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID, AL_BUFFERS_PROCESSED, &processedBuffersCount);

		if (processedBuffersCount == emitterData->CurrentPlaylist.GetSize() && !emitterCmp->Looping)
		{
			emitterCmp->Active = false;
			emitterCmp->StateChanged = true;
		}

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

			if (!emitterCmp->Active)
				emitterCmp->Paused = true;
		
			if (emitterCmp->Paused)
				alSourcePause(emitterID);
			else
				alSourcePlay(emitterID);
		}
	}
}

//---------------------------------------------------------------------------------------------------
void ALSoundDevice::SetDevice(const String& device)
{
	alcCloseDevice(Device);
	Device = alcOpenDevice(device.GetCStr());
	HEAVY_ASSERTE(Device, "OpenAL device creation failed");
}

//---------------------------------------------------------------------------------------------------
String ALSoundDevice::GetCurrentDevice()
{
	return String(alcGetString(Device, ALC_DEVICE_SPECIFIER));
}

//---------------------------------------------------------------------------------------------------
Dynarray<String> ALSoundDevice::GetAvailableDevices()
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

//---------------------------------------------------------------------------------------------------
void ALSoundDevice::PushSoundResource(SoundEmitterComponent* emitterCmp, const SoundResource& src)
{
	if (emitterCmp->SoundEmitterData == nullptr)
	{
		emitterCmp->SoundEmitterData = new SoundEmitterData;
		alGenSources(1, &reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID);
	}

	SoundBuffer* buffer = new SoundBuffer();

	alGenBuffers(1, &buffer->BufferID);
	alBufferData(buffer->BufferID, FormatMap[src.GetSampleFormat()], src.GetRawData()->GetData(), (int)src.GetRawData()->GetSize(), (int)src.GetFrequency());

	alSourceQueueBuffers(reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID, 1, &buffer->BufferID);

	emitterCmp->StateChanged = true;

	reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->CurrentPlaylist.PushBack(buffer->BufferID);
}

//---------------------------------------------------------------------------------------------------
void ALSoundDevice::PopSoundResource(SoundEmitterComponent* emitterCmp)
{
	if (emitterCmp->SoundEmitterData == nullptr)
	{
		emitterCmp->SoundEmitterData = new SoundEmitterData;
		alGenSources(1, &reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID);
	}

	int processedBuffersCount;
	alGetSourcei(reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID, AL_BUFFERS_PROCESSED, &processedBuffersCount);

	if (processedBuffersCount > 0)
	{
		alSourceUnqueueBuffers(reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID, 1, &reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->CurrentPlaylist[0]);
		reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->CurrentPlaylist.PopFront();
	}
}

//---------------------------------------------------------------------------------------------------
int ALSoundDevice::GetProcessedBuffersCount(SoundEmitterComponent* emitterCmp)
{
	if (emitterCmp->SoundEmitterData == nullptr)
	{
		emitterCmp->SoundEmitterData = new SoundEmitterData;
		alGenSources(1, &reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID);
	}

	int processedBuffersCount;
	alGetSourcei(reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID, AL_BUFFERS_PROCESSED, &processedBuffersCount);
	return processedBuffersCount;
}

//---------------------------------------------------------------------------------------------------
int ALSoundDevice::GetQueuedBuffersCount(SoundEmitterComponent* emitterCmp)
{
	if (emitterCmp->SoundEmitterData == nullptr)
	{
		emitterCmp->SoundEmitterData = new SoundEmitterData;
		alGenSources(1, &reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID);
	}

	int queuedBuffersCount;
	alGetSourcei(reinterpret_cast<SoundEmitterData*>(emitterCmp->SoundEmitterData)->EmitterID, AL_BUFFERS_QUEUED, &queuedBuffersCount);
	return queuedBuffersCount;
}

//------------------------------------------------------------------------------
ISoundDevice* __stdcall PolyCreateSoundDevice()
{
	return new ALSoundDevice();
}
