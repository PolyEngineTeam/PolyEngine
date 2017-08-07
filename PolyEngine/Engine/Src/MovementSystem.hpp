#pragma once

namespace Poly
{
	class World;
	class TransformComponent;

	namespace MovementSystem
	{
		void MovementUpdatePhase(World* world);

		Vector ENGINE_DLLEXPORT GetLocalForward(const TransformComponent* transform);
		Vector ENGINE_DLLEXPORT GetLocalRight(const TransformComponent* transform);
		Vector ENGINE_DLLEXPORT GetLocalUp(const TransformComponent* transform);

		Vector ENGINE_DLLEXPORT GetGlobalForward(const TransformComponent* transform);
		Vector ENGINE_DLLEXPORT GetGlobalRight(const TransformComponent* transform);
		Vector ENGINE_DLLEXPORT GetGlobalUp(const TransformComponent* transform);
	}
}