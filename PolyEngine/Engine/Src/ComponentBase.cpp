#include "EnginePCH.hpp"

using namespace Poly;

EntityTransform& ComponentBase::GetTransform()
{
	return Owner->GetTransform();
}

const EntityTransform& ComponentBase::GetTransform() const
{
	return Owner->GetTransform();
}