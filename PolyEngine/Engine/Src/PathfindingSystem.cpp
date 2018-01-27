#include "EnginePCH.hpp"

#include "PathfindingSystem.hpp"
#include "PathfindingComponent.hpp"

using namespace Poly;

Optional<Dynarray<Vector>> CalculateNewPath(const NavGrid* grid, const Vector& start, const Vector& dest, float agentRadius)
{
	Vector2f start2D = Vector2f(start.X, start.Z);
	Vector2f dest2D = Vector2f(dest.X, dest.Z);

	Queue<Vector2f> openList, closedList;


	//TODO implement
	return Optional<Dynarray<Vector>>();
}

void SmoothPath(Dynarray<Vector>& path)
{
	//TODO implement
}

ENGINE_DLLEXPORT void Poly::PathfindingSystem::UpdatePhase(World* world)
{
	for (auto& tuple : world->IterateComponents<PathfindingComponent>())
	{
		PathfindingComponent* pathfindingCmp = std::get<PathfindingComponent*>(tuple);

		if (pathfindingCmp->RecalculateRequested)
		{
			pathfindingCmp->RecalculateRequested = false;
			Optional<Dynarray<Vector>> path = CalculateNewPath(pathfindingCmp->NavigationGrid,
				pathfindingCmp->GetOwner()->GetTransform().GetGlobalTranslation(), pathfindingCmp->CurentDestination.Value(), pathfindingCmp->AgentRadius);

			if (!path.HasValue())
			{
				pathfindingCmp->LastPathSearchFailed = true;
				continue;
			}

			pathfindingCmp->CalculatedPath = std::move(path.TakeValue());
		}
	}
}
