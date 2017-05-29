#include "GameManagerSystem.hpp"

using namespace Poly;

void GameManagerSystem::GameManagerSystemPhase(World* world)
{

}

void GameManagerSystem::AddEntity(World* world, UniqueID ent)
{
	for (auto gameManagerTuple : world->IterateComponents<GameManagerComponent>())
	{
		GameManagerComponent* gameManager = std::get<GameManagerComponent*>(gameManagerTuple);
		//gameManager->GetGameEntities()->push_back(ent);
	}
}

void GameManagerSystem::RemoveEntity(World* world, UniqueID ent)
{
	for (auto gameManagerTuple : world->IterateComponents<GameManagerComponent>())
	{
		GameManagerComponent* gameManager = std::get<GameManagerComponent*>(gameManagerTuple);
		//gameManager->GetGameEntities()->erase(ent);
	}
}

