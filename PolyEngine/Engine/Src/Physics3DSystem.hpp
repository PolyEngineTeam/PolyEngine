#pragma once

#include <Vector.hpp>
#include <Dynarray.hpp>

namespace Poly
{
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

	namespace Physics3DSystem
	{
		void ENGINE_DLLEXPORT Physics3DUpdatePhase(World* world);

		void ENGINE_DLLEXPORT RegisterRigidbody(World* world, const UniqueID& entityID);
		void ENGINE_DLLEXPORT UnregisterRigidBody(World* world, const UniqueID& entityID);

		// TODO(squares): there are collision filter groups and masks; check that
		void ENGINE_DLLEXPORT RegisterTriger(World* world, const UniqueID& entityID);
		void ENGINE_DLLEXPORT UnregisterTriger(World* world, const UniqueID& entityID);

		// TODO(squares: check if collisions and raycasts work also with ordinary rigid bodies
		bool ENGINE_DLLEXPORT IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID);
		const ENGINE_DLLEXPORT ContactResult& ContactPair(World* world, const UniqueID& firstID, const UniqueID& secondID);
		const ENGINE_DLLEXPORT ContactResult& Contact(World* world, const UniqueID& entityID);

		const ENGINE_DLLEXPORT RaycastResult& AllHitsRaycast(World* world, const Vector& from, const Vector& to);
		const ENGINE_DLLEXPORT RaycastResult& ClosestHitRaycast(World* world, const Vector& from, const Vector& to);
	}
}
