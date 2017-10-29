#pragma once

#include <alc.h>
#include <unordered_map>

#include <ISoundDevice.hpp>
#include <UniqueID.hpp>
#include <Dynarray.hpp>

namespace Poly
{
	class SOUND_DEVICE_DLLEXPORT ALSoundDevice : public ISoundDevice
	{
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

		std::unordered_map<UniqueID, unsigned int> OwnerToEmitterMap;
	};
} // namespace Poly

extern "C"
{
	SOUND_DEVICE_DLLEXPORT Poly::ISoundDevice* __stdcall PolyCreateSoundDevice();
}