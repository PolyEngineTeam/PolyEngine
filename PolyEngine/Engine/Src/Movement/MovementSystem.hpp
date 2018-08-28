#pragma once

#include <Defines.hpp>

namespace Poly
{
	class Scene;
	class EntityTransform;

	namespace MovementSystem
	{
		void MovementUpdatePhase(Scene* world);

		Vector ENGINE_DLLEXPORT GetLocalForward(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetLocalRight(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetLocalUp(const EntityTransform& transform);

		Vector ENGINE_DLLEXPORT GetGlobalForward(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetGlobalRight(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetGlobalUp(const EntityTransform& transform);
	}
}