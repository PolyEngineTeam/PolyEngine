#pragma once

#include <Vector.hpp>

class btRigidBody;

namespace Poly
{
	namespace Physics3DSystem
	{
		void ENGINE_DLLEXPORT Physics3DUpdatePhase(World* world);

		void ENGINE_DLLEXPORT RegisterRigidbody(World* world, const UniqueID& entityID);
		void ENGINE_DLLEXPORT UnregisterRigidBody(World* world, const UniqueID& entityID);
	}
}
