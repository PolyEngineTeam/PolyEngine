#pragma once

#include <Vector.hpp>

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"
#include "Physics3DShapes.hpp"

class btCollisionShape;
struct btDefaultMotionState;
class btRigidBody;

namespace Poly
{
	class World;
	struct RigidBody3DData;

	enum class eRigidBody3DType
	{
		STATIC,
		DYNAMIC,
		_COUNT
	};

	struct Rigidbody3DComponentConstructionInfo
	{
		float Mass = 0;
		float Friction = 0;

		Physics3DShape* Shape;
		eRigidBody3DType Type;
	};

	class ENGINE_DLLEXPORT Rigidbody3DComponent : public ComponentBase
	{
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
		friend void Physics3DSystem::RegisterRigidbody(World * world, const UniqueID& entityID);
		friend void Physics3DSystem::UnregisterRigidBody(World * world, const UniqueID& entityID);
	public:
		Rigidbody3DComponent(World* world, eRigidBody3DType type, Physics3DShape* shape,
			float restitution = 0, float friction = 0, float rollingFriction = 0, float spinningFriction = 0, float mass = 0);
		~Rigidbody3DComponent();


		void ApplyForceToCenter(const Vector& force);
		void ApplyImpulseToCenter(const Vector& impulse);
		void ApplyDamping(float timestep);
		void ApplyForce(const Vector& force, const Vector& relPos);
		void ApplyGravity();
		void ApplyImpulse(const Vector& impulse, const Vector& relPos);
		void ApplyTorque(const Vector& torque);
		void ApplyTorqueImpulse(const Vector& torque);

		void ClearForces();
		void UpdatePosition();

		void SetLinearFactor(const Vector& factor);
		const Vector& GetLinearFactor();
		void SetLinearVelocity(const Vector& velocity);
		const Vector& GetLinearVelocity();

		void SetAngularFactor(float factor);
		void SetAngularFactor(const Vector& factor);
		const Vector& GetAngularFactor();
		void SetAngularVelocity(const Vector& velocity);
		const Vector& GetAngularVelocity();

		void SetDamping(float linearDamping, float angularDamping);
		float GetLinearDamping();
		float GetAngularDamping();

		void SetGravity(float gravity);
		float GetGravity();

		void SetMassProperties(float mass, const Vector& intertia);
		float GetInvMass();

		void SetFixedTranslation(bool fixed);
		void SetFixedRotation(bool fixed);

		eRigidBody3DType GetBodyType() const { return BodyType; }

	private:
		bool Registered;

		World* BodyWorld;
		eRigidBody3DType BodyType;

		btCollisionShape* Shape;
		btDefaultMotionState* MotionState;
		btRigidBody* RigidBody;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Rigidbody3DComponent)
}