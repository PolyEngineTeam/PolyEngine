#include "EnginePCH.hpp"

#include <btBulletDynamicsCommon.h>

Poly::Physics3DWorldComponent::Physics3DWorldComponent(Physics3DConfig config)
	: Config(config)
{
	CollisionConfiguration = new btDefaultCollisionConfiguration();
	Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
	Broadphase = new btDbvtBroadphase();
	Solver = new btSequentialImpulseConstraintSolver();
	
	DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher, Broadphase, Solver, CollisionConfiguration);

	DynamicsWorld->setGravity(btVector3(config.Gravity.X, config.Gravity.Y, config.Gravity.Z));
}

void Poly::Physics3DWorldComponent::SetGravity(Vector g)
{
	DynamicsWorld->setGravity(btVector3(g.X, g.Y, g.Z));
}

Poly::Vector Poly::Physics3DWorldComponent::GetGravity()
{
	btVector3 g = DynamicsWorld->getGravity();

	return Vector(g.getX(), g.getY(), g.getZ());
}
