#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT SequenceComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::SequenceComponent)
		{ 
			NO_RTTI_PROPERTY(); 
		}

	private:

	}; // class SequenceComponent
} // namespace Poly