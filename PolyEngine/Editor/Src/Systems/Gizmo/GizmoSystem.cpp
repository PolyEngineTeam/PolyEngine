#include "PolyEditorPCH.hpp"

#include "Time/TimeSystem.hpp"
#include "Input/InputWorldComponent.hpp"

void GizmoSystem::Update(Scene* scene)
{
	std::vector<Entity*> entities = static_cast<PolyEditor::IEditor*>(gEngine->GetEditor())->GetSelectedEntities();
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(scene, Poly::eEngineTimer::GAMEPLAY));
	InputWorldComponent* inputCmp = scene->GetWorldComponent<InputWorldComponent>();
	float delta = (float)inputCmp->GetMousePosDelta().Y;

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
			Vector move;

			if (inputCmp->IsPressed(eKey::KEY_1))
				move += Vector::UNIT_X;
			else if (inputCmp->IsPressed(eKey::KEY_2))
				move -= Vector::UNIT_Y;
			else if (inputCmp->IsPressed(eKey::KEY_3))
				move += Vector::UNIT_Z;

			move *= delta * deltaTime;
		
			for (Entity* entity : entities)
				entity->GetTransform().SetGlobalTranslation(entity->GetTransform().GetGlobalTranslation() + move);
		}
	}
}
