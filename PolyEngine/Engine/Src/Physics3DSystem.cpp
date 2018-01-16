#include "EnginePCH.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include "Rigidbody3DImpl.hpp"
#include "Trigger3DImpl.hpp"

//********************************************************************************************************************************************
void Poly::Physics3DSystem::Physics3DUpdatePhase(World* world)
{
	// get physics world component and add telta time to delta overflow
		// FIXME(squares): bullet lags without loosing FPS 
	Physics3DWorldComponent* physicsWorldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	physicsWorldCmp->LastDeltaOverflow += (float)TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY);
	
	// update all bullet rigidbodies from engine tranforms
	for (auto tuple : world->IterateComponents<Rigidbody3DComponent, TransformComponent>())
	{
		Rigidbody3DComponent* rigidbody = std::get<Rigidbody3DComponent*>(tuple);
		rigidbody->EnsureInit();
		rigidbody->UpdatePosition();
	}
	
	// step simulation
	physicsWorldCmp->DynamicsWorld->stepSimulation((float)TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY), physicsWorldCmp->Config.MaxSimulationStepsPerFrame);
	
	// update all engine transform from bullet rigidbodies
	for (auto tuple : world->IterateComponents<Rigidbody3DComponent, TransformComponent>())
	{
		Rigidbody3DComponent* rigidbody = std::get<Rigidbody3DComponent*>(tuple);
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
void Poly::Physics3DSystem::RegisterRigidbody(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* cmp = world->GetComponent<Rigidbody3DComponent>(entityID);

	if (!cmp->Registered)
	{
		worldCmp->DynamicsWorld->addRigidBody(cmp->ImplData->BulletRigidBody);
		// see Physics3DSystem::AllHitsRaycast
		worldCmp->BulletTriggerToEntity.insert(std::pair<const btCollisionObject*, UniqueID>(cmp->ImplData->BulletRigidBody, entityID));

		cmp->Registered = true;
	}
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterRigidBody(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* cmp = world->GetComponent<Rigidbody3DComponent>(entityID);

	if (cmp->Registered)
	{
		worldCmp->BulletTriggerToEntity.erase(cmp->ImplData->BulletRigidBody);
		// see Physics3DSystem::AllHitsRaycast
		worldCmp->DynamicsWorld->removeRigidBody(cmp->ImplData->BulletRigidBody);

		cmp->Registered = false;
	}
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::RegisterTriger(World* world, const UniqueID& entityID, EnumFlags<eCollisionGroup> collisionGroup, EnumFlags<eCollisionGroup> collidesWith)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Trigger3DComponent* cmp = world->GetComponent<Trigger3DComponent>(entityID);

	if (!cmp->Registered)
	{
		worldCmp->DynamicsWorld->addCollisionObject(cmp->ImplData->BulletTrigger);
		// see Physics3DSystem::AllHitsRaycast
		worldCmp->BulletTriggerToEntity.insert(std::pair<const btCollisionObject*, UniqueID>(cmp->ImplData->BulletTrigger, entityID));

		cmp->Registered = true;
	}
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterTriger(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Trigger3DComponent* cmp = world->GetComponent<Trigger3DComponent>(entityID);

	if (cmp->Registered)
	{
		worldCmp->BulletTriggerToEntity.erase(cmp->ImplData->BulletTrigger);
		// see Physics3DSystem::AllHitsRaycast
		worldCmp->DynamicsWorld->removeCollisionObject(cmp->ImplData->BulletTrigger);

		cmp->Registered = true;
	}
}

//********************************************************************************************************************************************
bool Poly::Physics3DSystem::IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID)
{
	ASSERTE(world->GetComponent<Trigger3DComponent>(firstID)->Registered && world->GetComponent<Trigger3DComponent>(secondID)->Registered, "One of the entities was not registered as trigger.");

	return world->GetComponent<Trigger3DComponent>(firstID)->ImplData->BulletTrigger->checkCollideWith(world->GetComponent<Trigger3DComponent>(secondID)->ImplData->BulletTrigger);
}

//********************************************************************************************************************************************
const Poly::ContactResult& Poly::Physics3DSystem::ContactPair(World* world, const UniqueID& firstID, const UniqueID & secondID)
{
		// TODO(squares): implement this
	return ContactResult();
}

//********************************************************************************************************************************************
const Poly::ContactResult& Poly::Physics3DSystem::Contact(World* world, const UniqueID& entityID)
{
		// TODO(squares): implement this
	return ContactResult();
}

//********************************************************************************************************************************************
const Poly::RaycastResult& Poly::Physics3DSystem::AllHitsRaycast(World* world, const Vector& from, const Vector & to)
{
	RaycastResult result;

	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	// create ray
	btCollisionWorld::AllHitsRayResultCallback r(btVector3(from.X, from.Y, from.Z), btVector3(to.X, to.Y, to.Z));
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
const Poly::RaycastResult& Poly::Physics3DSystem::ClosestHitRaycast(World* world, const Vector& from, const Vector & to)
{
	RaycastResult result;

	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	// create ray
	btCollisionWorld::ClosestRayResultCallback r(btVector3(from.X, from.Y, from.Z), btVector3(to.X, to.Y, to.Z));
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
