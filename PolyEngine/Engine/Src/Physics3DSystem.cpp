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
	world->GetWorldComponent<Physics3DWorldComponent>()->DynamicsWorld->addRigidBody(world->GetComponent<Rigidbody3DComponent>(entityID)->BulletRigidBody);
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterRigidBody(World* world, const UniqueID& entityID)
{
	world->GetWorldComponent<Physics3DWorldComponent>()->DynamicsWorld->removeRigidBody(world->GetComponent<Rigidbody3DComponent>(entityID)->BulletRigidBody);
}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::RegisterTriger(World* world, const UniqueID& entityID)
{
	//world->GetWorldComponent<Physics3DWorldComponent>()->DynamicsWorld->addCollisionObject(world->GetComponent<Rigidbody3DComponent>(entityID)->BulletRigidBody);

}

//********************************************************************************************************************************************
void Poly::Physics3DSystem::UnregisterTriger(World* world, const UniqueID& entityID)
{
	return void ENGINE_DLLEXPORT();
}
