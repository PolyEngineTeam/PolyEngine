#pragma once

#include <Vector.hpp>

class btRigidBody;

namespace Poly
{
	namespace Physics3DSystem
	{
		void Physics3DUpdatePhase(World* world);

		void RegisterRigidbody(World* world, btRigidBody* body);
	}
}
