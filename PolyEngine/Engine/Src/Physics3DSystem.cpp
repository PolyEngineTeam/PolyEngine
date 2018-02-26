#include "EnginePCH.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include "Rigidbody3DImpl.hpp"
#include "Collider3DImpl.hpp"
#include "Physics3DShapesImpl.hpp"
#include "Physics3DShapes.hpp"

	// TODO(squares): prevent cases where user removes one 
	// of components and adds a new one

//------------------------------------------------------------------------------
void Poly::Physics3DSystem::Physics3DUpdatePhase(World* world)
{
	// get physics world component and add telta time to delta overflow
	Physics3DWorldComponent* physicsWorldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	physicsWorldCmp->LastDeltaOverflow += (float)TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY);

	// update all bullet colliders and rigidbodies from engine tranforms
	for (auto tuple : world->IterateComponents<Collider3DComponent>())
	{
		Collider3DComponent* collider = std::get<Collider3DComponent*>(tuple);
		Rigidbody3DComponent* rigidbody = collider->GetSibling<Rigidbody3DComponent>();

		EnsureInit(world, collider->GetOwner());

		if (rigidbody && rigidbody->IsRegistered())
		{
			rigidbody->UpdatePosition();

			if (rigidbody->TemplateChanged)
			{
				btRigidBody* bulletRigidbody = rigidbody->ImplData->BulletRigidBody;
				Rigidbody3DComponentTemplate& tmp = rigidbody->Template;
				Vector v, u;

				v = tmp.Inertia.Value();
				bulletRigidbody->setMassProps(tmp.Mass, btVector3(v.X, v.Y, v.Z));

				v = tmp.Gravity.Value();
				bulletRigidbody->setGravity(btVector3(v.X, v.Y, v.Z));
				bulletRigidbody->setRestitution(tmp.Restitution);
				bulletRigidbody->setFriction(tmp.Friction);
				bulletRigidbody->setRollingFriction(tmp.RollingFriction);
				bulletRigidbody->setDamping(tmp.LinearDamping, tmp.AngularDamping);

				v = tmp.LinearFactor;
				bulletRigidbody->setLinearFactor(btVector3(v.X, v.Y, v.Z));

				v = tmp.AngularFactor;
				bulletRigidbody->setAngularFactor(btVector3(v.X, v.Y, v.Z));

				if (tmp.LinearVelocity.HasValue())
				{
					v = tmp.LinearVelocity.TakeValue();
					bulletRigidbody->setLinearVelocity(btVector3(v.X, v.Y, v.Z));
				}

				if (tmp.AngularVelocity.HasValue())
				{
					v = tmp.AngularVelocity.TakeValue();
					bulletRigidbody->setAngularVelocity(btVector3(v.X, v.Y, v.Z));
				}

				if (rigidbody->ImpulseToCenter.HasValue())
				{
					v = rigidbody->ImpulseToCenter.TakeValue();
					bulletRigidbody->applyCentralImpulse(btVector3(v.X, v.Y, v.Z));
				}

				if (rigidbody->Impulse.HasValue())
				{
					v = rigidbody->Impulse.TakeValue();
					u = rigidbody->ImpulsePos.TakeValue();
					bulletRigidbody->applyImpulse(btVector3(v.X, v.Y, v.Z), btVector3(u.X, u.Y, u.Z));
				}

				if (rigidbody->TorqueImpulse.HasValue())
				{
					v = rigidbody->TorqueImpulse.TakeValue();
					bulletRigidbody->applyTorqueImpulse(btVector3(v.X, v.Y, v.Z));
				}

				rigidbody->TemplateChanged = false;
			}
		}
		else if (collider->IsRegistered())
			collider->UpdatePosition();
	}

	// Update physics simulation, maintain fixed step
	while (physicsWorldCmp->LastDeltaOverflow >= physicsWorldCmp->Config.TimeStep)
	{
		physicsWorldCmp->LastDeltaOverflow -= physicsWorldCmp->Config.TimeStep;
		physicsWorldCmp->DynamicsWorld->stepSimulation(physicsWorldCmp->Config.TimeStep, 0);
	}

	// update all engine transform from bullet rigidbodies
	for (auto tuple : world->IterateComponents<Rigidbody3DComponent>())
	{
		Rigidbody3DComponent* rigidbody = std::get<Rigidbody3DComponent*>(tuple);

		// ship if rigidbody was not registered
		if (!rigidbody->IsRegistered())
			continue;

		EntityTransform& transform = rigidbody->GetTransform();

		btTransform trans;
		rigidbody->ImplData->BulletMotionState->getWorldTransform(trans);

		Vector localTrans;
		Quaternion localrot;

		localTrans.X = trans.getOrigin().getX();
		localTrans.Y = trans.getOrigin().getY();
		localTrans.Z = trans.getOrigin().getZ();
		localTrans.W = 1;

		localrot.X = trans.getRotation().getX();
		localrot.Y = trans.getRotation().getY();
		localrot.Z = trans.getRotation().getZ();
		localrot.W = trans.getRotation().getW();

		transform.SetGlobalTranslation(localTrans);
		transform.SetGlobalRotation(localrot);
	}
}

