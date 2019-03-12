#pragma once

#include <Defines.hpp>
#include <ECS/Scene.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT DeferredTaskBase : public BaseObject<>
	{
	public:
		virtual void Execute(Scene* w) = 0;
		virtual const char* GetDescription() const = 0;
	};
}
