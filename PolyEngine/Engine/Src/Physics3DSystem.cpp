#include "EnginePCH.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include "Rigidbody3DImpl.hpp"
#include "Collider3DImpl.hpp"

//********************************************************************************************************************************************
void Poly::Physics3DSystem::Physics3DUpdatePhase(World* world)
{
	// get physics world component and add telta time to delta overflow
	Physics3DWorldComponent* physicsWorldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	physicsWorldCmp->LastDeltaOverflow += (float)TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY) * physicsWorldCmp->Config.TimeFactor;
	
	// update all bullet rigidbodies from engine tranforms
	for (auto tuple : world->IterateComponents<Rigidbody3DComponent, TransformComponent>())
	{
		Rigidbody3DComponent* rigidbody = std::get<Rigidbody3DComponent*>(tuple);
	
		// skip if rigidbody was not registered
		if (!rigidbody->IsRegistered())
			continue;
	
		Physics3DSystem::EnsureInit(world, rigidbody->GetOwnerID());
		rigidbody->UpdatePosition();
	}

	// Update physics simulation, maintain fixed step
	while (physicsWorldCmp->LastDeltaOverflow >= physicsWorldCmp->Config.TimeStep)
	{
		physicsWorldCmp->LastDeltaOverflow -= physicsWorldCmp->Config.TimeStep;
		physicsWorldCmp->DynamicsWorld->stepSimulation(physicsWorldCmp->Config.TimeStep, 0);
	}
	
	// update all engine transform from bullet rigidbodies
	for (auto tuple : world->IterateComponents<Rigidbody3DComponent, TransformComponent>())
	{
		Rigidbody3DComponent* rigidbody = std::get<Rigidbody3DComponent*>(tuple);
	
		// ship if rigidbody was not registered
		if (!rigidbody->IsRegistered())
			continue;
	
		TransformComponent* transform = std::get<TransformComponent*>(tuple);
	
		btTransform trans;
		rigidbody->ImplData->BulletMotionState->getWorldTransform(trans);
	
		Vector localTrans = transform->GetLocalTranslation();
		Quaternion localrot = transform->GetLocalRotation().ToEulerAngles();
	
		localTrans.X = trans.getOrigin().getX();
		localTrans.Y = trans.getOrigin().getY();
		localTrans.Z = trans.getOrigin().getZ();
	
		localrot.X = trans.getRotation().getX();
		localrot.Y = trans.getRotation().getY();
		localrot.Z = trans.getRotation().getZ();
		localrot.W = trans.getRotation().getW();
	
		transform->SetLocalTranslation(localTrans);
		transform->SetLocalRotation(localrot);
	}
}

