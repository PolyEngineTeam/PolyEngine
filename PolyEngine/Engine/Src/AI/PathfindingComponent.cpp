#include "EnginePCH.hpp"

#include "PathfindingComponent.hpp"

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::PathfindingComponent)

void Poly::PathfindingComponent::SetDestination(const Vector& pos)
{
	if(!CurentDestination.HasValue() || CurentDestination.Value() != pos)
		RecalculateRequested = true;

	CurentDestination = pos;
}

void Poly::PathfindingComponent::ResetDestination()
{
	CurentDestination = Optional<Vector>();
	CalculatedPath.Clear();
	RecalculateRequested = false;
}




