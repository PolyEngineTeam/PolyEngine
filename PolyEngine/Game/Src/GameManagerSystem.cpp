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
#include "TileComponent.hpp"
#include "PlayerControllerComponent.hpp"
#include "Physics2DWorldComponent.hpp"
#include <SoundEmitterComponent.hpp>
#include <SoundSystem.hpp>

using namespace SGJ;
using namespace Poly;


void SGJ::GameManagerSystem::LoadLevel(Poly::World* world, const Poly::String& path)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	gameMgrCmp->Levels.PushBack(new Level(path));
}

void SGJ::GameManagerSystem::Update(Poly::World* world)
{
	GameManagerWorldComponent* manager = world->GetWorldComponent<GameManagerWorldComponent>();
	
	// Proces sound entities
	for (size_t i=0; i < manager->SoundSampleEntities.GetSize();)
	{
		UniqueID soundEnt = manager->SoundSampleEntities[i];
		if (!SoundSystem::IsEmmiterActive(world, soundEnt))
		{
			manager->SoundSampleEntities.RemoveByIdx(i);
			DeferredTaskSystem::DestroyEntity(world, soundEnt);
		}
		else
			++i;
	}

	// Proces triggers
	for (Physics2DWorldComponent::Collision col : world->GetWorldComponent<Physics2DWorldComponent>()->GetCollidingBodies(world->GetComponent<RigidBody2DComponent>(manager->Player)))
	{
		TileComponent* obstacle = col.rb->GetSibling<TileComponent>();
		PlayerControllerComponent* playerCmp = world->GetComponent<PlayerControllerComponent>(manager->Player);

		if (!obstacle) continue;

		switch (obstacle->GetTileType())
		{
		case eTileType::SPIKESBOTTOM:
		case eTileType::SPIKESTOP:
		case eTileType::SPIKESLEFT:
		case eTileType::SPIKESRIGHT:
		{
			PlayerUpdateSystem::KillPlayer(world);
			return;
		}
		case eTileType::PLAYERENDPOS:
		{
			manager->CurrentLevelID = (manager->CurrentLevelID + 1) % manager->LevelEntities.GetSize();
			SpawnLevel(world, manager->CurrentLevelID);
			return;
		}
		break;
		case eTileType::FASTERCHARACTER:
			playerCmp->SetActivePowerup(ePowerup::INCREASED_SPEED);
			break;
		case eTileType::HIGHJUMP:
			playerCmp->SetActivePowerup(ePowerup::HIGH_JUMP);
			break;
		case eTileType::LARGERCHARACTER:
			playerCmp->SetActivePowerup(ePowerup::INCREASED_SIZE);
			break;
		case eTileType::SMALLERCHARACTER:
			playerCmp->SetActivePowerup(ePowerup::DECREASED_SIZE);
			break;
		case eTileType::POGOJUMP:
			playerCmp->SetActivePowerup(ePowerup::POGO_JUMP);
			break;
		case eTileType::REVERSEDCONTROLS:
			playerCmp->SetActivePowerup(ePowerup::REVERSED_CONTROLS);
			break;
		case eTileType::LOWJUMP:
			playerCmp->SetActivePowerup(ePowerup::LOW_JUMP);
			break;
		case eTileType::INVERSEDGRAVITY:
			playerCmp->SetActivePowerup(ePowerup::INVERSED_GRAVITY);
			break;
		default:
			break;
		}
	}
}

Poly::UniqueID GameManagerSystem::CreateTileObject(Poly::World* world, const Poly::Vector& position, eTileType tileType, String meshSource, eRigidBody2DType physicsProperties = eRigidBody2DType::STATIC, const Vector& size = Vector(1, 1, 1), const Color& color = Color(0, 0, 0))
{
	UniqueID tile = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, tile);
	DeferredTaskSystem::AddComponentImmediate<TileComponent>(world, tile, tileType);
	DeferredTaskSystem::AddComponentImmediate<Box2DColliderComponent>(world, tile, size * 2);
	DeferredTaskSystem::AddComponentImmediate<RigidBody2DComponent>(world, tile, world, physicsProperties, tileType == eTileType::RIGIDBODYGROUND ? 0.5f : 1.0f);

	TransformComponent* tileTrans = world->GetComponent<TransformComponent>(tile);

	UniqueID mesh = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, mesh);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, mesh, meshSource, eResourceSource::GAME, color);
	TransformComponent* meshTrans = world->GetComponent<TransformComponent>(mesh);
	meshTrans->SetParent(tileTrans);
	meshTrans->SetLocalRotation(Quaternion(Vector::UNIT_X, 90_deg));
	switch (tileType)
	{
	case eTileType::SPIKESBOTTOM:
		break;
	case eTileType::SPIKESTOP:
		meshTrans->SetLocalRotation(Quaternion(Vector::UNIT_Z, 180_deg));
		break;
	case eTileType::SPIKESLEFT:
		meshTrans->SetLocalRotation(Quaternion(Vector::UNIT_Z, -90_deg));
		break;
	case eTileType::SPIKESRIGHT:
		meshTrans->SetLocalRotation(Quaternion(Vector::UNIT_Z, 90_deg));
		break;
	}
	meshTrans->SetLocalScale(size);

	tileTrans->SetLocalTranslation(position);
	return tile;
}

