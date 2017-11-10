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
} // namespace Poly

extern "C"
{
	SOUND_DEVICE_DLLEXPORT Poly::ISoundDevice* __stdcall PolyCreateSoundDevice();
}