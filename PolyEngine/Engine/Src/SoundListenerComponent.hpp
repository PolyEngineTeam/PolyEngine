#pragma once

#include "ComponentBase.hpp"

namespace Poly
{
	class ISoundListenerData;

	class ENGINE_DLLEXPORT SoundListenerComponent : public ComponentBase
	{
	public:
		SoundListenerComponent();
		~SoundListenerComponent();

		float Gain = 1.0f;

		ISoundListenerData* SoundListenerData = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SoundListenerComponent)

} // namespace Poly