#pragma once

#include <pe/Defines.hpp>

namespace pe::api::ecs
{
	class Scene;
	class EntityTransform;
}

namespace pe::engine::movement
{
	namespace MovementSystem
	{
		void MovementUpdatePhase(api::ecs::Scene* world);

		::pe::core::math::Vector ENGINE_DLLEXPORT GetLocalForward(const api::ecs::EntityTransform& transform);
		::pe::core::math::Vector ENGINE_DLLEXPORT GetLocalRight(const api::ecs::EntityTransform& transform);
		::pe::core::math::Vector ENGINE_DLLEXPORT GetLocalUp(const api::ecs::EntityTransform& transform);

		::pe::core::math::Vector ENGINE_DLLEXPORT GetGlobalForward(const api::ecs::EntityTransform& transform);
		::pe::core::math::Vector ENGINE_DLLEXPORT GetGlobalRight(const api::ecs::EntityTransform& transform);
		::pe::core::math::Vector ENGINE_DLLEXPORT GetGlobalUp(const api::ecs::EntityTransform& transform);
	}
}