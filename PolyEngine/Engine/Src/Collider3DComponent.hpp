#pragma once

#include <Vector.hpp>

#include "ComponentBase.hpp"
#include "Physics3DSystem.hpp"
#include "Physics3DShapes.hpp"

namespace Poly
{
	//------------------------------------------------------------------------------
	// helper structures


	/// Structure containing implementation dependent data.
	struct Trigger3DImpl;

	/// This structure is used for initialization and keeping important properties of collider.
	/// @see Collider3DComponent
	/// @see Physics3DShape
	/// @see eCollisionGroup
	struct Collider3DComponentTemplate : BaseObject<>
	{
		/// It's recommended to use one shape in multiple colliders rather than make copies of shape for each of them.
		std::unique_ptr<Physics3DShape> Shape;
		
		/// If this flag is set during Trigger3DComponent::EnsureInit() that collider will be immidiately registered.
		bool Registered = true;
		/// Determines in which collision group is currently collider.
		EnumFlags<eCollisionGroup> CollisionGroup = eCollisionGroup::TRIGGER;
		/// Determines whith which collision groups this collider will collide.
		EnumFlags<eCollisionGroup> CollisionMask = eCollisionGroup::TRIGGER;
	};


	//------------------------------------------------------------------------------
	// component declaration


	/// Collider3DComponent indicates that Entity containing it implements collision detection logic 
	/// (aka overlap/trigger logic) in 3D. Pair it with RidgidBody3DComponent to create dynamic physics body.
	/// @see Physics3DWorldComponent
	/// @see Rigidbody3DComponent
	class ENGINE_DLLEXPORT Collider3DComponent : public ComponentBase
	{
		// friendship declarations


		friend class Rigidbody3DComponent;
		friend void Physics3DSystem::Physics3DUpdatePhase(World* world);
		friend void Physics3DSystem::EnsureInit(World* world, Entity* entity);
		friend void Physics3DSystem::SetCollisionGroup(World* world, Entity* entity, EnumFlags<eCollisionGroup> group);
		friend void Physics3DSystem::SetCollisionMask(World* world, Entity* entity, EnumFlags<eCollisionGroup> mask);
		friend void Physics3DSystem::RegisterComponent(World* world, Entity* entity, bool enablePhysics);
		friend void Physics3DSystem::UnregisterComponent(World * world, Entity* entity);
		friend bool Physics3DSystem::IsColliding(World* world, Entity* firstEntity, Entity* secondEntity);

	public:
		// constructors and destructor


		/// Creates Collider3DComponent from provided template parameters.
		/// @param world - world where owner entity exists
		/// @param tmp - template with collider properties
		/// @see Collider3DComponentTemplate
		Collider3DComponent(World* world, const Collider3DComponentTemplate& tmp);

		/// If collider is registered it will be unregistered.
		~Collider3DComponent();


		// setters


		/// This function will replace current shape pointer in @see[Rigidbody3DComponentTemplate].
		/// Also shape will be replaced within Bullet Physics implementation.
		/// @param shape - pointer to shape that will be assigned to this collider
		/// @see Trigger3DComponent::GetShape
		void SetShape(const Physics3DShape& shape);


		// getters


		/// @return const reference to template containing current collider properties
		const Collider3DComponentTemplate& GetTemplate() const { return Template; }

		/// @return shape of this collider
		/// @see Trigger3DComponent::SetShape
		const Physics3DShape& GetShape() const { return *Template.Shape; }

		/// Collider may be registered and unregistered; if registered component is treated as active.
		/// @return if this component is registered then this function returns true
		bool IsRegistered() const { return Template.Registered; }

		bool IsPhysicsInitialized() const { return PhysicsInitialized; }

		/// Use to get collider collision group.
		/// Collision group determines in which collision group collider currently is.
		/// @return collision group of this collider
		/// @see Trigger3DComponent::SetCollisionGroup
		EnumFlags<eCollisionGroup> GetCollisionGroup() const { return Template.CollisionGroup; }

		/// Use to get collider collision mask.
		/// Collision mask determines whith which collision groups this collider will collide.
		/// @return collision mask of this collider
		/// @see Trigger3DComponent::SetCollisionMask
		EnumFlags<eCollisionGroup> GetCollisionMask() const { return Template.CollisionMask; }


		// other


		void UpdatePosition();


	private:
		bool PhysicsInitialized = false;
		/// Needed for destructuion and re-registration.
		/// @see Trigger3DComponent::~Trigger3DComponent
		World* BodyWorld;
		///
		/// It's easier to initialize collider when we can just copy construction data.
		Collider3DComponentTemplate Template;
		/// Thanks to this structure we don't need to declare implementation dependent structures in headers.
		std::unique_ptr<Trigger3DImpl> ImplData;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Collider3DComponent)
} // namespace Poly