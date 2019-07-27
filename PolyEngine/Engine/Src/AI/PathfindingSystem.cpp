#include <EnginePCH.hpp>

#include <AI/PathfindingSystem.hpp>
#include <AI/PathfindingComponent.hpp>
#include <ECS/Scene.hpp>

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

std::optional<std::vector<Vector>> CalculateNewPath(const NavGraph* graph, const Vector& start, const Vector& dest, size_t depthLimit)
{
	// Swap start and dest to better handle path parsing later
	Vector startInternal = dest;
	Vector destInternal = start;

	const NavNode* startNode = graph->GetNodeFromWorldPosition(startInternal);
	const NavNode* destNode = graph->GetNodeFromWorldPosition(destInternal);
	if(!startNode || !destNode)
		return {};

	std::vector<PathNode> AllNodes;

	PriorityQueue<std::pair<i64, float>, PathNodeCmp> openList, closedList;
	std::map<const NavNode*, float> minCosts;

	AllNodes.push_back(PathNode(startNode, 0, graph->GetHeuristicCost(startNode, destNode) ));
	openList.Push(std::make_pair(AllNodes.size() - 1, graph->GetHeuristicCost(startNode, destNode)));
	minCosts.insert({startNode, 0.f});

	i64 bestNodeIdx = -1;
	std::vector<const NavNode*> connections(8);
	while (openList.GetSize() > 0 && bestNodeIdx < 0)
	{
		i64 qIdx = openList.Pop().first;
		
		connections.clear();
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

			const auto& valIt = minCosts.find(s.Node);
			if (valIt != minCosts.end() && valIt->second < s.Cost)
				continue; // node has worse base cost than other (in the same pos) we visited before, skip it

			AllNodes.push_back(s);
			openList.Push(std::make_pair(AllNodes.size() - 1, s.TotalCost()));
			minCosts[s.Node] = s.Cost;
		}

		closedList.Push(std::make_pair(qIdx, AllNodes[qIdx].TotalCost()));
		minCosts[AllNodes[qIdx].Node] = AllNodes[qIdx].Cost;
	}

	if (bestNodeIdx < 0)
		return {};

	std::vector<Vector> path;
	path.push_back(start);
	i64 currNodeIdx = bestNodeIdx;
	do
	{
		const PathNode& q = AllNodes[currNodeIdx];
		path.push_back(graph->GetNodeWorldPosition(q.Node));
		currNodeIdx = q.PrevNode;
	} while (currNodeIdx >= 0);
	
	path.push_back(dest);
	return path;
}

void SmoothPath(const NavGraph* graph, std::vector<Vector>& path)
{
	size_t currIdx = 0;
	while (currIdx < path.size() - 1)
	{

		size_t stillVisibleIdx = currIdx;
		for (size_t idx = path.size() - 1; idx > currIdx; --idx)
		{
			if (graph->CanConnectDirectly(graph->GetNodeFromWorldPosition(path[currIdx]), graph->GetNodeFromWorldPosition(path[idx])))
			{
				stillVisibleIdx = idx;
				break;
			}
		}

		for (size_t idx = currIdx + 1; idx < stillVisibleIdx; ++idx)
			path.erase(path.begin() + currIdx + 1);

		++currIdx;
	}
}

ENGINE_DLLEXPORT void Poly::PathfindingSystem::UpdatePhase(Scene* world)
{
	for (auto tuple : world->IterateComponents<PathfindingComponent>())
	{
		PathfindingComponent* pathfindingCmp = std::get<PathfindingComponent*>(tuple);

		if (pathfindingCmp->RecalculateRequested)
		{
			pathfindingCmp->RecalculateRequested = false;
			Vector trans = pathfindingCmp->GetOwner()->GetTransform().GetGlobalTranslation();
			std::optional<std::vector<Vector>> path = CalculateNewPath(pathfindingCmp->NavigationGraph,
				trans, pathfindingCmp->CurentDestination.value(), 16);

			if (!path.has_value())
			{
				pathfindingCmp->LastPathSearchFailed = true;
				continue;
			}

			SmoothPath(pathfindingCmp->NavigationGraph, path.value());
			pathfindingCmp->CalculatedPath = std::move(path.value());
		}
	}
}
