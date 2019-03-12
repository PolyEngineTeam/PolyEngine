#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Physics2D/Physics2DSystem.hpp>
#include <Physics2D/Rigidbody2DComponent.hpp>

class b2World;

namespace Poly
{
	struct ENGINE_DLLEXPORT Physics2DConfig
	{
	public:
		const float TimeStep = 1.0f / 60.0f;
		const int32_t VelocityIterations = 8;
		const int32_t PositionIterations = 3;
		const Vector Gravity = Vector(0.f, -9.81f, 0.f);
	};

	class Physics2DContactListener;

	class ENGINE_DLLEXPORT Physics2DWorldComponent : public ComponentBase
	{
		friend void Physics2DSystem::Physics2DUpdatePhase(Scene* world);
		friend class Physics2DContactListener;
		friend class RigidBody2DComponent;
	public:
		RTTI_DECLARE_COMPONENT(::Poly::Physics2DWorldComponent) { NO_RTTI_PROPERTY(); }

		struct Collision
		{
			RigidBody2DComponent* rb;
			Vector Normal;
		};

		Physics2DWorldComponent(const Physics2DConfig& config);
		~Physics2DWorldComponent();

		const Dynarray<Collision>& GetCollidingBodies(RigidBody2DComponent* rb) const;

		void SetGravity(const Vector& gravity) const;
		const Vector& GetGravity() const { return Config.Gravity; }
	private:
		float LastDeltaOverflow = 0.f;

		const Physics2DConfig Config;
		std::unique_ptr<b2World> Scene;
		std::unique_ptr<Physics2DContactListener> ContactListener;

		std::unordered_map<RigidBody2DComponent*, Dynarray<Collision>> OverlapingBodies;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Physics2DWorldComponent)
}
