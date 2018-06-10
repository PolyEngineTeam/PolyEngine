#include "PolyEditorPCH.hpp"

#include "Time/TimeSystem.hpp"
#include "Input/InputWorldComponent.hpp"

void GizmoSystem::Update(World* world)
{
	Dynarray<Entity*> entities = gEngine->GetEditor()->GetSelectedEntities();
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));
	InputWorldComponent* inputCmp = world->GetWorldComponent<InputWorldComponent>();

	if (inputCmp->IsPressed(eKey::LCTRL))
	{
		// rotate
		if (inputCmp->IsPressed(eMouseButton::MIDDLE))
		{
			if (inputCmp->IsPressed(eKey::KEY_1))
			{

			}
			else if (inputCmp->IsPressed(eKey::KEY_2))
			{

			}
			else if (inputCmp->IsPressed(eKey::KEY_3))
			{

			}
		}
		// move
		else if (inputCmp->IsPressed(eMouseButton::RIGHT))
		{
			if (inputCmp->IsPressed(eKey::KEY_1))
			{

			}
			else if (inputCmp->IsPressed(eKey::KEY_2))
			{

			}
			else if (inputCmp->IsPressed(eKey::KEY_3))
			{

			}
		}
	}
}