//********************************************************************************************************************************************
Poly::Vector Poly::Physics3DSystem::CalculateIntertia(const Physics3DShape& shape, float mass)
{
	btVector3 i;
	shape.BulletShape->calculateLocalInertia(mass, i);

	return Vector(i.x(), i.y(), i.z());
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::SetCollisionGroup(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> group)
{
	// get template
	Collider3DComponentTemplate* tmp = world->GetComponent<Collider3DComponent>(entityID)->Template.get();

	// check if this changes anything
	if (tmp->CollisionGroup != group)
	{
		tmp->CollisionGroup = group;

		// check if we have to re-register collider with new group and mask
		if (tmp->Registered)
		{
			// if we got rigidbody we have to register rigidbody otherwise we can do this for collider
			if (world->GetComponent<Rigidbody3DComponent>(entityID) != nullptr)
			{
				Physics3DSystem::UnregisterRigidBody(world, entityID);
				Physics3DSystem::RegisterRigidbody(world, entityID, tmp->CollisionGroup, tmp->CollisionMask);
			}
			else
			{
				Physics3DSystem::UnregisterCollider(world, entityID);
				Physics3DSystem::RegisterCollider(world, entityID, tmp->CollisionGroup, tmp->CollisionMask);
			}
		}
	}
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::SetCollisionMask(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> mask)
{
	// get template
	Collider3DComponentTemplate* tmp = world->GetComponent<Collider3DComponent>(entityID)->Template.get();

	// check if this changes anything
	if (tmp->CollisionMask != mask)
	{
		tmp->CollisionMask = mask;

		// check if we have to re-register collider with new group and mask

		if (tmp->Registered)
		{
			// if we got rigidbody we have to register rigidbody otherwise we can do this for collider
			if (world->GetComponent<Rigidbody3DComponent>(entityID) != nullptr)
			{
				Physics3DSystem::UnregisterRigidBody(world, entityID);
				Physics3DSystem::RegisterRigidbody(world, entityID, tmp->CollisionGroup, tmp->CollisionMask);
			}
			else
			{
				Physics3DSystem::UnregisterCollider(world, entityID);
				Physics3DSystem::RegisterCollider(world, entityID, tmp->CollisionGroup, tmp->CollisionMask);
			}
		}
	}
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::EnsureInit(World* world, const UniqueID& entityID)
{
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entityID);
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entityID);

	ASSERTE(collider != nullptr, "Entity doesn't contain collider.");

	// if we got rigid body here
	if (rigidbody != nullptr)
	{
		// break if Bullet Physics rigidbody exists
		if (rigidbody->ImplData != nullptr)
			return;

		// create unique impl data and motion state
		rigidbody->ImplData = std::make_unique<Rigidbody3DImpl>();
		collider->ImplData = std::make_unique<Trigger3DImpl>();
		rigidbody->ImplData->BulletMotionState = new btDefaultMotionState();

		// get collider shape
		btCollisionShape* shape = collider->GetShape().BulletShape;

		btVector3 inertia(0, 0, 0);

		switch (rigidbody->Template->RigidbodyType)
		{
		case eRigidBody3DType::STATIC:
			// TODO(squares): check if that's enough
			break;

		case eRigidBody3DType::DYNAMIC:
			// FIXME(squares): this is't proper way of preventing zero mass
			ASSERTE(rigidbody->Template->Mass > 0, "Can't create dynamic body with 0 mass.");
			shape->calculateLocalInertia(rigidbody->Template->Mass, inertia);
			break;

		case eRigidBody3DType::DYNAMIC_CUSTOM_INTERTIA:
			Vector i = rigidbody->Template->Intertia;
			inertia = btVector3(i.X, i.Y, i.Z);
			break;
		}

		// create construction info
		btRigidBody::btRigidBodyConstructionInfo CI(rigidbody->Template->Mass, rigidbody->ImplData->BulletMotionState, shape, inertia);

		// create rigidbody
		rigidbody->ImplData->BulletRigidBody = new btRigidBody(CI);
		// assign just created rigidbody as collider in Collider3DComponent
		collider->ImplData->BulletTrigger = rigidbody->ImplData->BulletRigidBody;

		if (rigidbody->Template->Registered)
		{
			Physics3DSystem::RegisterRigidbody(world, entityID, collider->Template->CollisionGroup, collider->Template->CollisionMask);
			collider->Template->Registered = true;
		}
		else
		{
			collider->Template->Registered = false;
		}
	}
	// or if we just have to initialize collider
	else
	{
		// break if this component has been already initialized or has sibling of type Rigidbody3DComponent
		if (collider->ImplData != nullptr)
			return;

		collider->ImplData->BulletTrigger = new btCollisionObject();
		collider->ImplData->BulletTrigger->setCollisionFlags(collider->ImplData->BulletTrigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		collider->ImplData->BulletTrigger->setCollisionShape(collider->Template->Shape->BulletShape);

		// if component is registered at initialization that means it needs to be registered right now
		if (collider->Template->Registered)
			Physics3DSystem::RegisterCollider(world, entityID, collider->Template->CollisionGroup, collider->Template->CollisionMask);
	}
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::RegisterRigidbody(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entityID);
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entityID);

		// FIXME(squares): ucrtbase exception here
	//ASSERTE(!rigidbody->Template->Registered, "You cant register rigidbody when it is already registered.");
	//ASSERTE(!collider->Template->Registered, "You cant register rigidbody when collider it is already registered.");

	collider->Template->CollisionGroup = collisionGroup;
	collider->Template->CollisionMask = collidesWith;
	worldCmp->DynamicsWorld->addRigidBody(rigidbody->ImplData->BulletRigidBody, static_cast<int>(collisionGroup), static_cast<int>(collidesWith));
	// see Physics3DSystem::AllHitsRaycast
	worldCmp->BulletTriggerToEntity.insert(std::pair<const btCollisionObject*, UniqueID>(rigidbody->ImplData->BulletRigidBody, entityID));

	rigidbody->Template->Registered = true;
	collider->Template->Registered = true;
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterRigidBody(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entityID);
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entityID);

	ASSERTE(rigidbody->Template->Registered, "To unregister rigidbody it must be registered first.");

	worldCmp->BulletTriggerToEntity.erase(rigidbody->ImplData->BulletRigidBody);
	// see Physics3DSystem::AllHitsRaycast
	worldCmp->DynamicsWorld->removeRigidBody(rigidbody->ImplData->BulletRigidBody);

	rigidbody->Template->Registered = false;
	collider->Template->Registered = false;
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::RegisterCollider(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entityID);
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entityID);

	ASSERTE(!rigidbody, "You can't register collider when it has rigidbody sibling.");
	ASSERTE(!collider->Template->Registered, "You cant register rigidbody when it is already registered.");
	
	collider->Template->CollisionGroup = collisionGroup;
	collider->Template->CollisionMask = collidesWith;
	worldCmp->DynamicsWorld->addCollisionObject(collider->ImplData->BulletTrigger, static_cast<int>(collisionGroup), static_cast<int>(collidesWith));
	// see Physics3DSystem::AllHitsRaycast
	worldCmp->BulletTriggerToEntity.insert(std::pair<const btCollisionObject*, UniqueID>(collider->ImplData->BulletTrigger, entityID));

	collider->Template->Registered = true;
	
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterCollider(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entityID);
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entityID);

	ASSERTE(!rigidbody, "You can't unregister collider when it has rigidbody sibling.");
	ASSERTE(collider->Template->Registered, "To unregister collider it must be registered first.");

	worldCmp->BulletTriggerToEntity.erase(collider->ImplData->BulletTrigger);
	// see Physics3DSystem::AllHitsRaycast
	worldCmp->DynamicsWorld->removeCollisionObject(collider->ImplData->BulletTrigger);

	collider->Template->Registered = false;
}

