#pragma once

#include <Vector.hpp>

namespace Poly
{
	namespace Physics3DSystem
	{
		void ENGINE_DLLEXPORT Physics3DUpdatePhase(World* world);

		void ENGINE_DLLEXPORT RegisterRigidbody(World* world, const UniqueID& entityID);
		void ENGINE_DLLEXPORT UnregisterRigidBody(World* world, const UniqueID& entityID);

		void ENGINE_DLLEXPORT RegisterTriger(World* world, const UniqueID& entityID);
		void ENGINE_DLLEXPORT UnregisterTriger(World* world, const UniqueID& entityID);
	}
}
