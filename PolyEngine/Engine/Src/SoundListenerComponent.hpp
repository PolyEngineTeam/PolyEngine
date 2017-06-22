#pragma once

#include "ComponentBase.hpp"

typedef float ALfloat;

namespace Poly
{
	class SoundWorldComponent;

	/// Contains information about listener.
	/// There can be any number of listeners but every listener has its own copy of every emitter (except of resources)
	/// hence 10 listeners uses 10 times more CPU and memory than 1 listener.
	/// @see SoundEmitterComponent
	class ENGINE_DLLEXPORT SoundListenerComponent : public ComponentBase
	{
	public:
		/// Sets everything to 0 and copies SourcesArray from SoundWoeldComponent
		SoundListenerComponent(SoundWorldComponent* worldComponent);
		~SoundListenerComponent();

	private:
		ALfloat Gain;
		ALfloat Position[3];
		ALfloat Velocity[3];
		ALfloat Orientation[3];
	};

} // namespace Poly