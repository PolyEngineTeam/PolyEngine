#pragma once

#include "ComponentBase.hpp"

class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;

namespace Poly
{
	struct ENGINE_DLLEXPORT Physics3DConfig
	{
	public:
		const float TimeStep = 1.0f / 60.0f;
		const Vector Gravity = Vector(0.f, -9.81f, 0.f);
		const int MaxSimulationStepsPerFrame = 20;
	};

	class ENGINE_DLLEXPORT Physics3DWorldComponent : public ComponentBase
	{
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
		friend void Physics3DSystem::RegisterRigidbody(World* world, const UniqueID& entityID);
		friend void Physics3DSystem::UnregisterRigidBody(World* world, const UniqueID& entityID);
	public:
		Physics3DWorldComponent(Physics3DConfig config);

		void SetGravity(Vector g);
		Vector GetGravity();

	private:
		const Physics3DConfig Config;

		btDiscreteDynamicsWorld* DynamicsWorld;

		btDefaultCollisionConfiguration* CollisionConfiguration;
		btCollisionDispatcher* Dispatcher;
		btBroadphaseInterface* Broadphase;
		btSequentialImpulseConstraintSolver* Solver;

		float LastDeltaOverflow = 0.f;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, Physics3DWorldComponent)
}