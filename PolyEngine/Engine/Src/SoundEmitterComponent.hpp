#pragma once

#include "ComponentBase.hpp"

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
		SoundEmitterComponent();
		~SoundEmitterComponent();

		float Pitch;
		float Gain;
		float MaxDistance;
		float RolloffFactor;
		float RefferenceDistance;
		float MinGain;
		float MaxGain;
		float ConeOuterGain;
		float ConeInnerAngle;
		float ConeOuterAngle;
		float Direction;
		float SecondsOffset;
		bool Looping;
		bool Paused;
		Queue<SoundResource*> Playlist;
	};

} // namespace Poly