//------------------------------------------------------------------------------
Poly::Vector Poly::Physics3DSystem::CalculateInertia(const Physics3DShape* shape, float mass)
{
	btVector3 i;
	shape->ImplData->BulletShape->calculateLocalInertia(mass, i);

	return Vector(i.x(), i.y(), i.z());
}

//------------------------------------------------------------------------------
void Poly::Physics3DSystem::SetCollisionGroup(World* world, Entity* entity, EnumFlags<eCollisionGroup> group)
{
	// get template
	Collider3DComponent* cmp = world->GetComponent<Collider3DComponent>(entity);

	// check if this changes anything
	if (cmp->Template.CollisionGroup != group)
	{
		cmp->Template.CollisionGroup = group;

		// check if we have to re-register collider with new group and mask
		if (cmp->Template.Registered)
		{
			Physics3DSystem::UnregisterComponent(world, entity);
			Physics3DSystem::RegisterComponent(world, entity, world->GetComponent<Rigidbody3DComponent>(entity) != nullptr);
		}
	}
}

//------------------------------------------------------------------------------
void Poly::Physics3DSystem::SetCollisionMask(World* world, Entity* entity, EnumFlags<eCollisionGroup> mask)
{
	// get template
	Collider3DComponent* cmp = world->GetComponent<Collider3DComponent>(entity);

	// check if this changes anything
	if (cmp->Template.CollisionMask != mask)
	{
		cmp->Template.CollisionMask = mask;

		// check if we have to re-register collider with new group and mask

		if (cmp->Template.Registered)
		{
			Physics3DSystem::UnregisterComponent(world, entity);
			Physics3DSystem::RegisterComponent(world, entity, world->GetComponent<Rigidbody3DComponent>(entity) != nullptr);
		}
	}
}

//------------------------------------------------------------------------------
void Poly::Physics3DSystem::EnsureInit(World* world, Entity* entity)
{
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entity);
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entity);

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
		btCollisionShape* shape = collider->GetShape().ImplData->BulletShape;

		btVector3 inertia(0, 0, 0);

		switch (rigidbody->Template.RigidbodyType)
		{
		case eRigidBody3DType::STATIC:
			ASSERTE(Cmpf(rigidbody->Template.Mass, 0), "Can't create static body wit non zero mass");
			rigidbody->Template.Inertia = Vector(0, 0, 0);
			break;

		case eRigidBody3DType::DYNAMIC:
			ASSERTE(!Cmpf(rigidbody->Template.Mass, 0), "Can't create dynamic body with zero mass.");
			if (rigidbody->Template.Inertia.HasValue())
			{
				// if user has set inertia use provided
				Vector i = rigidbody->Template.Inertia.Value();
				inertia = btVector3(i.X, i.Y, i.Z);
			}
			else
			{
				// if inertia was not set then compute it from shape and mass and save it as current inertia
				shape->calculateLocalInertia(rigidbody->Template.Mass, inertia);
				rigidbody->Template.Inertia = Vector(inertia.x(), inertia.y(), inertia.z());
			}
			break;
		}

		// if gravity for this body was not set then set it as it is in the world
		if (!rigidbody->Template.Gravity.HasValue())
			rigidbody->Template.Gravity = world->GetWorldComponent<Physics2DWorldComponent>()->GetGravity();

		// create construction info
		btRigidBody::btRigidBodyConstructionInfo CI(rigidbody->Template.Mass, rigidbody->ImplData->BulletMotionState, shape, inertia);

		// create rigidbody
		btRigidBody* bulletRigidbody = rigidbody->ImplData->BulletRigidBody = new btRigidBody(CI);
		// assign just created rigidbody as collider in Collider3DComponent
		collider->ImplData->BulletTrigger = rigidbody->ImplData->BulletRigidBody;

		// FIXME(squares): use enablePhysics flag for registering
		if (rigidbody->Template.Registered)
		{
			rigidbody->Template.Registered = false;
			collider->Template.Registered = false;
			RegisterComponent(world, entity, true);
		}
		else
			collider->Template.Registered = false;

		if (!rigidbody->Template.Deactivatable)
			bulletRigidbody->setActivationState(DISABLE_DEACTIVATION);

		rigidbody->Initialized = true;
	}
	// or if we just have to initialize collider
	else
	{
		// break if this component has been already initialized or has sibling of type Rigidbody3DComponent
		if (collider->ImplData != nullptr)
			return;

		collider->ImplData = std::make_unique<Trigger3DImpl>();
		collider->ImplData->BulletTrigger = new btCollisionObject();
		collider->ImplData->BulletTrigger->setCollisionFlags(collider->ImplData->BulletTrigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		collider->ImplData->BulletTrigger->setCollisionShape(collider->Template.Shape->ImplData->BulletShape);

		// if component is registered at initialization that means it needs to be registered right now
		if (collider->Template.Registered)
			Physics3DSystem::RegisterComponent(world, entity, false);
	}

	collider->Initialized = true;
	rigidbody->Initialized = true;
}

