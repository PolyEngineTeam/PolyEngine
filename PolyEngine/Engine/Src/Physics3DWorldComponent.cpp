#include "EnginePCH.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

//********************************************************************************************************************************************
Poly::Physics3DWorldComponent::Physics3DWorldComponent(Physics3DConfig config)
	: Config(config)
{
	CollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	Dispatcher = std::make_unique<btCollisionDispatcher>(CollisionConfiguration.get());
	Broadphase = std::make_unique<btDbvtBroadphase>();
	Solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	DynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(Dispatcher.get(), Broadphase.get(), Solver.get(), CollisionConfiguration.get());

	// set default gravity
	DynamicsWorld->setGravity(btVector3(config.Gravity.X, config.Gravity.Y, config.Gravity.Z));
}

//********************************************************************************************************************************************
void Poly::Physics3DWorldComponent::SetGravity(Vector g)
{
	DynamicsWorld->setGravity(btVector3(g.X, g.Y, g.Z));
}

//********************************************************************************************************************************************
Poly::Vector Poly::Physics3DWorldComponent::GetGravity()
{
	btVector3 g = DynamicsWorld->getGravity();

	return Vector(g.getX(), g.getY(), g.getZ());
}
