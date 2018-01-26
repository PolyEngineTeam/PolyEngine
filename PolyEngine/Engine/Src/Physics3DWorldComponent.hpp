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

namespace Poly
{
	/// Config for physics simulation
	/// @see Physics3DWorldComponent
	/// @see Trigger3DComponent
	/// @see Rigidbody3DComponent
	struct ENGINE_DLLEXPORT Physics3DConfig
	{
	public:
		float TimeStep = 1.f / 60.f;
		Vector Gravity = Vector(0.f, -9.81f, 0.f);
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
		friend void Physics3DSystem::RegisterComponent(World* world, const UniqueID& entityID, bool enablePhysics);
		friend void Physics3DSystem::UnregisterComponent(World * world, const UniqueID& entityID);
		friend bool Physics3DSystem::IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID);
		friend ContactResult Physics3DSystem::ContactPair(World* world, const UniqueID& firstID, const UniqueID& secondID);
		friend ContactResult Physics3DSystem::Contact(World* world, const UniqueID& entityID);
		friend RaycastResult Physics3DSystem::AllHitsRaycast(World* world, const Vector& from, const Vector& to, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith);
		friend RaycastResult Physics3DSystem::ClosestHitRaycast(World* world, const Vector& from, const Vector& to, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith);
	public:
		Physics3DWorldComponent(Physics3DConfig config);
		~Physics3DWorldComponent();

		void SetGravity(const Vector& g);
		void SetTimeStep(float timestep);

		const Vector& GetGravity() const { return Config.Gravity; }
		float GetTimeStep() const { return Config.TimeStep; }

	private:
		Physics3DConfig Config;

		std::unique_ptr<btDiscreteDynamicsWorld> DynamicsWorld;

		std::unique_ptr<btDefaultCollisionConfiguration> CollisionConfiguration;
		std::unique_ptr<btCollisionDispatcher> Dispatcher;
		std::unique_ptr<btBroadphaseInterface> Broadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver> Solver;

		float LastDeltaOverflow = 0.f;
		// raytest returns bullet rigidbody and we want to get UniqueID of that body
		std::map<const btCollisionObject*, UniqueID> BulletTriggerToEntity;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, Physics3DWorldComponent)
} // namespace Poly