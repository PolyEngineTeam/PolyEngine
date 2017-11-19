#pragma once

#include "ComponentBase.hpp"
#include "ResourceManager.hpp"

namespace Poly
{
	enum class eResourceSource;
	class ISoundBufferProxy;
	class ISoundEmitterDataProxy;

	class ENGINE_DLLEXPORT SoundEmitterComponent : public ComponentBase
	{
	public:
		SoundEmitterComponent();
		~SoundEmitterComponent();

		float Pitch = 1.0f;
		float Gain = 1.0f;
		float MaxDistance = 100.0f;
		float RolloffFactor = 1.0f;
		float RefferenceDistance = 100.0f;
		float MinGain = 1.0f;
		float MaxGain = 1.0f;
		float ConeInnerAngle = 360.0f;
		float ConeOuterAngle = 360.0f;
		float ConeOuterGain = 1.0f;
		float SecondsOffset = 0.0f;
		bool Looping = false;
		bool Paused = false;
		bool Active = true;
		bool StateChanged = true;

		ISoundEmitterDataProxy* SoundEmitterData = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SoundEmitterComponent)

} // namespace Poly