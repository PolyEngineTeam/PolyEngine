#pragma once

#include "ComponentBase.hpp"
#include "SoundSystem.hpp"

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
	friend void SoundSystem::SoundPhase(World* world);
	public:
		/// Loads resource from given path (optimized by resource manager).
		/// @param path path to sound resource
		/// @see SoundListenerComponent
		SoundEmitterComponent(const String& path, eResourceSource source, bool background = false);

		/// Releases resource (optimized by resource manager).
		~SoundEmitterComponent();

		float Pitch;
		float Gain;
		float MaxDistance;
		float RolloffFactor;
		float RefferenceDistance;
		float MinGain;
		float MaxGain;
		float ConeOuterGain;
		Vector ConeInnerAngle;
		Vector ConeOuterAngle;
		Vector Direction;
		bool Looping;
		Queue<SoundResource*> Playlist;
		bool Paused;
		float SecondsOffset;
	};

} // namespace Poly