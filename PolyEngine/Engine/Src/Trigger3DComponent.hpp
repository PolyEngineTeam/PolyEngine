#pragma once

#include <Vector.hpp>

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"
#include "Physics3DShapes.hpp"

namespace Poly
{
	struct Trigger3DImpl;

	/// You should use this class when You need to detect collision without making entity rigid body.
	/// @see Physics3DWorldComponent
	/// @see Rigidbody3DComponent
	class ENGINE_DLLEXPORT Trigger3DComponent : public ComponentBase
	{
		friend class Rigidbody3DComponent;
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
		friend void Physics3DSystem::RegisterTriger(World * world, const UniqueID& entityID, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith);
		friend void Physics3DSystem::UnregisterTriger(World * world, const UniqueID& entityID);
		friend bool Physics3DSystem::IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID);
	public:
		/// Creates trigger with given @see[Physics3DShape].
		/// @param shape - collider shape
		/// @see Rigidbody3DComponent
		Trigger3DComponent(World* world, Physics3DShape* shape);
		~Trigger3DComponent();

		bool IsRegistered() const { return Registered; }
		const Physics3DShape& GetShape();

		void EnsureInit();

	private:
		bool Registered = false;

		World* BodyWorld;

		std::unique_ptr<Trigger3DImpl> ImplData;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Trigger3DComponent)
} // namespace Poly