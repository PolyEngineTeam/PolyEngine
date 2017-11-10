#pragma once

#include "ComponentBase.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT SoundListenerComponent : public ComponentBase
	{
	public:
		SoundListenerComponent();
		~SoundListenerComponent();

		float Gain = 1.0f;
	};

} // namespace Poly