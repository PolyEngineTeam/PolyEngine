#include "EnginePCH.hpp"

#include <btBulletDynamicsCommon.h>

//********************************************************************************************************************************************
Poly::Physics3DWorldComponent::Physics3DWorldComponent(Physics3DConfig config)
	: Config(config)
{
	// create importand things for bullet world
	CollisionConfiguration = new btDefaultCollisionConfiguration();
	Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
	Broadphase = new btDbvtBroadphase();
	Solver = new btSequentialImpulseConstraintSolver();
	
	// create world
	DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher, Broadphase, Solver, CollisionConfiguration);

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
