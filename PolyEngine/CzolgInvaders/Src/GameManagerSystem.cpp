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
	float y_pos = gEngine->GetRenderingContext()->GetScreenSize().Height;
	y_pos *= 1.95f;
	textComponent->SetScreenPosition(Vector{0.0f, y_pos, 0.0f});
	std::string text = "Kill count: ";
	text = text + std::to_string(gameManager->GetKillCount());
	textComponent->SetText(&text[0]);
}


