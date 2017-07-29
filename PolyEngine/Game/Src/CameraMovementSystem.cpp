#include "CameraMovementSystem.hpp"

#include <World.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>

#include "GameManagerWorldComponent.hpp"

using namespace Poly;
using namespace SGJ;

void SGJ::CameraMovementSystem::CameraMovementUpdatePhase(Poly::World* world)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	UniqueID player = gameMgrCmp->Player;
	TransformComponent* playerTransCmp = world->GetComponent<TransformComponent>(player);

	for (auto& tuple : world->IterateComponents<CameraComponent, TransformComponent>())
	{
		CameraComponent* cameraCmp = std::get<CameraComponent*>(tuple);
		TransformComponent* transformCmp = std::get<TransformComponent*>(tuple);

		Vector desiredPosition;
		desiredPosition.Y = 0.5f;
		desiredPosition.Z = 8.0f / Tan(cameraCmp->GetFOV() / 2.0f);

		float sideSpan = 8.0f * cameraCmp->GetAspect() - 0.5f;
		float minW = gameMgrCmp->MinLevelWidth + sideSpan;
		float maxW = gameMgrCmp->MaxLevelWidth - sideSpan;
		desiredPosition.X = Clamp(playerTransCmp->GetGlobalTranslation().X, minW, maxW);

		transformCmp->SetLocalTranslation(desiredPosition);
	}
}
