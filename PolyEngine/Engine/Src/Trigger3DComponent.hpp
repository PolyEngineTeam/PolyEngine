#pragma once

#include <Vector.hpp>

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"
#include "Physics3DShapes.hpp"

class btCollisionShape;
class btCollisionObject;

namespace Poly
{
	class ENGINE_DLLEXPORT Trigger3DComponent : public ComponentBase
	{
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
		friend void Physics3DSystem::RegisterTriger(World * world, const UniqueID& entityID);
		friend void Physics3DSystem::UnregisterTriger(World * world, const UniqueID& entityID);
		friend bool Physics3DSystem::Colliding(World* world, const UniqueID& firstID, const UniqueID& secondID);
	public:
		Trigger3DComponent(Physics3DShape* shape);
		~Trigger3DComponent();

		const Physics3DShape const* Shape; // FIXME(squares): ...
	private:
		bool Registered = false;

		World* BodyWorld;

		btCollisionObject* BulletTrigger;
	};
} // namespace Poly