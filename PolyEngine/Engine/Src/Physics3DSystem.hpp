#pragma once

#include <Vector.hpp>
#include <Dynarray.hpp>
#include <EnumUtils.hpp>

#include "Physics3DShapes.hpp"

namespace Poly
{
	//********************************************************************************************************************************************
	// helper structures


	enum class eCollisionGroup
	{
		TRIGGER = 0x01,
		RIGIDBODY = 0x02,
		RIGIDBODY_GREEN = 0x04,
		RIGIDBODY_RED = 0x08,
		TRIGGER_GREEN = 0x16,
		TRIGGER_RED = 0x32,
	};

	/// Contains dynarray of contacts. 
	/// Dynarray has size 0 when none contact has been encountered.
	struct ENGINE_DLLEXPORT ContactResult
	{
		struct Contact
		{
			UniqueID HitEntityID;
			Vector WorldHitNormal;
			Vector WorldHitPoint;
			float HitFraction;
		};

		Dynarray<Contact> Contacts;
	};

	/// Contains dynarray of hits. 
	/// Dynarray has size 0 when none hit has been encountered.
	struct ENGINE_DLLEXPORT RaycastResult
	{
		struct RaycastHit
		{
			UniqueID HitEntityID;
			Vector WorldHitNormal;
			Vector WorldHitPoint;
			float HitFraction;
		};

		Dynarray<RaycastHit> Hits;
	};


	//********************************************************************************************************************************************
	// system namespace


	namespace Physics3DSystem
	{
		/// Main update function for Physics3DSystem.
		/// @param world - world to update
		/// @see Physics3DWorldComponent
		/// @see Trigger3DComponent
		/// @see Rigidbody3DComponent
		void ENGINE_DLLEXPORT Physics3DUpdatePhase(World* world);


		/// Use this when you need proper intertia for shape with mass.
		/// @param shape - shape for intertia computation
		/// @param mass - mass of object with given shape
		/// @return three dimensional vector containing intertia for given shape with mass
		/// @see Rigidbody3DComponentTemplate
		const Vector& CalculateIntertia(const Physics3DShape& shape, float mass);


		//********************************************************************************************************************************************
		// collision groups administration


		/// Use to change collider collision group.
		/// Collision group determines in which collision group collider currently is.
		/// @param group - new collision group
		/// @see Trigger3DComponent::GetCollisionGroup
		/// @see Trigger3DComponent::SetCollisionMask
		void SetCollisionGroup(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> group);

		/// Use to change collider collision mask.
		/// Collision mask determines whith which collision groups this collider will collide.
		/// @param mask - new collision mask
		/// @see Trigger3DComponent::GetCollisionMask
		/// @see Trigger3DComponent::SetCollisionGroup
		void SetCollisionMask(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> mask);


		//********************************************************************************************************************************************
		// deferred administration

		/// The actual collider or rigibody is created during the first Physics3DSystem::Update call after creation of that component.
		/// @see Physics3DSystem::Physics3DUpdatePhase
		void EnsureInit(World* world, const UniqueID& entityID);


		//********************************************************************************************************************************************
		// registration


		/// You can have @see[Rigidbody3DComponent] added to entity but till it's not registered 
		/// it won't be considered in physics simulation (until You call @see[UnregisterRigidBody]).
		/// WARNING - undefined behaviour when you register collider and then register rigidbody
		/// @param world - world where exists component You want to register
		/// @param entityID- @see[UniqueID] of entity that contains @see[Rigidbody3DComponent] You want to register
		/// @see RegisterTriger
		void ENGINE_DLLEXPORT RegisterRigidbody(World* world, const UniqueID& entityID, 
			EnumFlags<eCollisionGroup> colliderGroup = eCollisionGroup::RIGIDBODY, EnumFlags<eCollisionGroup> collidesWith = eCollisionGroup::RIGIDBODY);

