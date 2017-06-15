#pragma once

#include "ComponentBase.hpp"
#include "SoundSystem.hpp"

typedef float ALfloat;

namespace Poly
{
	class ENGINE_DLLEXPORT SoundListenerComponent : public ComponentBase
	{
	public:
		ALfloat GetGain() const { return Gain; }
		ALfloat* GetPosition() { return Position; }
		ALfloat* GetVelocity() { return Velocity; }
		ALfloat* GetOrientation() { return Orientation; }
		
	private:
		ALfloat Gain;
		ALfloat Position[3];
		ALfloat Velocity[3];
		ALfloat Orientation[3];
	};
}