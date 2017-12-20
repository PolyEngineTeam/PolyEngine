#pragma once

#include <Vector.hpp>

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"

class btCollisionShape;
struct btDefaultMotionState;
class btRigidBody;
class btSliderConstraint;

namespace Poly
{
	class World;
	struct RigidBody3DData;

	enum class eRigidBody3DType
	{
		STATIC,
		KINEMATIC,
		DYNAMIC,
		_COUNT
	};

	class Rigidbody3DComponent : public ComponentBase
	{
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
	public:
		Rigidbody3DComponent(World* world, eRigidBody3DType type, btCollisionShape* shape, float mass);
		

		eRigidBody3DType GetBodyType() const { return BodyType; }

	private:
		World* BodyWorld;
		eRigidBody3DType BodyType;

		btCollisionShape* Shape;
		btDefaultMotionState* MotionState;
		btRigidBody* RigidBody;
		btSliderConstraint* Constraint;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Rigidbody3DComponent)
}