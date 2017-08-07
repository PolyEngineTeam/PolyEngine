#pragma once

#include <BaseObject.hpp>

#include "SoundSystem.hpp"

typedef struct ALCdevice_struct ALCdevice;

namespace Poly {
	class SoundWorldComponent;

	/// Class containing device used to render sound.
	/// Device is available to change by SoundSystem functions but there can be only one active device.
	class ENGINE_DLLEXPORT OpenALDevice : public BaseObject<>  
	{
	friend class SoundWorldComponent;
	friend void SoundSystem::SetDevice(const String&);
	friend const String& SoundSystem::GetDeviceName();
	friend const Dynarray<String>& SoundSystem::GetAvailableDevices();
	public:
		/// Creates and saves default device.
		/// Also loads available device names for device creation.
		OpenALDevice();

		/// Releases currently stored device
		~OpenALDevice();

	private:
		ALCdevice* Device;
		Dynarray<String> AvailableDevices;
	};

} // namespace Poly