#pragma once

#include <Defines.hpp>
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
	struct Rigidbody3DComponentTemplate : BaseObject<>
	{
		float Mass = 0;
		/// If inertia isn't set then it will be computed from mass and shape
		Optional<Vector> Inertia;
		Optional<Vector> Gravity;
		/// Restitution is in fact a bounciness.
		float Restitution = 0.5f;
		float Friction = 0.5f;
		float RollingFriction = 0.5f;
		float LinearDamping = 0.5f;
		float AngularDamping = 0.5f;
		Vector LinearFactor = Vector(1.f, 1.f, 1.f);
		Vector AngularFactor = Vector(1.f, 1.f, 1.f);
		Optional<Vector> LinearVelocity = Vector(0.f, 0.f, 0.f);
		Optional<Vector> AngularVelocity = Vector(0.f, 0.f, 0.f);

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


		void SetMassProps(float mass, const Vector& inertia);
		void SetRestitution(float restitution);
		void SetFriction(float friction);
		void SetRollingFriction(float rollingFriction);
		void SetDamping(float linearDamping, float angularDamping);

		void SetLinearFactor(const Vector& linearFactor);
		void SetAngularFactor(const Vector& angularFactor);

		void SetLinearVelocity(const Vector& velocity);
		void SetAngularVelocity(const Vector& velocity);


		// getters


		float GetMass() const { return Template.Mass; }
		const Vector& GetInertia() const { return Template.Inertia.Value(); }
		float GetRestitution() const { return Template.Restitution; }
		float GetFriction() const { return Template.Friction; }
		float GetRollingFriction() const { return Template.RollingFriction; }
		float GetLinearDamping() const { return Template.LinearDamping; }
		float GetAngularDamping() const { return Template.AngularDamping; }

		const Vector& GetLinearFactor() const { return Template.LinearFactor; }
		const Vector& GetAngularFactor() const { return Template.AngularFactor; }

		Vector GetLinearVelocity();
		Vector GetAngularVelocity();


		bool IsRegistered() const { return Template.Registered; }
		bool IsInitialized() const { return Initialized; }
		eRigidBody3DType GetBodyType() const { return Template.RigidbodyType; }


		// other

		void ApplyImpulseToCenter(const Vector& impulse);
		void ApplyImpulse(const Vector& impulse, const Vector& relPos);
		void ApplyTorqueImpulse(const Vector& torque);

		void SetGravity(const Vector& gravity);
		Vector GetGravity() const;

	private:
		void UpdatePosition();

		bool Initialized = false;
		Scene* BodyWorld;
		std::unique_ptr<Rigidbody3DImpl> ImplData;

		bool TemplateChanged = true;
		Rigidbody3DComponentTemplate Template;

		Optional<Vector> ImpulseToCenter;
		Optional<Vector> Impulse;
		Optional<Vector> ImpulsePos;
		Optional<Vector> TorqueImpulse;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Rigidbody3DComponent)
} // namespace Poly