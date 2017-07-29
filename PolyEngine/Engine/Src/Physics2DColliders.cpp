#include "EnginePCH.hpp"
#include "Box2D/Box2D.h"
#include "Physics2DColliders.hpp"

Poly::Box2DColliderComponent::Box2DColliderComponent(const Vector& size)
{
	Shape = std::make_unique<b2PolygonShape>();
	Shape->SetAsBox(size.X / 2, size.Y / 2);
}

Poly::Box2DColliderComponent::~Box2DColliderComponent()
{
}

void Poly::Box2DColliderComponent::SetSize(Vector size)
{
	Shape->SetAsBox(size.X / 2, size.Y / 2);
}