Poly::UniqueID GameManagerSystem::SpawnPlayer(Poly::World* world, const Poly::Vector& position)
{
	UniqueID player = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, player);
	DeferredTaskSystem::AddComponentImmediate<PlayerControllerComponent>(world, player);
	PlayerControllerComponent* p = world->GetComponent<PlayerControllerComponent>(player);
	p->SpawnPoint = position;
	TransformComponent* playerTrans = world->GetComponent<Poly::TransformComponent>(player);

	DeferredTaskSystem::AddComponentImmediate<Poly::Circle2DColliderComponent>(world, player, 0.4);
	DeferredTaskSystem::AddComponentImmediate<Poly::RigidBody2DComponent>(world, player, world, eRigidBody2DType::DYNAMIC, 1.0f, 0.5f);
	Poly::RigidBody2DComponent* rigidBody = world->GetComponent<Poly::RigidBody2DComponent>(player);
	rigidBody->SetDamping(3);
	rigidBody->SetRotationDamping(10);

	UniqueID body = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, body);
	Poly::TransformComponent* bodyTrans = world->GetComponent<Poly::TransformComponent>(body);
	bodyTrans->SetParent(playerTrans);
	bodyTrans->SetLocalRotation(Quaternion(Vector::UNIT_X, 90_deg));
	Vector correctedSize = Vector(0.8, 0.8, 0.0);
	correctedSize.Z = 0.8f;
	bodyTrans->SetLocalScale(correctedSize);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, body, "Quad.obj", eResourceSource::GAME, Color(0, 1, 0));

	UniqueID playerLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, playerLight);
	Poly::TransformComponent* lightTrans = world->GetComponent<Poly::TransformComponent>(playerLight);
	lightTrans->SetParent(playerTrans);
	lightTrans->SetLocalTranslation(Vector(0.0, 0.0, 5.0));
	DeferredTaskSystem::AddComponentImmediate<PointLightSourceComponent>(world, playerLight, Color(0.0, 1.0, 0.0, 1.0), 1000.0f, 100.0f);

	playerTrans->SetLocalTranslation(position);
	return player;
}

void SGJ::GameManagerSystem::SpawnLevel(Poly::World* world, size_t idx)
{
	// cleanup previous level
	DespawnLevel(world);

	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	Level* level = gameMgrCmp->Levels[idx];

	gameMgrCmp->CurrentLevelID = idx;

	// calculate level center
	gameMgrCmp->MinLevelWidth = level->Width;
	gameMgrCmp->MaxLevelWidth = 0;
	gameMgrCmp->MinLevelHeight = level->Height;
	gameMgrCmp->MaxLevelHeight = 0;
	float meanW = 0, meanH = 0;
	size_t count = 0;
	for (size_t idx = 0; idx < level->Tiles.GetSize(); ++idx)
	{
		if (level->Tiles[idx] != SGJ::eTileType::NOTHING)
		{
			size_t w = (idx % level->Width);
			size_t h = (idx / level->Width);

			if (w < gameMgrCmp->MinLevelWidth)
				gameMgrCmp->MinLevelWidth = w;
			if (w > gameMgrCmp->MaxLevelWidth)
				gameMgrCmp->MaxLevelWidth = w;

			if (h < gameMgrCmp->MinLevelHeight)
				gameMgrCmp->MinLevelHeight = h;
			if (h > gameMgrCmp->MaxLevelHeight)
				gameMgrCmp->MaxLevelHeight = h;
		}
	}
	meanW = (gameMgrCmp->MaxLevelWidth - gameMgrCmp->MinLevelWidth + 1)/2;
	meanH = (gameMgrCmp->MaxLevelHeight - gameMgrCmp->MinLevelHeight + 1)/2;

	gameMgrCmp->MinLevelWidth -= meanW;
	gameMgrCmp->MaxLevelWidth -= meanW;
	gameMgrCmp->MinLevelHeight -= meanH;
	gameMgrCmp->MaxLevelHeight -= meanH;

	// spawn level tiles
	for (int idx = 0; idx < level->Tiles.GetSize(); ++idx)
	{
		if (level->Tiles[idx] != SGJ::eTileType::NOTHING)
		{
			int posW = (idx % level->Width) - meanW;
			int posH = (idx / level->Width) - meanH;

			switch (level->Tiles[idx])
			{
			case eTileType::PLAYERSTARTPOS:
				if (gameMgrCmp->Player)   
					PlayerUpdateSystem::ResetPlayer(world, Vector(posW, -posH, 0));
				else
					gameMgrCmp->Player = SpawnPlayer(world, Vector(posW, -posH, 0));
				break;
			case eTileType::PLAYERENDPOS:
				gameMgrCmp->LevelEntities.PushBack(CreateTileObject(world, Vector(posW, -posH, 0), level->Tiles[idx], "Models/cube.fbx", eRigidBody2DType::STATIC, Vector(0.5, 0.5, 0.5), Color(0, 0, 1)));
				break;

			case eTileType::STATICGROUND:
			case eTileType::RIGIDBODYGROUND:
				gameMgrCmp->LevelEntities.PushBack(CreateTileObject(world, Vector(posW, -posH, 0), level->Tiles[idx], "Models/cube.fbx",
					level->Tiles[idx] == eTileType::STATICGROUND ? eRigidBody2DType::STATIC : eRigidBody2DType::DYNAMIC, 
					level->Tiles[idx] == eTileType::STATICGROUND ? Vector(0.5, 0.5, 0.5) : Vector(0.4, 0.4, 0.4),
					level->Tiles[idx] == eTileType::STATICGROUND ? Color(0, 0, 0) : Color(0.5, 0.5, 0.5)));
				break;

			case eTileType::SPIKESBOTTOM:
			case eTileType::SPIKESTOP:
			case eTileType::SPIKESLEFT:
			case eTileType::SPIKESRIGHT:
				gameMgrCmp->LevelEntities.PushBack(CreateTileObject(world, Vector(posW, -posH, 0), level->Tiles[idx], "Models/spikes.fbx", eRigidBody2DType::STATIC, Vector(0.5, 0.5, 0.25), Color(1, 0, 0)));
				break;

			default:
				gameMgrCmp->LevelEntities.PushBack(CreateTileObject(world, Vector(posW, -posH, 0), level->Tiles[idx], "Models/cube.fbx", eRigidBody2DType::STATIC, Vector(0.5, 0.5, 0.5), Color(1, 0, 1)));
				break;
			}
		}
	}
}