//********************************************************************************************************************************************
bool Poly::Physics3DSystem::IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID)
{
	ASSERTE(world->GetComponent<Collider3DComponent>(firstID)->Template->Registered && world->GetComponent<Collider3DComponent>(secondID)->Template->Registered, "One of the entities was not registered as trigger.");

	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();

	int numManifolds = worldCmp->Dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = worldCmp->Dispatcher->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		if ((worldCmp->BulletTriggerToEntity[obA] == firstID && worldCmp->BulletTriggerToEntity[obB] == secondID)
			|| (worldCmp->BulletTriggerToEntity[obA] == secondID && worldCmp->BulletTriggerToEntity[obB] == firstID))
		{
			return true;
		}
	}

	return false;
}

//********************************************************************************************************************************************
Poly::ContactResult Poly::Physics3DSystem::ContactPair(World* world, const UniqueID& firstID, const UniqueID& secondID)
{
		// TODO(squares): implement this
	return ContactResult();
}

//********************************************************************************************************************************************
Poly::ContactResult Poly::Physics3DSystem::Contact(World* world, const UniqueID& entityID)
{
		// TODO(squares): implement this
	return ContactResult();
}

//********************************************************************************************************************************************
Poly::RaycastResult Poly::Physics3DSystem::AllHitsRaycast(World* world, const Vector& from, const Vector& to, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith)
{
	RaycastResult result;

	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	// create ray
	btCollisionWorld::AllHitsRayResultCallback r(btVector3(from.X, from.Y, from.Z), btVector3(to.X, to.Y, to.Z));
	r.m_collisionFilterGroup = static_cast<int>(collisionGroup);
	r.m_collisionFilterMask = static_cast<int>(collidesWith);
	// call raytest
	worldCmp->DynamicsWorld->rayTest(r.m_rayFromWorld, r.m_rayToWorld, r);

	for (int i = 0; i < r.m_collisionObjects.size(); ++i)
	{
		RaycastResult::RaycastHit hit;

			// FIXME(squares): try catch something?
		// get UniqueID
		hit.HitEntityID = worldCmp->BulletTriggerToEntity[r.m_collisionObjects[i]];
		// get fraction
		hit.HitFraction = r.m_hitFractions[i];
		// get normal
		btVector3 n = r.m_hitNormalWorld[i];
		hit.WorldHitNormal = Vector(n.x(), n.y(), n.z());
		// get point
		n = r.m_hitPointWorld[i];
		hit.WorldHitPoint = Vector(n.x(), n.y(), n.z());

		// push hit
		result.Hits.PushBack(hit);
	}

	return result;
}

//********************************************************************************************************************************************
Poly::RaycastResult Poly::Physics3DSystem::ClosestHitRaycast(World* world, const Vector& from, const Vector& to, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith)
{
	RaycastResult result;

	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	// create ray
	btCollisionWorld::ClosestRayResultCallback r(btVector3(from.X, from.Y, from.Z), btVector3(to.X, to.Y, to.Z));
	r.m_collisionFilterGroup = static_cast<int>(collisionGroup);
	r.m_collisionFilterMask = static_cast<int>(collidesWith);
	// call raytest
	worldCmp->DynamicsWorld->rayTest(r.m_rayFromWorld, r.m_rayToWorld, r);

	if (r.hasHit())
	{
		RaycastResult::RaycastHit hit;

			// FIXME(squares): try catch something?
		// get UniqueID
		hit.HitEntityID = worldCmp->BulletTriggerToEntity[r.m_collisionObject];
		// get fraction
		hit.HitFraction = r.m_closestHitFraction;
		// get normal
		hit.WorldHitNormal = Vector(r.m_hitNormalWorld.x(), r.m_hitNormalWorld.y(), r.m_hitNormalWorld.z());
		// get point
		hit.WorldHitPoint = Vector(r.m_hitPointWorld.x(), r.m_hitPointWorld.y(), r.m_hitPointWorld.z());

		// push hit
		result.Hits.PushBack(hit);
	}

	return result;
}
