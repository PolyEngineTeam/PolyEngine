#pragma once

#include <map>

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
	/// Config for physics simulation
	/// @see Physics3DWorldComponent
	/// @see Trigger3DComponent
	/// @see Rigidbody3DComponent
	struct ENGINE_DLLEXPORT Physics3DConfig
	{
	public:
		const float TimeStep = 1.0f / 60.0f;
		const Vector Gravity = Vector(0.f, -9.81f, 0.f);
		const int MaxSimulationStepsPerFrame = 20;
	};

	/// World component contains configuration for physics simulation and 
	/// many other important things per world but only thing the user has to be 
	/// concerned about is gravity.
	/// @see Physics3DConfig
	/// @see Trigger3DComponent
	/// @see Rigidbody3DComponent
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
		std::map<const btCollisionObject*, UniqueID> BulletTriggerToEntity;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, Physics3DWorldComponent)
} // namespace Poly