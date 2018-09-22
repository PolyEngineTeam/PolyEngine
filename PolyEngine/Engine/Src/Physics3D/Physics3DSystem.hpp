#pragma once

#include <Defines.hpp>
#include <ECS/Scene.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	// helper structures


	class Physics3DShape;


	enum class eCollisionGroup
	{
		NOTHING = 0x00,
		TRIGGER = 0x01,
		RIGIDBODY = 0x02,
		RIGIDBODY_RED = 0x04,
		RIGIDBODY_GREEN = 0x08,
		RIGIDBODY_BLUE = 0x16,
		TRIGGER_RED = 0x32,
		TRIGGER_GREEN = 0x64,
		TRIGGER_BLUE = 0x128,
	};

	struct ENGINE_DLLEXPORT ContactPairResults
	{
		struct  ContactPair
		{
			Entity* FirstEntity;
			Entity* SecondEntity;
			Vector Normal;
		};

		Dynarray<ContactPair> ContactPairs;
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
			Entity* HitEntity;
			Vector WorldHitNormal;
			Vector WorldHitPoint;
			float HitFraction;
		};

		Dynarray<RaycastHit> Hits;
	};


	//------------------------------------------------------------------------------
	// system namespace


	namespace Physics3DSystem
	{
		/// Main update function for Physics3DSystem.
		/// @param world - world to update
		/// @see Physics3DWorldComponent
		/// @see Trigger3DComponent
		/// @see Rigidbody3DComponent
		void ENGINE_DLLEXPORT Physics3DUpdatePhase(Scene* world);


		/// Use this when you need proper intertia for shape with mass.
		/// @param shape - shape for intertia computation
		/// @param mass - mass of object with given shape
		/// @return three dimensional vector containing intertia for given shape with mass
		/// @see Rigidbody3DComponentTemplate
		Vector ENGINE_DLLEXPORT CalculateInertia(const Physics3DShape* shape, float mass);


		// collision groups administration


		/// Use to change collider collision group.
		/// Collision group determines in which collision group collider currently is.
		/// @param group - new collision group
		/// @see Trigger3DComponent::GetCollisionGroup
		/// @see Trigger3DComponent::SetCollisionMask
		void ENGINE_DLLEXPORT SetCollisionGroup(Scene* world, Entity* entity, EnumFlags<eCollisionGroup> group);

		/// Use to change collider collision mask.
		/// Collision mask determines whith which collision groups this collider will collide.
		/// @param mask - new collision mask
		/// @see Trigger3DComponent::GetCollisionMask
		/// @see Trigger3DComponent::SetCollisionGroup
		void ENGINE_DLLEXPORT SetCollisionMask(Scene* world, Entity* entity, EnumFlags<eCollisionGroup> mask);


		// deferred administration

		/// The actual collider or rigibody is created during the first Physics3DSystem::Update call after creation of that component.
		/// @see Physics3DSystem::Physics3DUpdatePhase
		void ENGINE_DLLEXPORT EnsureInit(Scene* world, Entity* entity);


		// registration


		void ENGINE_DLLEXPORT RegisterComponent(Scene* world, Entity* entity, bool enablePhysics);

		/// You can unregister your entity with this method so it won't be further
		/// considered during the simulation ((until You call @see[RegisterColliderComponent])
		/// @param world - world where exists component You want to unregister
		/// @param entityID- @see[UniqueID] of entity that contains @see[Trigger3DComponent] You want to unregister
		/// @see UnregisterRigidBody
		void ENGINE_DLLEXPORT UnregisterComponent(Scene* world, Entity* entity);


		// collisions


		/// When You want to get collision points You should use this function.
		/// Both entities have to contain @see[Trigger3DComponent] or @see[Rigidbody3DComponent].
		/// @param world - world with entities You want to test
		/// @param firstID - first enitity
		/// @param secondID - second enitity
		/// @return result - instance of @see[ContactResult]
		/// @see IsColliding
		/// @see Contact
		ENGINE_DLLEXPORT ContactResult ContactPair(Scene* world, Entity* firstEntity, Entity* secondEntity);


		ENGINE_DLLEXPORT ContactPairResults GetAllContactPairs(Scene* world);

		// raytracing

		/// This function returns all hits encountered within one raycast.
		/// Only entities with @see[Trigger3DComponent] or @see[Rigidbody3DComponent] will be considered.
		/// @param world - world with entities You want to test
		/// @param from - cast origin
		/// @param to - cast destination
		/// @return result - instance of @see[RaycastResult]
		/// @see ClosestHitRaycast
		ENGINE_DLLEXPORT RaycastResult AllHitsRaycast(Scene* world, const Vector& from, const Vector& to, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith);

		/// This function returns only first hit encountered within one raycast.
		/// Only entities with @see[Trigger3DComponent] or @see[Rigidbody3DComponent] will be considered.
		/// @param world - world with entities You want to test
		/// @param from - cast origin
		/// @param to - cast destination
		/// @return result - instance of @see[RaycastResult]
		/// @see AllHitsRaycast
		ENGINE_DLLEXPORT RaycastResult ClosestHitRaycast(Scene* world, const Vector& from, const Vector& to, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith);
	}
} // namespace poly
