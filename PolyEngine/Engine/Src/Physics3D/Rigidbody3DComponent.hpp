#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Physics3D/Physics3DSystem.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	// helper structures


	/// Structure containing implementation dependent data.
	struct Rigidbody3DImpl;

	/// Rigidbody type; for now rigidbody can be only static or dynamic.
	/// @see Rigidbody3DComponent
	enum class eRigidBody3DType
	{
		STATIC,
		DYNAMIC,
		_COUNT
	};

	/// This structure is used for initialization and keeping important properties of rigid body.
	/// @see Rigidbody3DComponent
	/// @see eRigidBody3DType
	struct Rigidbody3DComponentTemplate : public ::pe::core::BaseObject<>
	{
		float Mass = 0;
		/// If inertia isn't set then it will be computed from mass and shape
		std::optional<core::math::Vector> Inertia;
		std::optional<core::math::Vector> Gravity;
		/// Restitution is in fact a bounciness.
		float Restitution = 0.5f;
		float Friction = 0.5f;
		float RollingFriction = 0.5f;
		float LinearDamping = 0.5f;
		float AngularDamping = 0.5f;
		core::math::Vector LinearFactor = core::math::Vector(1.f, 1.f, 1.f);
		core::math::Vector AngularFactor = core::math::Vector(1.f, 1.f, 1.f);
		std::optional<core::math::Vector> LinearVelocity = core::math::Vector(0.f, 0.f, 0.f);
		std::optional<core::math::Vector> AngularVelocity = core::math::Vector(0.f, 0.f, 0.f);

		/// Due to optimalization rigidbodies are deactivated after some time of idleness.
		bool Deactivatable = false;
		/// Rigidbody must be registered to collide and be hit during ray test.
		bool Registered = true;
		eRigidBody3DType RigidbodyType = eRigidBody3DType::DYNAMIC;
	};


	//------------------------------------------------------------------------------
	// component declaration


	/// You should use this component when you want to simulate physics bodies.
	/// If you simply want to detect collisions use @see[Collider3DComponent].
	/// @see Rigidbody3DComponentTemplate
	class ENGINE_DLLEXPORT Rigidbody3DComponent : public ComponentBase
	{

		// friendship declarations


		friend void Physics3DSystem::Physics3DUpdatePhase(Scene* world);
		friend void Physics3DSystem::EnsureInit(Scene* world, Entity* entity);
		friend void Physics3DSystem::RegisterComponent(Scene* world, Entity* entity, bool enablePhysics);
		friend void Physics3DSystem::UnregisterComponent(Scene * world, Entity* entity);

	public:
		RTTI_DECLARE_COMPONENT(::Poly::Rigidbody3DComponent) { NO_RTTI_PROPERTY(); }

		// constructors and destructor

		/// Creates Rigidbody3DComponent from provided template parameters.
		/// @param world - world where owner entity exists
		/// @param tmp - template with rigid body properties
		/// @see Rigidbody3DComponentTemplate
		Rigidbody3DComponent(Scene* world, const Rigidbody3DComponentTemplate& tmp);

		/// If rigid body is registered destructor will unregister it on component destruction.
		~Rigidbody3DComponent();


		// setters


		void SetMassProps(float mass, const core::math::Vector& inertia);
		void SetRestitution(float restitution);
		void SetFriction(float friction);
		void SetRollingFriction(float rollingFriction);
		void SetDamping(float linearDamping, float angularDamping);

		void SetLinearFactor(const core::math::Vector& linearFactor);
		void SetAngularFactor(const core::math::Vector& angularFactor);

		void SetLinearVelocity(const core::math::Vector& velocity);
		void SetAngularVelocity(const core::math::Vector& velocity);


		// getters


		float GetMass() const { return Template.Mass; }
		const core::math::Vector& GetInertia() const { return Template.Inertia.value(); }
		float GetRestitution() const { return Template.Restitution; }
		float GetFriction() const { return Template.Friction; }
		float GetRollingFriction() const { return Template.RollingFriction; }
		float GetLinearDamping() const { return Template.LinearDamping; }
		float GetAngularDamping() const { return Template.AngularDamping; }

		const core::math::Vector& GetLinearFactor() const { return Template.LinearFactor; }
		const core::math::Vector& GetAngularFactor() const { return Template.AngularFactor; }

		core::math::Vector GetLinearVelocity();
		core::math::Vector GetAngularVelocity();


		bool IsRegistered() const { return Template.Registered; }
		bool IsInitialized() const { return Initialized; }
		eRigidBody3DType GetBodyType() const { return Template.RigidbodyType; }


		// other

		void ApplyImpulseToCenter(const core::math::Vector& impulse);
		void ApplyImpulse(const core::math::Vector& impulse, const core::math::Vector& relPos);
		void ApplyTorqueImpulse(const core::math::Vector& torque);

		void SetGravity(const core::math::Vector& gravity);
		core::math::Vector GetGravity() const;

	private:
		void UpdatePosition();

		bool Initialized = false;
		Scene* BodyWorld;
		std::unique_ptr<Rigidbody3DImpl> ImplData;

		bool TemplateChanged = true;
		Rigidbody3DComponentTemplate Template;

		std::optional<core::math::Vector> ImpulseToCenter;
		std::optional<core::math::Vector> Impulse;
		std::optional<core::math::Vector> ImpulsePos;
		std::optional<core::math::Vector> TorqueImpulse;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Rigidbody3DComponent)
} // namespace Poly