		/// You can unregister your entity with this method so it won't be further
		/// considered during the simulation ((until You call @see[RegisterRigidbody])
		/// @param world - world where exists component You want to unregister
		/// @param entityID- @see[UniqueID] of entity that contains @see[Rigidbody3DComponent] You want to unregister
		/// @see UnregisterTriger
		void ENGINE_DLLEXPORT UnregisterRigidBody(World* world, const UniqueID& entityID);

		/// You can have @see[Trigger3DComponent] added to entity but till it's not registered 
		/// it won't be considered in physics simulation (until You call @see[UnregisterTriger]).
		/// @param world - world where exists component You want to register
		/// @param entityID- @see[UniqueID] of entity that contains @see[Trigger3DComponent] You want to register
		/// @see RegisterRigidbody
		void ENGINE_DLLEXPORT RegisterCollider(World* world, const UniqueID& entityID,
			EnumFlags<eCollisionGroup> colliderGroup = eCollisionGroup::TRIGGER, EnumFlags<eCollisionGroup> collidesWith = eCollisionGroup::TRIGGER | eCollisionGroup::RIGIDBODY);

		/// You can unregister your entity with this method so it won't be further
		/// considered during the simulation ((until You call @see[RegisterTriger])
		/// @param world - world where exists component You want to unregister
		/// @param entityID- @see[UniqueID] of entity that contains @see[Trigger3DComponent] You want to unregister
		/// @see UnregisterRigidBody
		void ENGINE_DLLEXPORT UnregisterCollider(World* world, const UniqueID& entityID);


		//********************************************************************************************************************************************
		// collisions


		/// When You simply want to check if one entity is colliding with anther just call that function.
		/// Both entities have to contain @see[Trigger3DComponent] or @see[Rigidbody3DComponent].
		/// @param world - world with entities You want to test
		/// @param firstID - first enitity
		/// @param secondID - second enitity
		/// @return colliding - true if given entities are colliding
		/// @see ContactPair
		/// @see Contact
		bool ENGINE_DLLEXPORT IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID);

		/// When You want to get collision points You should use this function.
		/// Both entities have to contain @see[Trigger3DComponent] or @see[Rigidbody3DComponent].
		/// @param world - world with entities You want to test
		/// @param firstID - first enitity
		/// @param secondID - second enitity
		/// @return result - instance of @see[ContactResult]
		/// @see IsColliding
		/// @see Contact
		const ENGINE_DLLEXPORT ContactResult& ContactPair(World* world, const UniqueID& firstID, const UniqueID& secondID);

		/// With this function You can get all entities colliding with given entity.
		/// Detects collision only if entity contains @see[Trigger3DComponent] or @see[Rigidbody3DComponent].
		/// @param world - world with entities You want to test
		/// @param entityID - enitity
		/// @return result - instance of @see[ContactResult]
		/// @see IsColliding
		/// @see ContactPair
		const ENGINE_DLLEXPORT ContactResult& Contact(World* world, const UniqueID& entityID);


		//********************************************************************************************************************************************
		// raytracing


		/// This function returns all hits encountered within one raycast.
		/// Only entities with @see[Trigger3DComponent] or @see[Rigidbody3DComponent] will be considered.
		/// @param world - world with entities You want to test
		/// @param from - cast origin
		/// @param to - cast destination
		/// @return result - instance of @see[RaycastResult]
		/// @see ClosestHitRaycast
		const ENGINE_DLLEXPORT RaycastResult& AllHitsRaycast(World* world, const Vector& from, const Vector& to);

		/// This function returns only first hit encountered within one raycast.
		/// Only entities with @see[Trigger3DComponent] or @see[Rigidbody3DComponent] will be considered.
		/// @param world - world with entities You want to test
		/// @param from - cast origin
		/// @param to - cast destination
		/// @return result - instance of @see[RaycastResult]
		/// @see AllHitsRaycast
		const ENGINE_DLLEXPORT RaycastResult& ClosestHitRaycast(World* world, const Vector& from, const Vector& to);
	}
} // namespace poly
