#include "GameManagerSystem.hpp"

#include "GameManagerWorldComponent.hpp"
#include "Level.hpp"

#include <DeferredTaskSystem.hpp>
#include <TransformComponent.hpp>
#include <Physics2DColliders.hpp>
#include <RigidBody2DComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <CameraComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <LightSourceComponent.hpp>

using namespace SGJ;
using namespace Poly;


void SGJ::GameManagerSystem::LoadLevel(Poly::World* world, const Poly::String& path)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	gameMgrCmp->Levels.PushBack(new Level(path));
}

UniqueID SGJ::GameManagerSystem::CreateTileObject(World* world, const Vector& position, const Vector& size, const Color& color, eRigidBody2DType type)
{
	UniqueID obj1 = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, obj1);
	Poly::TransformComponent* obj1Trans = world->GetComponent<Poly::TransformComponent>(obj1);

	DeferredTaskSystem::AddComponentImmediate<Poly::Box2DColliderComponent>(world, obj1, size);
	DeferredTaskSystem::AddComponentImmediate<Poly::RigidBody2DComponent>(world, obj1, world, type);

	UniqueID subobj1 = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, subobj1);
	Poly::TransformComponent* subobj1Trans = world->GetComponent<Poly::TransformComponent>(subobj1);
	subobj1Trans->SetParent(obj1Trans);
	subobj1Trans->SetLocalRotation(Quaternion(Vector::UNIT_X, 90_deg));
	Vector correctedSize = size;
	correctedSize.Z = 1.0f;
	subobj1Trans->SetLocalScale(correctedSize);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, subobj1, "Quad.obj", eResourceSource::GAME, color);

	obj1Trans->SetLocalTranslation(position);
	return obj1;
}

void SGJ::GameManagerSystem::SpawnLevel(Poly::World* world, size_t idx)
{
	// cleanup previous level
	DespawnLevel(world);
	
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	Level* level = gameMgrCmp->Levels[idx];

	// calculate level center
	size_t meanW = 0, meanH = 0;
	size_t count = 0;
	for (size_t idx = 0; idx < level->Tiles.GetSize(); ++idx)
	{
		if (level->Tiles[idx] != SGJ::eTileType::NOTHING)
		{
			meanW += (idx % 100);
			meanH += (idx / 100);
			++count;
		}
	}
	meanW /= count;
	meanH /= count;

	// spawn level tiles
	for (int idx = 0; idx < level->Tiles.GetSize(); ++idx)
	{
		if (level->Tiles[idx] != SGJ::eTileType::NOTHING)
		{
			int posW = (idx % 100) - meanW;
			int posH = (idx / 100) - meanH;
			UniqueID id = CreateTileObject(world, Vector(posW, posH, 0), Vector(1, 1, 0), Color(0, 1, 0), eRigidBody2DType::STATIC);
			gameMgrCmp->LevelEntities.PushBack(id);
		}
	}
}

void SGJ::GameManagerSystem::DespawnLevel(Poly::World* world)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	for (auto ent : gameMgrCmp->LevelEntities)
		DeferredTaskSystem::DestroyEntityImmediate(world, ent);
}

void SGJ::GameManagerSystem::PrepareNonlevelObjects(Poly::World * world)
{
	// Spawn entities
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	gameMgrCmp->Camera = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(gEngine->GetWorld(), gameMgrCmp->Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(gEngine->GetWorld(), gameMgrCmp->Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(gEngine->GetWorld(), gameMgrCmp->Camera, 10.0f, 0.003f);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = gEngine->GetWorld()->GetComponent<Poly::TransformComponent>(gameMgrCmp->Camera);
	cameraTrans->SetLocalTranslation(Vector(0, 0, 50.f));

	// Set background 
	//double Time = gEngine->GetWorld()->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime();
	//Background = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	//DeferredTaskSystem::AddComponentImmediate<Poly::BackgroundComponent>(gEngine->GetWorld(), Background, Time);
	//GameManagerComponent* gameManagerComponent = Engine->GetWorld()->GetComponent<GameManagerComponent>(GameManager);
	//gEngine->RegisterGameUpdatePhase(BackgroundSystem::BackgroundSystemSystemPhase);

	// SETUP SCENE HERE

	// For physics testing
	for (int i = 0; i < 100; ++i)
	{
		UniqueID id = SGJ::GameManagerSystem::CreateTileObject(gEngine->GetWorld(), Vector((i * 2) % 20, 20.0f + (i % 20), 0), Vector(1, 1, 0), Color(1, 0, 0), eRigidBody2DType::DYNAMIC);
		gameMgrCmp->OtherEntities.PushBack(id);
	}

	UniqueID id = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(gEngine->GetWorld(), id);
	Poly::TransformComponent* lightTrans = gEngine->GetWorld()->GetComponent<Poly::TransformComponent>(id);
	//lightTrans->SetLocalRotation(Quaternion(Vector::UNIT_Y, 180_deg));
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightSourceComponent>(gEngine->GetWorld(), id, Color(1,0,0), 1.0f);
	gameMgrCmp->OtherEntities.PushBack(id);

	UniqueID czolg = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(gEngine->GetWorld(), czolg);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(gEngine->GetWorld(), czolg, "model-tank/tank.fbx", eResourceSource::GAME);
}

void SGJ::GameManagerSystem::Cleanup(Poly::World* world)
{
	DespawnLevel(world);
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	for (auto ent : gameMgrCmp->OtherEntities)
		DeferredTaskSystem::DestroyEntityImmediate(world, ent);

	//DeferredTaskSystem::DestroyEntityImmediate(world, gameMgrCmp->Player);
	DeferredTaskSystem::DestroyEntityImmediate(world, gameMgrCmp->Camera);
}