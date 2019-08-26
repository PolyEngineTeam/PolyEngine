#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT BoneComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::BoneComponent) { NO_RTTI_PROPERTY(); }

		BoneComponent();
	};

	REGISTER_COMPONENT(ComponentsIDGroup, BoneComponent)
}