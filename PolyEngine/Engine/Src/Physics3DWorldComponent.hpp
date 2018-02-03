#pragma once

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"

namespace Poly
{
	struct Physics3DWorldComponentImplData;

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
		friend void Physics3DSystem::RegisterComponent(World* world, Entity* entity, bool enablePhysics);
		friend void Physics3DSystem::UnregisterComponent(World * world, Entity* entity);
		friend bool Physics3DSystem::IsColliding(World* world, Entity* firstEntity, Entity* secondEntity);
		friend ContactResult Physics3DSystem::ContactPair(World* world, Entity* firstEntity, Entity* secondEntity);
		friend ContactResult Physics3DSystem::Contact(World* world, Entity* entity);
		friend ContactPairResults Poly::Physics3DSystem::GetAllContactPairs(World* world);
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

		std::unique_ptr<Physics3DWorldComponentImplData> ImplData;

		float LastDeltaOverflow = 0.f;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, Physics3DWorldComponent)
} // namespace Poly