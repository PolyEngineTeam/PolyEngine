#include <EnginePCH.hpp>

#include <Box2D/Box2D.h>
#include <Physics2D/Physics2DColliders.hpp>

RTTI_DEFINE_COMPONENT(::Poly::Box2DColliderComponent)
RTTI_DEFINE_COMPONENT(::Poly::Circle2DColliderComponent)

Poly::Box2DColliderComponent::Box2DColliderComponent(const Vector& size)
{
	Shape = std::make_unique<b2PolygonShape>();
	Shape->SetAsBox(size.X / 2, size.Y / 2);
}

Poly::Box2DColliderComponent::~Box2DColliderComponent()
{
}

void Poly::Box2DColliderComponent::SetSize(const Vector& size)
{
	Shape->SetAsBox(size.X / 2, size.Y / 2);
}


Poly::Circle2DColliderComponent::Circle2DColliderComponent(float size)
{
	Shape = std::make_unique<b2CircleShape>();
	Shape->m_radius = size;
}

Poly::Circle2DColliderComponent::~Circle2DColliderComponent()
{
}

void Poly::Circle2DColliderComponent::SetSize(float size)
{
	Shape->m_radius = size;
}
