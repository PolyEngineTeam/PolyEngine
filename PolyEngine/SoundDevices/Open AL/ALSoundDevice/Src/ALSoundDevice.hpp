#pragma once

#include <alc.h>

#include <ISoundDevice.hpp>
#include <UniqueID.hpp>
#include <Dynarray.hpp>

namespace Poly
{
	class SOUND_DEVICE_DLLEXPORT ALSoundDevice : ISoundDevice
	{
	public:
		void Init() override;
		void Close() override;
		void RenderWorld(World* world) override;

		void SetDevice(const String& device) override;
		const String& GetCurrentDevice() override;
		const Dynarray<String>& GetAvailableDevices() override;

	private:
		ALCdevice* Device;

		std::unordered_map<UniqueID, unsigned int> OwnerToEmitterMap;
	};
}