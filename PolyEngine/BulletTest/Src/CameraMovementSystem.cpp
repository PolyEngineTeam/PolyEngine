#include "CameraMovementSystem.hpp"

#include <World.hpp>
#include <InputWorldComponent.hpp>
#include <InputSystem.hpp>
#include <TransformComponent.hpp>

#include "GameManagerWorldComponent.hpp"

using namespace Poly;

void BT::CameraMovementSystem::Update(World* world)
{
	InputWorldComponent* inputCmp = world->GetWorldComponent<InputWorldComponent>();

	if (inputCmp->IsPressed(eMouseButton::RIGHT))
	{
		Vector2i delta = inputCmp->GetMousePosDelta();
		TransformComponent* camera = world->GetComponent<TransformComponent>(world->GetWorldComponent<GameManagerWorldComponent>()->Camera);

		Quaternion xRot(Vector(1, 0, 0), Angle::FromDegrees(0));
		Quaternion yRot(Vector(0, 1, 0), Angle::FromDegrees(delta.X / 4.f));
		Quaternion zRot(Vector(0, 0, 1), Angle::FromDegrees(0));

		Quaternion rotation = xRot * yRot * zRot; 

		camera->SetLocalRotation(camera->GetLocalRotation() * rotation);
	}
}