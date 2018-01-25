#pragma once

namespace Poly
{
	class World;
	class EntityTransform;

	namespace MovementSystem
	{
		void MovementUpdatePhase(World* world);

		Vector ENGINE_DLLEXPORT GetLocalForward(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetLocalRight(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetLocalUp(const EntityTransform& transform);

		Vector ENGINE_DLLEXPORT GetGlobalForward(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetGlobalRight(const EntityTransform& transform);
		Vector ENGINE_DLLEXPORT GetGlobalUp(const EntityTransform& transform);
	}
}