void SGJ::GameManagerSystem::DespawnLevel(Poly::World* world)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	for (int i = 0; i < gameMgrCmp->LevelEntities.GetSize(); i++)
	{
		DeferredTaskSystem::DestroyEntityImmediate(world, gameMgrCmp->LevelEntities[i]);
		gameMgrCmp->LevelEntities.RemoveByIdx(i);
	}

}

void SGJ::GameManagerSystem::PlaySample(Poly::World* world, const String& file, const Vector& position, float pitch, float gain)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();

	UniqueID id = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, id);
	Poly::TransformComponent* trans = world->GetComponent<Poly::TransformComponent>(id);
	trans->SetLocalTranslation(position);
	DeferredTaskSystem::AddComponentImmediate<Poly::SoundEmitterComponent>(world, id, file, eResourceSource::GAME);
	
	SoundSystem::SetEmitterFrequency(world, id, pitch);
	SoundSystem::SetEmitterGain(world, id, gain);
	SoundSystem::PlayEmitter(world, id);
	gameMgrCmp->SoundSampleEntities.PushBack(id);
}

void SGJ::GameManagerSystem::PrepareNonlevelObjects(Poly::World * world)
{
	// Spawn entities
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	gameMgrCmp->Camera = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(gEngine->GetWorld(), gameMgrCmp->Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(gEngine->GetWorld(), gameMgrCmp->Camera, 60_deg, 1.0f, 1000.f);

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

	UniqueID backgroundPlayer = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<SoundEmitterComponent>(world, backgroundPlayer, "Audio/Reloaded_installer_1.ogg", eResourceSource::GAME);
	SoundSystem::PlayEmitter(world, backgroundPlayer);
	SoundSystem::LoopEmitter(world, backgroundPlayer);

	UniqueID id = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(gEngine->GetWorld(), id);
	Poly::TransformComponent* lightTrans = gEngine->GetWorld()->GetComponent<Poly::TransformComponent>(id);
	lightTrans->SetLocalRotation(Quaternion(Vector::UNIT_Y, -15_deg));
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightSourceComponent>(gEngine->GetWorld(), id, Color(1,0,1), 100.0f);
	gameMgrCmp->OtherEntities.PushBack(id);
}

void SGJ::GameManagerSystem::Cleanup(Poly::World* world)
{
	DespawnLevel(world);
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	for (auto ent : gameMgrCmp->OtherEntities)
		DeferredTaskSystem::DestroyEntityImmediate(world, ent);

	DeferredTaskSystem::DestroyEntityImmediate(world, gameMgrCmp->Player);
	DeferredTaskSystem::DestroyEntityImmediate(world, gameMgrCmp->Camera);
}