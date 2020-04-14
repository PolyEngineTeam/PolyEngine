#pragma once

#include <pe/Defines.hpp>

#include <pe/api/ecs/ISystem.hpp>

namespace pe::api::ecs
{
	class Scene;
	class EntityTransform;
}

namespace pe::engine::movement
{
	class ENGINE_DLLEXPORT MovementSystem : public api::ecs::ISystem
	{
		void onUpdate(api::ecs::Scene* scene) override;
	};
}