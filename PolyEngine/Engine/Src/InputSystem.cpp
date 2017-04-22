#include "InputSystem.hpp"

#include "World.hpp"
#include "InputWorldComponent.hpp"

using namespace Poly;

void InputSystem::InputPhase(World* world)
{
	InputWorldComponent* com = world->GetInputWorldComponent();

	for (int i = 0; i < 256; i++)
		com->PrevKey[i] = com->CurrKey[i];

	//memcpy(&com->PrevKey, &com->CurrKey, 256 * sizeof(bool));

	com->PrevMouse = com->CurrMouse;
	com->PrevWheel = com->CurrWheel;

	while (!InputEventsQueue->Empty())
	{
		switch (InputEventsQueue->Front().Type)
		{
		case eInputEventType::KEYDOWN:
			com->CurrKey[static_cast<int>(InputEventsQueue->Front().Key)] = true;
			break;
		case eInputEventType::KEYUP:
			com->CurrKey[static_cast<int>(InputEventsQueue->Front().Key)] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			com->CurrMouse = InputEventsQueue->Front().Pos;
			break;
		case eInputEventType::WHEELMOVE:
			com->CurrWheel += InputEventsQueue->Front().Pos.X;
			break;
		}
		InputEventsQueue->Pop();
	}
}

