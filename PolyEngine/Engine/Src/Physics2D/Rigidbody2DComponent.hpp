#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Physics2D/Physics2DSystem.hpp>

namespace Poly
{
	class Scene;

	enum class eRigidBody2DType
	{
		STATIC,
		KINEMATIC,
		DYNAMIC,
		_COUNT
	};

	struct RigidBody2DData;

	struct ENGINE_DLLEXPORT RigidBody2DSensorTag {};

	class ENGINE_DLLEXPORT RigidBody2DComponent : public ComponentBase
	{
		friend void Physics2DSystem::Physics2DUpdatePhase(Scene* world);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::RigidBody2DComponent) { NO_RTTI_PROPERTY(); }

		RigidBody2DComponent(Scene* world, eRigidBody2DType type, float density = 1.0f, float friction = 0.3f);
		RigidBody2DComponent(Scene* world, eRigidBody2DType type, RigidBody2DSensorTag sensorTag);
		~RigidBody2DComponent();

		void DebugPrintInfo() const;

		void ApplyForceToCenter(const Vector& force);
		void ApplyImpulseToCenter(const Vector& impulse);

		void SetLinearDamping(float dampfactor);
		void SetAngularDamping(float dampfactor);
		void SetFixedRotation(bool fixed);
		
		void SetDensity(float density);
		float GetDensity() const;

		Vector GetLinearVelocity() const;
		void SetLinearVelocity(const Vector& speed);
		float GetAngularVelocity() const;
		void SetAngularVelocity(float speed);

		void UpdatePosition();

		eRigidBody2DType GetBodyType() const { return BodyType; }
	private:
		void EnsureInit();

		Scene* BodyWorld;
		eRigidBody2DType BodyType;
		std::unique_ptr<RigidBody2DData> ImplData;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, RigidBody2DComponent)
}