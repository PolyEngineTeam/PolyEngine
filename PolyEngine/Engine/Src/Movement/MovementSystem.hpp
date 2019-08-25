#pragma once

#include <pe/Defines.hpp>

namespace Poly
{
	class Scene;
	class EntityTransform;

	namespace MovementSystem
	{
		void MovementUpdatePhase(Scene* world);

		core::math::Vector ENGINE_DLLEXPORT GetLocalForward(const EntityTransform& transform);
		core::math::Vector ENGINE_DLLEXPORT GetLocalRight(const EntityTransform& transform);
		core::math::Vector ENGINE_DLLEXPORT GetLocalUp(const EntityTransform& transform);

		core::math::Vector ENGINE_DLLEXPORT GetGlobalForward(const EntityTransform& transform);
		core::math::Vector ENGINE_DLLEXPORT GetGlobalRight(const EntityTransform& transform);
		core::math::Vector ENGINE_DLLEXPORT GetGlobalUp(const EntityTransform& transform);
	}
}