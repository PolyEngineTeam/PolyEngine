#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/Scene.hpp>

namespace pe::api::ecs
{
	class ENGINE_DLLEXPORT DeferredTaskBase : public ::pe::core::BaseObject<>
	{
	public:
		virtual void Execute(Scene* w) = 0;
		virtual const char* GetDescription() const = 0;
	};
}
