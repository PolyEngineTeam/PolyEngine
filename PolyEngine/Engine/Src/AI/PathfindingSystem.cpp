#include "EnginePCH.hpp"

#include "AI/PathfindingSystem.hpp"
#include "PathfindingComponent.hpp"
#include <Collections/OrderedMap.hpp>

using namespace Poly;

struct PathNode
{
	PathNode(const NavNode* node, float cost, float heurCost, i64 prevNode = -1)
		: Node(node), Cost(cost), HeuristicCost(heurCost), PrevNode(prevNode) {}

	const NavNode* Node;
	float Cost;
	float HeuristicCost;
	i64 PrevNode = -1;
	float TotalCost() const { return Cost + HeuristicCost; }
};

struct PathNodeCmp
{
	bool operator()(const std::pair<i64, float>& a, const std::pair<i64, float>& b) const { return a.second < b.second; }
};

Optional<Dynarray<Vector>> CalculateNewPath(const NavGraph* graph, const Vector& start, const Vector& dest, size_t depthLimit)
{
	// Swap start and dest to better handle path parsing later
	Vector startInternal = dest;
	Vector destInternal = start;

	const NavNode* startNode = graph->GetNodeFromWorldPosition(startInternal);
	const NavNode* destNode = graph->GetNodeFromWorldPosition(destInternal);
	if(!startNode || !destNode)
		return Optional<Dynarray<Vector>>();

	Dynarray<PathNode> AllNodes;

	PriorityQueue<std::pair<i64, float>, PathNodeCmp> openList, closedList;
	OrderedMap<const NavNode*, float> minCosts;

	AllNodes.PushBack(PathNode(startNode, 0, graph->GetHeuristicCost(startNode, destNode) ));
	openList.Push(std::make_pair(AllNodes.GetSize() - 1, graph->GetHeuristicCost(startNode, destNode)));
	minCosts.Insert(startNode, 0.f);

	i64 bestNodeIdx = -1;
	Dynarray<const NavNode*> connections(8);
	while (openList.GetSize() > 0 && bestNodeIdx < 0)
	{
		i64 qIdx = openList.Pop().first;
		
		connections.Clear();
		graph->GetConnections(AllNodes[qIdx].Node, connections);
		for (const NavNode* connection : connections)
		{
			// we reached destination
			if (connection == destNode)
			{
				bestNodeIdx = qIdx;
				break;
			}
			
			const PathNode& q = AllNodes[qIdx];

			const float moveCost = graph->GetTravelCost(connection, q.Node);
			PathNode s{ connection, q.Cost + moveCost, graph->GetHeuristicCost(connection, destNode), qIdx };

			const auto& valOpt = minCosts.Get(s.Node);
			if (valOpt.HasValue() && valOpt.Value() < s.Cost)
				continue; // node has worse base cost than other (in the same pos) we visited before, skip it

			AllNodes.PushBack(s);
			openList.Push(std::make_pair(AllNodes.GetSize() - 1, s.TotalCost()));
			auto entry = minCosts.Entry(s.Node);
			if (entry.IsVacant())
				entry.VacantInsert(s.Cost);
			else
				entry.OccupiedReplace(s.Cost);
		}

		closedList.Push(std::make_pair(qIdx, AllNodes[qIdx].TotalCost()));
		minCosts[AllNodes[qIdx].Node] = AllNodes[qIdx].Cost;
	}

	if (bestNodeIdx < 0)
		return Optional<Dynarray<Vector>>();

	Dynarray<Vector> path;
	path.PushBack(start);
	i64 currNodeIdx = bestNodeIdx;
	do
	{
		const PathNode& q = AllNodes[currNodeIdx];
		path.PushBack(graph->GetNodeWorldPosition(q.Node));
		currNodeIdx = q.PrevNode;
	} while (currNodeIdx >= 0);
	
	path.PushBack(dest);
	return path;
}

void SmoothPath(const NavGraph* graph, Dynarray<Vector>& path)
{
	size_t currIdx = 0;
	while (currIdx < path.GetSize() - 1)
	{

		size_t stillVisibleIdx = currIdx;
		for (size_t idx = path.GetSize() - 1; idx > currIdx; --idx)
		{
			if (graph->CanConnectDirectly(graph->GetNodeFromWorldPosition(path[currIdx]), graph->GetNodeFromWorldPosition(path[idx])))
			{
				stillVisibleIdx = idx;
				break;
			}
		}

		for (size_t idx = currIdx + 1; idx < stillVisibleIdx; ++idx)
			path.RemoveByIdx(currIdx + 1);

		++currIdx;
	}
}

ENGINE_DLLEXPORT void Poly::PathfindingSystem::UpdatePhase(World* world)
{
	for (auto tuple : world->IterateComponents<PathfindingComponent>())
	{
		PathfindingComponent* pathfindingCmp = std::get<PathfindingComponent*>(tuple);

		if (pathfindingCmp->RecalculateRequested)
		{
			pathfindingCmp->RecalculateRequested = false;
			Vector trans = pathfindingCmp->GetOwner()->GetTransform().GetGlobalTranslation();
			Optional<Dynarray<Vector>> path = CalculateNewPath(pathfindingCmp->NavigationGraph,
				trans, pathfindingCmp->CurentDestination.Value(), 16);

			if (!path.HasValue())
			{
				pathfindingCmp->LastPathSearchFailed = true;
				continue;
			}

			SmoothPath(pathfindingCmp->NavigationGraph, path.Value());
			pathfindingCmp->CalculatedPath = path.TakeValue();
		}
	}
}
