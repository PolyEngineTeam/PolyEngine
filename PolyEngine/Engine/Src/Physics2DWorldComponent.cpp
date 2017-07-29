#include "EnginePCH.hpp"

#include "Physics2DWorldComponent.hpp"
#include <Box2D/Box2D.h>

using namespace Poly;

namespace Poly {
	class Physics2DContactListener : public b2ContactListener
	{
	public:
		Physics2DContactListener(Physics2DWorldComponent* cmp)
			: Component(cmp)
		{}

		void BeginContact(b2Contact* contact)
		{
			RigidBody2DComponent* rb1 = static_cast<RigidBody2DComponent*>(contact->GetFixtureA()->GetUserData());
			RigidBody2DComponent* rb2 = static_cast<RigidBody2DComponent*>(contact->GetFixtureB()->GetUserData());
			Component->OverlapingBodies[rb1].PushBack(rb2);
			Component->OverlapingBodies[rb2].PushBack(rb1);
		}

		void EndContact(b2Contact* contact)
		{
			RigidBody2DComponent* rb1 = static_cast<RigidBody2DComponent*>(contact->GetFixtureA()->GetUserData());
			RigidBody2DComponent* rb2 = static_cast<RigidBody2DComponent*>(contact->GetFixtureB()->GetUserData());
			Component->OverlapingBodies[rb1].Remove(rb2);
			Component->OverlapingBodies[rb2].Remove(rb1);
		}

		Physics2DWorldComponent* Component;
	};
}

Physics2DWorldComponent::Physics2DWorldComponent(const Physics2DConfig& config)
	: Config(config), World(std::make_unique<b2World>(b2Vec2(config.Gravity.X, config.Gravity.Y)))
{
	ContactListener = std::make_unique<Physics2DContactListener>(this);
}

Poly::Physics2DWorldComponent::~Physics2DWorldComponent()
{
}

const Dynarray<RigidBody2DComponent*>& Poly::Physics2DWorldComponent::GetOverlapingBodies(RigidBody2DComponent* rb) const
{
	return OverlapingBodies.at(rb);
}
