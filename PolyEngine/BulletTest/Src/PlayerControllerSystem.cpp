#include "PlayerControllerSystem.hpp"

#include <Vector.hpp>
#include <Quaternion.hpp>
#include <TimeSystem.hpp>
#include <World.hpp>
#include <TransformComponent.hpp>
#include <InputWorldComponent.hpp>

#include "PlayerControllerComponent.hpp"
#include "GameManagerWorldComponent.hpp"

using namespace Poly;

void BT::PlayerControllerSystem::Update(World* world)
{
	float dt = Poly::TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
	InputWorldComponent* inputCmp = world->GetWorldComponent<InputWorldComponent>();
	PlayerControllerComponent* player = world->GetComponent<PlayerControllerComponent>(world->GetWorldComponent<GameManagerWorldComponent>()->Camera);
	TransformComponent* transform = world->GetComponent<TransformComponent>(world->GetWorldComponent<GameManagerWorldComponent>()->Camera);

	if (inputCmp->IsPressed(eMouseButton::RIGHT))
	{
		Vector2i delta = inputCmp->GetMousePosDelta();

		transform->SetLocalRotation(transform->GetLocalRotation() * Quaternion(Vector(1, 0, 0), Angle::FromDegrees(delta.Y / 4.f)));
		transform->SetLocalRotation(transform->GetLocalRotation() * Quaternion(transform->GetGlobalTransformationMatrix().GetInversed() * Vector(0, 1, 0, 0), Angle::FromDegrees(delta.X / 4.f)));
	}
}
