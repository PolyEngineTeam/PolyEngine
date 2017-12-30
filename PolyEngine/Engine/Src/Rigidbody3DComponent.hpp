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
	public:
		Rigidbody3DComponent(World* world, eRigidBody3DType type, Physics3DShape* shape, float mass);
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

		void SetAngularFactor();
		const Vector& GetAngularFactor();
		void SetAngularVelocity();
		const Vector& GetAngularVelocity();

		void SetDamping(float dampfactor);
		float GetLinearDamping();
		float GetAngularDamping();

		void SetGravity();
		float GetGravity();

		void SetLinearFactor();
		const Vector& GetLinearFactor();
		void SetLinearVelocity();
		const Vector& GetLinearVelocity();

		void SetMassProperties();
		float GetInvMass();

		void SetFixedTranslation(bool fixed);
		void SetFixedRotation(bool fixed);

		void UpdatePosition();

		eRigidBody3DType GetBodyType() const { return BodyType; }

	private:
		World* BodyWorld;
		eRigidBody3DType BodyType;

		btCollisionShape* Shape;
		btDefaultMotionState* MotionState;
		btRigidBody* RigidBody;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Rigidbody3DComponent)
}