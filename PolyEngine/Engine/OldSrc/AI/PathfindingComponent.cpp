#include <EnginePCH.hpp>

#include <AI/PathfindingComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::PathfindingComponent)

void Poly::PathfindingComponent::SetDestination(const ::pe::core::math::Vector& pos)
{
	if(!CurentDestination.has_value() || CurentDestination.value() != pos)
		RecalculateRequested = true;

	CurentDestination = pos;
}

void Poly::PathfindingComponent::ResetDestination()
{
	CurentDestination = {};
	CalculatedPath.clear();
	RecalculateRequested = false;
}




