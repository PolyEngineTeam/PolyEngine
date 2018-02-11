#pragma once

#include <Core.hpp>
#include "ComponentBase.hpp"
#include "Optional.hpp"
#include <Vector2f.hpp>

#include "PathfindingSystem.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT NavNode : public BaseObject<>
	{};

	class ENGINE_DLLEXPORT NavGraph : public BaseObject<>
	{
	public:
		virtual float GetTravelCost(const Poly::NavNode* from, const Poly::NavNode* to) const = 0;
		virtual float GetHeuristicCost(const Poly::NavNode* from, const Poly::NavNode* to) const = 0;
		virtual Vector GetNodeWorldPosition(const Poly::NavNode* node) const = 0;
		virtual const NavNode* GetNodeFromWorldPosition(const Vector& pos) const = 0;
		virtual Dynarray<const NavNode*> GetConnections(const Poly::NavNode* node) const = 0;
	};

	class ENGINE_DLLEXPORT PathfindingComponent : public ComponentBase
	{
	public:
		PathfindingComponent(const NavGraph* navGraphs) : NavigationGraph(navGraphs) {}

		void SetDestination(const Vector& pos);
		void ResetDestination();

		inline const Dynarray<Vector>& GetPath() const { return CalculatedPath; }
	private:
		const NavGraph* NavigationGraph = nullptr;
		Dynarray<Vector> CalculatedPath;
		Optional<Vector> CurentDestination;
		bool RecalculateRequested = false;
		bool LastPathSearchFailed = false;

		friend void PathfindingSystem::UpdatePhase(World* world);
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PathfindingComponent)
}
