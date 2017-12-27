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

	class ENGINE_DLLEXPORT Rigidbody3DComponent : public ComponentBase
	{
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
	public:
		Rigidbody3DComponent(World* world, eRigidBody3DType type, Physics3DShape* shape, float mass = 0);
		~Rigidbody3DComponent();

		eRigidBody3DType GetBodyType() const { return BodyType; }
		void UpdatePosition();

	private:

		World* BodyWorld;
		eRigidBody3DType BodyType;

		btCollisionShape* Shape;
		btDefaultMotionState* MotionState;
		btRigidBody* RigidBody;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Rigidbody3DComponent)
}