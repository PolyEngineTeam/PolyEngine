#include "GameManagerSystem.hpp"

using namespace Poly;

void GameManagerSystem::GameManagerSystemPhase(World* world)
{

	GameManagerComponent* gameManager = nullptr;
	for (auto componentTuple : world->IterateComponents<GameManagerComponent>())
	{
		gameManager = std::get<GameManagerComponent*>(componentTuple);
	}

	/*for (auto componentTuple : world->IterateComponents<ScreenSpaceTextComponent>())
	{
		ScreenSpaceTextComponent* textComponent  = std::get<ScreenSpaceTextComponent*>(componentTuple);
		std::string text = "Kill count: ";
		text = text + std::to_string(gameManager->GetKillCount());
		textComponent->SetText(&text[0]);
	}*/
}


