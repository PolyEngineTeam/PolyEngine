#include "EnginePCH.hpp"

#include "InputSystem.hpp"

using namespace Poly;

void InputSystem::InputPhase(World* world)
{
	InputWorldComponent* com = world->GetWorldComponent<InputWorldComponent>();

	com->PrevKey = com->CurrKey;

	com->PrevMouse = com->CurrMouse;
	com->PrevWheel = com->CurrWheel;

	InputQueue& InputEventsQueue = gEngine->GetInputQueue();

	while (!InputEventsQueue.IsEmpty())
	{
		InputEvent& ev = InputEventsQueue.Front();
		switch (ev.Type)
		{
		case eInputEventType::KEYDOWN:
			com->CurrKey[ev.Key] = true;
			break;
		case eInputEventType::KEYUP:
			com->CurrKey[ev.Key] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			com->CurrMouse = ev.Pos;
			break;
		case eInputEventType::WHEELMOVE:
			com->CurrWheel += (int)ev.Pos.X;
			break;
		case eInputEventType::_COUNT:
			HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
			break;
		}
		InputEventsQueue.PopFront();
	}


}

