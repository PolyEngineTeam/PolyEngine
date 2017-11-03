#pragma once

#include <alc.h>
#include <al.h>
#include <unordered_map>

#include <ISoundDevice.hpp>
#include <UniqueID.hpp>
#include <Dynarray.hpp>
#include <SoundResource.hpp>

namespace Poly
{
	struct AlEmitter;

	enum class eSoundSampleFormatAL
	{
		MONO8 = AL_FORMAT_MONO8,
		MONO16 = AL_FORMAT_MONO16,
		STEREO8 = AL_FORMAT_STEREO8,
		STEREO16 = AL_FORMAT_STEREO16,
		_COUNT = 4
	};

	class SOUND_DEVICE_DLLEXPORT ALSoundDevice : public ISoundDevice
	{
		friend struct ALEmitter;
	public:
		ALSoundDevice();
		~ALSoundDevice();

		void Init() override;
		void Close() override;
		void RenderWorld(World* world) override;

		void SetDevice(const String& device) override;
		String GetCurrentDevice() override;
		Dynarray<String> GetAvailableDevices() override;

	private:
		ALCdevice* Device;

		std::unordered_map<UniqueID, ALEmitter*> OwnerToEmitterMap;

		static const int BuffersInQueueCount = 2;
		std::unordered_map<eSoundSampleFormat, int>EngineFormatToALFormatMap;
	};

	struct ALEmitter
	{
		unsigned int EmitterID;
		unsigned int QueuedBuffers[ALSoundDevice::BuffersInQueueCount];
		unsigned int QueuedBuffersCount = 0;
	};
} // namespace Poly

extern "C"
{
	SOUND_DEVICE_DLLEXPORT Poly::ISoundDevice* __stdcall PolyCreateSoundDevice();
}