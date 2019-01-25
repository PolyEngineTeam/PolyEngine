#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT ActiveSequenceComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::ActiveSequenceComponent	)
		{
			NO_RTTI_PROPERTY();
		}
	}; // class ActiveSequenceComponent
} // namespace Poly