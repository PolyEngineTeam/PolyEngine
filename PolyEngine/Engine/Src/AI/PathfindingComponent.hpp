#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <AI/PathfindingSystem.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT NavNode : public ::pe::core::BaseObject<>
	{};

	class ENGINE_DLLEXPORT NavGraph : public ::pe::core::BaseObject<>
	{
	public:
		virtual float GetTravelCost(const Poly::NavNode* from, const Poly::NavNode* to) const = 0;
		virtual float GetHeuristicCost(const Poly::NavNode* from, const Poly::NavNode* to) const = 0;
		virtual core::math::Vector GetNodeWorldPosition(const Poly::NavNode* node) const = 0;
		virtual const NavNode* GetNodeFromWorldPosition(const core::math::Vector& pos) const = 0;
		virtual void GetConnections(const Poly::NavNode* node, std::vector<const NavNode*>& connections) const = 0;
		virtual bool CanConnectDirectly(const NavNode* n1, const NavNode* n2) const = 0;
	};

	class ENGINE_DLLEXPORT PathfindingComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::PathfindingComponent) { NO_RTTI_PROPERTY(); }

		PathfindingComponent(const NavGraph* navGraphs) : NavigationGraph(navGraphs) {}

		void SetDestination(const core::math::Vector& pos);
		void ResetDestination();

		inline const std::vector<core::math::Vector>& GetPath() const { return CalculatedPath; }
	private:
		const NavGraph* NavigationGraph = nullptr;
		std::vector<core::math::Vector> CalculatedPath;
		std::optional<core::math::Vector> CurentDestination;
		bool RecalculateRequested = false;
		bool LastPathSearchFailed = false;

		friend void PathfindingSystem::UpdatePhase(Scene* world);
	};

	REGISTER_COMPONENT(::Poly::ComponentsIDGroup, PathfindingComponent)
}
