#pragma once

#include <OrderedMap.hpp>

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"

class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;

class btCollisionObject;
class btRigidBody;

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

		friend void Physics3DSystem::RegisterTriger(World* world, const UniqueID& entityID);
		friend void Physics3DSystem::UnregisterTriger(World* world, const UniqueID& entityID);

		friend bool Physics3DSystem::IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID);
		friend const ContactResult& Physics3DSystem::ContactPair(World* world, const UniqueID& firstID, const UniqueID& secondID);
		friend const ContactResult& Physics3DSystem::Contact(World* world, const UniqueID& entityID);

		friend const RaycastResult& Physics3DSystem::AllHitsRaycast(World* world, const Vector& from, const Vector& to);
		friend const RaycastResult& Physics3DSystem::ClosestHitRaycast(World* world, const Vector& from, const Vector& to);
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
		OrderedMap<btRigidBody*, UniqueID> BulletRigidbodyToEntity;
		OrderedMap<btCollisionObject*, UniqueID> BulletTriggerToEntity;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, Physics3DWorldComponent)
}