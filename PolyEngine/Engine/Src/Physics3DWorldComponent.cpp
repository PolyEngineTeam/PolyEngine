#include "EnginePCH.hpp"

#include "Physics3DWorldComponentImpl.hpp"

//------------------------------------------------------------------------------
Poly::Physics3DWorldComponent::Physics3DWorldComponent(Physics3DConfig config)
	: Config(config)
{
	ImplData = std::make_unique<Physics3DWorldComponentImplData>();

	ImplData->CollisionConfiguration = new btDefaultCollisionConfiguration();
	ImplData->Dispatcher = new btCollisionDispatcher(ImplData->CollisionConfiguration);
	ImplData->Broadphase = new btDbvtBroadphase();
	ImplData->Solver = new btSequentialImpulseConstraintSolver();
	ImplData->DynamicsWorld = new btDiscreteDynamicsWorld(ImplData->Dispatcher, ImplData->Broadphase, ImplData->Solver, ImplData->CollisionConfiguration);

	// set default gravity
	ImplData->DynamicsWorld->setGravity(btVector3(config.Gravity.X, config.Gravity.Y, config.Gravity.Z));
}

//------------------------------------------------------------------------------
Poly::Physics3DWorldComponent::~Physics3DWorldComponent()
{
	delete ImplData->DynamicsWorld;
	delete ImplData->Solver;
	delete ImplData->Broadphase;
	delete ImplData->Dispatcher;
	delete ImplData->CollisionConfiguration;
}

//------------------------------------------------------------------------------
void Poly::Physics3DWorldComponent::SetGravity(const Vector& g)
{
	Config.Gravity = g;
	ImplData->DynamicsWorld->setGravity(btVector3(g.X, g.Y, g.Z));
}

void Poly::Physics3DWorldComponent::SetTimeStep(float timestep)
{
	Config.TimeStep = timestep;
}