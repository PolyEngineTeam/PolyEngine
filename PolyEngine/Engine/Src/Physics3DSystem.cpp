#include "EnginePCH.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

//********************************************************************************************************************************************
void Poly::Physics3DSystem::Physics3DUpdatePhase(World* world)
{
	Physics3DWorldComponent* physicsWorldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	physicsWorldCmp->LastDeltaOverflow += (float)TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY);

	for (auto tuple : world->IterateComponents<Rigidbody3DComponent, TransformComponent>())
	{
		Rigidbody3DComponent* rigidbody = std::get<Rigidbody3DComponent*>(tuple);
		rigidbody->UpdatePosition();
	}

	physicsWorldCmp->DynamicsWorld->stepSimulation((float)TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY), physicsWorldCmp->Config.MaxSimulationStepsPerFrame);

	for (auto tuple : world->IterateComponents<Rigidbody3DComponent, TransformComponent>())
	{
		Rigidbody3DComponent* rigidbody = std::get<Rigidbody3DComponent*>(tuple);
		TransformComponent* transform = std::get<TransformComponent*>(tuple);

		btTransform trans;
		rigidbody->BulletMotionState->getWorldTransform(trans);

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
void Poly::Physics3DSystem::RegisterRigidbody(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* cmp = world->GetComponent<Rigidbody3DComponent>(entityID);

	worldCmp->DynamicsWorld->addRigidBody(cmp->BulletRigidBody);
	worldCmp->BulletRigidbodyToEntity.Insert(cmp->BulletRigidBody, entityID);

	cmp->Registered = true;
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterRigidBody(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Rigidbody3DComponent* cmp = world->GetComponent<Rigidbody3DComponent>(entityID);

	worldCmp->BulletRigidbodyToEntity.Remove(cmp->BulletRigidBody);
	worldCmp->DynamicsWorld->removeRigidBody(cmp->BulletRigidBody);

	cmp->Registered = true;
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::RegisterTriger(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Trigger3DComponent* cmp = world->GetComponent<Trigger3DComponent>(entityID);

	worldCmp->DynamicsWorld->addCollisionObject(cmp->BulletTrigger);
	worldCmp->BulletTriggerToEntity.Insert(cmp->BulletTrigger, entityID);

	cmp->Registered = true;
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterTriger(World* world, const UniqueID& entityID)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();
	Trigger3DComponent* cmp = world->GetComponent<Trigger3DComponent>(entityID);

	worldCmp->DynamicsWorld->removeCollisionObject(cmp->BulletTrigger);
	worldCmp->BulletTriggerToEntity.Remove(cmp->BulletTrigger);

	cmp->Registered = true;
}

//********************************************************************************************************************************************
bool Poly::Physics3DSystem::IsColliding(World* world, const UniqueID& firstID, const UniqueID& secondID)
{
	ASSERTE(world->GetComponent<Trigger3DComponent>(firstID)->Registered && world->GetComponent<Trigger3DComponent>(secondID)->Registered, "One of the entities was not registered as trigger.");

	return world->GetComponent<Trigger3DComponent>(firstID)->BulletTrigger->checkCollideWith(world->GetComponent<Trigger3DComponent>(secondID)->BulletTrigger);
}

//********************************************************************************************************************************************
const Poly::ContactResult& Poly::Physics3DSystem::ContactPair(World* world, const UniqueID& firstID, const UniqueID & secondID)
{
	return ContactResult();
}

//********************************************************************************************************************************************
const Poly::ContactResult& Poly::Physics3DSystem::Contact(World* world, const UniqueID& entityID)
{
	return ContactResult();
}

//********************************************************************************************************************************************
const Poly::RaycastResult& Poly::Physics3DSystem::AllHitsRaycast(World* world, const Vector& from, const Vector & to)
{
	return RaycastResult();
}

//********************************************************************************************************************************************
const Poly::RaycastResult& Poly::Physics3DSystem::ClosestHitRaycast(World* world, const Vector& from, const Vector & to)
{
	Physics3DWorldComponent* worldCmp = world->GetWorldComponent<Physics3DWorldComponent>();

	RaycastResult result;

	btVector3 f = btVector3(from.X, from.Y, from.Z);
	btVector3 t = btVector3(to.X, to.Y, to.Z);
	btCollisionWorld::ClosestRayResultCallback r(f, t);

	world->GetWorldComponent<Physics3DWorldComponent>()->DynamicsWorld->rayTest(f, t, r);

	//UniqueID hitEntityID = worldCmp->;


	//result.Hits.PushBack({})
	return RaycastResult();
}
