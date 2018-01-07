#pragma once

#include <Vector.hpp>
#include <Dynarray.hpp>

namespace Poly
{
	//********************************************************************************************************************************************
	// helper structures


	// Contains dynarray of contacts. 
	// Dynarray has size 0 when none contact has been encountered.
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

	// Contains dynarray of hits. 
	// Dynarray has size 0 when none hit has been encountered.
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
		// Main update function for Physics3DSystem.
		// @param world - world to update
		// @see Physics3DWorldComponent
		// @see Trigger3DComponent
		// @see Rigidbody3DComponent
		void ENGINE_DLLEXPORT Physics3DUpdatePhase(World* world);


		//********************************************************************************************************************************************
		// registration


		// You can have @see[Rigidbody3DComponent] added to entity but till it's not registered 
		// it won't be considered in physics simulation (until You call @see[UnregisterRigidBody]).
		// @param world - world where exists component You want to register
		// @param entityID- @see[UniqueID] of entity that contains @see[Rigidbody3DComponent] You want to register
		// @see RegisterTriger
		void ENGINE_DLLEXPORT RegisterRigidbody(World* world, const UniqueID& entityID);

		// You can unregister your entity with this method so it won't be further
		// considered during the simulation ((until You call @see[RegisterRigidbody])
		// @param world - world where exists component You want to unregister
		// @param entityID- @see[UniqueID] of entity that contains @see[Rigidbody3DComponent] You want to unregister
		// @see UnregisterTriger
		void ENGINE_DLLEXPORT UnregisterRigidBody(World* world, const UniqueID& entityID);

			// TODO(squares): there are collision filter groups and masks; check that

		// You can have @see[Trigger3DComponent] added to entity but till it's not registered 
		// it won't be considered in physics simulation (until You call @see[UnregisterTriger]).
		// @param world - world where exists component You want to register
		// @param entityID- @see[UniqueID] of entity that contains @see[Trigger3DComponent] You want to register
		// @see RegisterRigidbody
		void ENGINE_DLLEXPORT RegisterTriger(World* world, const UniqueID& entityID);

		// You can unregister your entity with this method so it won't be further
		// considered during the simulation ((until You call @see[RegisterTriger])
		// @param world - world where exists component You want to unregister
		// @param entityID- @see[UniqueID] of entity that contains @see[Trigger3DComponent] You want to unregister
		// @see UnregisterRigidBody
		void ENGINE_DLLEXPORT UnregisterTriger(World* world, const UniqueID& entityID);


		//********************************************************************************************************************************************
		// collisions


			// TODO(squares): check if collisions and raycasts work also with ordinary rigid bodies

		// When You simply want to check if one entity is colliding with anther just call that function.
		// Both entities have to contain @see[Trigger3DComponent] or @see[Rigidbody3DComponent].
		// @param world - world with entities You want to test
		// @param firstID - first enitity
		// @param secondID - second enitity
		// @return colliding - true if given entities are colliding
		// @see ContactPair
		// @see Contact
		bool ENGINE_DLLEXPORT IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID);

		// When You want to get collision points You should use this function.
		// Both entities have to contain @see[Trigger3DComponent] or @see[Rigidbody3DComponent].
		// @param world - world with entities You want to test
		// @param firstID - first enitity
		// @param secondID - second enitity
		// @return result - instance of @see[ContactResult]
		// @see IsColliding
		// @see Contact
		const ENGINE_DLLEXPORT ContactResult& ContactPair(World* world, const UniqueID& firstID, const UniqueID& secondID);

		// With this function You can get all entities colliding with given entity.
		// Detects collision only if entity contains @see[Trigger3DComponent] or @see[Rigidbody3DComponent].
		// @param world - world with entities You want to test
		// @param entityID - enitity
		// @return result - instance of @see[ContactResult]
		// @see IsColliding
		// @see ContactPair
		const ENGINE_DLLEXPORT ContactResult& Contact(World* world, const UniqueID& entityID);


		//********************************************************************************************************************************************
		// raytracing


		// This function returns all hits encountered within one raycast.
		// Only entities with @see[Trigger3DComponent] or @see[Rigidbody3DComponent] will be considered.
		// @param world - world with entities You want to test
		// @param from - cast origin
		// @param to - cast destination
		// @return result - instance of @see[RaycastResult]
		// @see ClosestHitRaycast
		const ENGINE_DLLEXPORT RaycastResult& AllHitsRaycast(World* world, const Vector& from, const Vector& to);

		// This function returns only first hit encountered within one raycast.
		// Only entities with @see[Trigger3DComponent] or @see[Rigidbody3DComponent] will be considered.
		// @param world - world with entities You want to test
		// @param from - cast origin
		// @param to - cast destination
		// @return result - instance of @see[RaycastResult]
		// @see AllHitsRaycast
		const ENGINE_DLLEXPORT RaycastResult& ClosestHitRaycast(World* world, const Vector& from, const Vector& to);
	}
}
