#include "EnginePCH.hpp"

#include "PathfindingSystem.hpp"
#include "PathfindingComponent.hpp"

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

	auto comparator = [&AllNodes](size_t a, size_t b) { return AllNodes[a].TotalCost() < AllNodes[b].TotalCost(); };
	
	PriorityQueue<i64> openList(comparator), closedList(comparator);
	std::unordered_map<const NavNode*, float> minCosts;

	AllNodes.PushBack(PathNode(startNode, 0, graph->GetHeuristicCost(startNode, destNode) ));
	openList.Push(AllNodes.GetSize() - 1);
	minCosts[startNode] = 0;

	i64 bestNodeIdx = -1;
	while (openList.GetSize() > 0 && bestNodeIdx < 0)
	{
		i64 qIdx = openList.Pop();
		
		Dynarray<const NavNode*> connections = graph->GetConnections(AllNodes[qIdx].Node);
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

			auto& it = minCosts.find(s.Node);
			if (it != minCosts.end() && it->second < s.Cost)
				continue; // node has worse base cost than other (in the same pos) we visited before, skip it

			AllNodes.PushBack(s);
			openList.Push(AllNodes.GetSize() - 1);
			minCosts[s.Node] = s.Cost;
		}

		closedList.Push(qIdx);
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

ENGINE_DLLEXPORT void Poly::PathfindingSystem::UpdatePhase(World* world)
{
	for (auto& tuple : world->IterateComponents<PathfindingComponent>())
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

			pathfindingCmp->CalculatedPath = std::move(path.TakeValue());
		}
	}
}
