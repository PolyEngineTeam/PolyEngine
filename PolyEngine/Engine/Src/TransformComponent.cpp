#include "TransformComponent.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
TransformComponent::TransformComponent(const Vector& direction)
{
	SetPosition(direction);
}

//------------------------------------------------------------------------------
void TransformComponent::SetPosition(const Vector& direction)
{
	Matrix.SetTranslation(direction);
}