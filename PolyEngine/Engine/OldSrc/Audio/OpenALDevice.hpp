#pragma once

#include <pe/Defines.hpp>
#include <Audio/SoundSystem.hpp>

typedef struct ALCdevice_struct ALCdevice;

namespace Poly {
	class SoundWorldComponent;

	/// Class containing device used to render sound.
	/// Device is available to change by SoundSystem functions but there can be only one active device.
	class ENGINE_DLLEXPORT OpenALDevice : public ::pe::core::BaseObject<>  
	{
	friend class SoundWorldComponent;
	friend void SoundSystem::SetDevice(const ::pe::core::storage::String&);
	friend const ::pe::core::storage::String& SoundSystem::GetDeviceName();
	friend const std::vector<::pe::core::storage::String>& SoundSystem::GetAvailableDevices();
	public:
		/// Creates and saves default device.
		/// Also loads available device names for device creation.
		OpenALDevice();

		/// Releases currently stored device
		~OpenALDevice();

	private:
		ALCdevice* Device;
		std::vector<::pe::core::storage::String> AvailableDevices;
	};

} // namespace Poly