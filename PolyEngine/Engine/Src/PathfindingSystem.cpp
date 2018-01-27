#include "EnginePCH.hpp"

#include "PathfindingSystem.hpp"
#include "PathfindingComponent.hpp"

using namespace Poly;

struct PathNode
{
	PathNode(const Vector2i& cell, float cost, float heurCost, i64 prevNode = -1)
		: Cell(cell), Cost(cost), HeuristicCost(heurCost), PrevNode(prevNode) {}

	Vector2i Cell;
	float Cost;
	float HeuristicCost;
	i64 PrevNode = -1;
	float TotalCost() const { return Cost + HeuristicCost; }
};

class Vector2iHash {
public:
	size_t operator()(const Vector2i& k) const {
		return k.X * 100 + k.Y; //TODO make this better
	}
};

Optional<Dynarray<Vector2f>> CalculateNewPath(const NavGrid* grid, const Vector2f& start, const Vector2f& dest, float agentRadius)
{
	// Swap start and dest to better handle path parsing later
	Vector2f startInternal = dest;
	Vector2f destInternal = start;

	if (!grid->IsPositionValid(startInternal, agentRadius) || !grid->IsPositionValid(destInternal, agentRadius))
		return Optional<Dynarray<Vector2f>>();

	Vector2i startInternalCell = grid->GetCellAtPosition(startInternal).Value();
	Vector2i destInternalCell = grid->GetCellAtPosition(destInternal).Value();

	Dynarray<PathNode> AllNodes;

	auto comparator = [&AllNodes](size_t a, size_t b) { return AllNodes[a].TotalCost() < AllNodes[b].TotalCost(); };
	auto heuristic = [destInternalCell](const Vector2i& pos) { return (destInternalCell.ToVector2f() - pos.ToVector2f()).Length(); };
	
	PriorityQueue<i64> openList(comparator), closedList(comparator);
	std::unordered_map<Vector2i, float, Vector2iHash> openListsMinCosts, closedListsMinCosts;

	AllNodes.PushBack(PathNode( startInternalCell, 0, heuristic(startInternalCell) ));
	openList.Push(AllNodes.GetSize() - 1);
	openListsMinCosts[startInternalCell] = 0;

	i64 bestNodeIdx = -1;
	while (openList.GetSize() > 0 && bestNodeIdx < 0)
	{
		i64 qIdx = openList.Pop();
		const PathNode& q = AllNodes[qIdx];

		Dynarray<Vector2i> neighbours = grid->GetNeighbours(q.Cell);
		for (const Vector2i& neighbour : neighbours)
		{
			// cannot walk here
			if (!grid->IsPositionValid(grid->GetCellMiddlePos(neighbour), agentRadius))
				continue;

			// we reached destination
			if (neighbour == destInternalCell)
			{
				bestNodeIdx = qIdx;
				break;
			}
				
			PathNode s{ neighbour, q.Cost + 1, heuristic(neighbour), qIdx };

			auto& it = openListsMinCosts.find(s.Cell);
			if (it != openListsMinCosts.end() && it->second < s.Cost)
				continue; // node has worse base cost than other (in the same pos) we visited before, skip it

			auto& it2 = closedListsMinCosts.find(s.Cell);
			if (it2 != closedListsMinCosts.end() && it2->second < s.Cost)
				continue; // node has worse base cost than other (in the same pos) we visited before, skip it


			AllNodes.PushBack(s);
			openList.Push(AllNodes.GetSize() - 1);
			openListsMinCosts[s.Cell] = s.Cost;
		}

		closedList.Push(qIdx);
		closedListsMinCosts[q.Cell] = q.Cost;
	}

	if (bestNodeIdx < 0)
		return Optional<Dynarray<Vector2f>>();

	Dynarray<Vector2f> path;
	path.PushBack(start);
	i64 currNodeIdx = bestNodeIdx;
	do
	{
		const PathNode& q = AllNodes[currNodeIdx];
		path.PushBack(grid->GetCellMiddlePos(q.Cell));
		currNodeIdx = q.PrevNode;
	} while (currNodeIdx >= 0);
	
	path.PushBack(dest);
	return path;
}

void SmoothPath(Dynarray<Vector2f>& path)
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
			Vector trans = pathfindingCmp->GetOwner()->GetTransform().GetGlobalTranslation();
			Optional<Dynarray<Vector2f>> path = CalculateNewPath(pathfindingCmp->NavigationGrid,
				Vector2f(trans.X, trans.Z), pathfindingCmp->CurentDestination.Value(), pathfindingCmp->AgentRadius);

			if (!path.HasValue())
			{
				pathfindingCmp->LastPathSearchFailed = true;
				continue;
			}

			pathfindingCmp->CalculatedPath = std::move(path.TakeValue());
		}
	}
}
