#pragma once

#include <Vector.hpp>

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"

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

	class Rigidbody3DComponent : public ComponentBase
	{
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
	public:
		Rigidbody3DComponent(World* world, eRigidBody3DType type, btCollisionShape* shape, float mass);
		~Rigidbody3DComponent();

		eRigidBody3DType GetBodyType() const { return BodyType; }

	private:
		void UpdatePosition();

		World* BodyWorld;
		eRigidBody3DType BodyType;

		btCollisionShape* Shape;
		btDefaultMotionState* MotionState;
		btRigidBody* RigidBody;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Rigidbody3DComponent)
}