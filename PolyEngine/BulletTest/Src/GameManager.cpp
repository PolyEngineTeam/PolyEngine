#include "GameManager.hpp"

#include <TransformComponent.hpp>

#include "GameManagerWorldComponent.hpp"

void BT::GameManagerSystem::Update(Poly::World* world)
{
	GameManagerWorldComponent* gameManager = Poly::gEngine->GetWorld()->GetWorldComponent<GameManagerWorldComponent>();
	Poly::TransformComponent* stoneTransform = Poly::gEngine->GetWorld()->GetComponent<Poly::TransformComponent>(gameManager->Stone);

	Poly::gConsole.LogDebug("{}, {}, {}", stoneTransform->GetLocalTranslation().X, stoneTransform->GetLocalTranslation().Y, stoneTransform->GetLocalTranslation().Z);
}