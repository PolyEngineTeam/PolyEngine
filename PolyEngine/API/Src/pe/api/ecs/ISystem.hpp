#pragma once

#include <pe/Defines.hpp>

namespace pe::api::ecs
{
	class Scene;
	class ISystem : public ::pe::core::BaseObject<>
	{
	public:
		virtual void OnUpdate(Scene* scene) = 0;
	};
}