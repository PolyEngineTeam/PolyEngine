#pragma once

#include <BaseObject.hpp>

typedef struct ALCdevice_struct ALCdevice;

namespace Poly {
	class SoundWorldComponent;

	/// Class containing device used to render sound.
	/// Device is available to change by SoundSystem functions but there can be only one active device.
	class ENGINE_DLLEXPORT OpenALRenderingDevice : public BaseObject<>  
	{
	friend class SoundWorldComponent;
	public:
		/// Creates and saves default device.
		OpenALRenderingDevice();

		/// Releases currently stored device
		~OpenALRenderingDevice();

		const ALCdevice* GetDevice() const { return Device; }

	private:
		ALCdevice* Device;
	};

} // namespace Poly