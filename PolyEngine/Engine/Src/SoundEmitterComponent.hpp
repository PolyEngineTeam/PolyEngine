#pragma once

#include "ComponentBase.hpp"

typedef unsigned int ALuint;

namespace Poly
{
	class SoundResource;

	/// Class representing sound source.
	/// Sound source is located where enititys TransformComponent points.
	/// Emitters by default have enabled Doppler effect and default distance model.
	/// If you wish you can change default distance model on your own or particularly for one of emitters.
	/// @see SoundListenerComponent
	/// @see SoundSystem 
	class ENGINE_DLLEXPORT SoundEmitterComponent : public ComponentBase
	{
	public:
		/// Loads resource from given path (optimized by resource manager).
		/// @param path - path to sound resource
		/// @see SoundListenerComponent
		SoundEmitterComponent(const String& path);

		/// Releases resource (optimized by resource manager).
		~SoundEmitterComponent();

		const ALuint GetEmitterID() const { return EmitterID; }

	private:
		ALuint EmitterID;
		SoundResource* Resource;
	};

} // namespace Poly