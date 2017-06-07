#include "GameManagerSystem.hpp"

using namespace Poly;

void GameManagerSystem::GameManagerSystemPhase(World* world)
{

	GameManagerComponent* gameManager = nullptr;
	for (auto componentTuple : world->IterateComponents<GameManagerComponent>())
	{
		gameManager = std::get<GameManagerComponent*>(componentTuple);
	}

	ScreenSpaceTextComponent* textComponent = world->GetComponent<ScreenSpaceTextComponent>(gameManager->GetKillCounter());
	std::string text = "Kill count: ";
	text = text + std::to_string(gameManager->GetKillCount());
	textComponent->SetText(&text[0]);
}


