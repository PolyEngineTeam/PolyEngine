#pragma once

namespace Poly
{
	class World;
	class TransformComponent;

	namespace MovementSystem
	{
		void MovementUpdatePhase(World* world);

		Vector GetLocalForward(const TransformComponent* transform);
		Vector GetLocalRight(const TransformComponent* transform);
		Vector GetLocalUp(const TransformComponent* transform);

		Vector GetGlobalForward(const TransformComponent* transform);
		Vector GetGlobalRight(const TransformComponent* transform);
		Vector GetGlobalUp(const TransformComponent* transform);
	}
}