//------------------------------------------------------------------------------
void Poly::Physics3DSystem::RegisterComponent(World* world, Entity* entity, bool enablePhysics)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entity);
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entity);

	//ASSERTE(!rigidbody->Template.Registered, "You cant register rigidbody when it is already registered.");
	//ASSERTE(!collider->Template.Registered, "You cant register rigidbody when collider is already registered.");
		// FIXME(squares): ucrtbase exception here

	if (enablePhysics)
	{
		worldCmp->DynamicsWorld->addRigidBody(rigidbody->ImplData->BulletRigidBody, 
			static_cast<int>(collider->Template.CollisionGroup), static_cast<int>(collider->Template.CollisionMask));
		worldCmp->BulletTriggerToEntity.insert(std::pair<const btCollisionObject*, Entity*>(rigidbody->ImplData->BulletRigidBody, entity));

		rigidbody->Template.Registered = true;
		collider->Template.Registered = true;
	}
	else
	{
		worldCmp->DynamicsWorld->addCollisionObject(collider->ImplData->BulletTrigger, 
			static_cast<int>(collider->Template.CollisionGroup), static_cast<int>(collider->Template.CollisionMask));
		worldCmp->BulletTriggerToEntity.insert(std::pair<const btCollisionObject*, Entity*>(collider->ImplData->BulletTrigger, entity));

		collider->Template.Registered = true;
	}

}

//------------------------------------------------------------------------------
void Poly::Physics3DSystem::UnregisterComponent(World* world, Entity* entity)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* rigidbody = world->GetComponent<Rigidbody3DComponent>(entity);
	Collider3DComponent* collider = world->GetComponent<Collider3DComponent>(entity);

	ASSERTE((rigidbody && rigidbody->Template.Registered) || collider->Template.Registered, "To unregister entity it must be registered first.");


	if (rigidbody && rigidbody->Template.Registered)
	{
		worldCmp->BulletTriggerToEntity.erase(rigidbody->ImplData->BulletRigidBody);
		worldCmp->DynamicsWorld->removeRigidBody(rigidbody->ImplData->BulletRigidBody);

		rigidbody->Template.Registered = false;
		collider->Template.Registered = false;
	}
	else
	{
		worldCmp->BulletTriggerToEntity.erase(collider->ImplData->BulletTrigger);
		worldCmp->DynamicsWorld->removeCollisionObject(collider->ImplData->BulletTrigger);

		collider->Template.Registered = false;
	}
}

//------------------------------------------------------------------------------
bool Poly::Physics3DSystem::IsColliding(World* world, Entity* firstEntity, Entity* secondEntity)
{
	ASSERTE(world->GetComponent<Collider3DComponent>(firstEntity)->Template.Registered && world->GetComponent<Collider3DComponent>(secondEntity)->Template.Registered, "One of the entities was not registered as trigger.");

	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();

	int numManifolds = worldCmp->Dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = worldCmp->Dispatcher->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		if ((worldCmp->BulletTriggerToEntity[obA] == firstEntity && worldCmp->BulletTriggerToEntity[obB] == secondEntity)
			|| (worldCmp->BulletTriggerToEntity[obA] == secondEntity && worldCmp->BulletTriggerToEntity[obB] == firstEntity))
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
Poly::ContactResult Poly::Physics3DSystem::ContactPair(World* world, Entity* firstEntity, Entity* secondEntity)
{
		// TODO(squares): implement this
	return ContactResult();
}

//------------------------------------------------------------------------------
Poly::ContactResult Poly::Physics3DSystem::Contact(World* world, Entity* entity)
{
		// TODO(squares): implement this
	return ContactResult();
}

//------------------------------------------------------------------------------
Poly::ContactPairResults Poly::Physics3DSystem::GetAllContactPairs(World* world)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();

	ContactPairResults results;

	int numManifolds = worldCmp->Dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = worldCmp->Dispatcher->getManifoldByIndexInternal(i);

		Vector normAvg;
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j<numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			btVector3 ptA = pt.getPositionWorldOnA();
			btVector3 ptB = pt.getPositionWorldOnB();
			Vector norm = Vector(ptA.x(), ptA.y(), ptA.z()) - Vector(ptB.x(), ptB.y(), ptB.z());
			normAvg += norm;
		}
		if (numContacts > 0)
		{
			normAvg /= (float)numContacts;
			normAvg.Normalize();
		}
			

		ContactPairResults::ContactPair pair;
		pair.FirstEntity = worldCmp->BulletTriggerToEntity[contactManifold->getBody0()];
		pair.SecondEntity = worldCmp->BulletTriggerToEntity[contactManifold->getBody1()];
		pair.Normal = normAvg;

		results.ContactPairs.PushBack(pair);
	}

	return results;
}

//------------------------------------------------------------------------------
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
		hit.HitEntity = worldCmp->BulletTriggerToEntity[r.m_collisionObjects[i]];
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

//------------------------------------------------------------------------------
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
		hit.HitEntity = worldCmp->BulletTriggerToEntity[r.m_collisionObject];
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
