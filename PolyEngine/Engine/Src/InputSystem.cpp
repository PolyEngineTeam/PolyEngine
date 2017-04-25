#include "InputSystem.hpp"

#include "World.hpp"
#include "InputWorldComponent.hpp"

using namespace Poly;

void InputSystem::InputPhase(World* world)
{
	InputWorldComponent com = world->GetInputWorldComponent();

	com.PrevKey = com.CurrKey;

	com.PrevMouse = com.CurrMouse;
	com.PrevWheel = com.CurrWheel;

	InputQueue InputEventsQueue = world->GetEngine()->GetInputQueue();

	while (!InputEventsQueue.Empty())
	{
		switch (InputEventsQueue.Front().Type)
		{
		case eInputEventType::KEYDOWN:
			com.CurrKey[InputEventsQueue.Front().Key] = true;
			break;
		case eInputEventType::KEYUP:
			com.CurrKey[InputEventsQueue.Front().Key] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			com.CurrMouse = InputEventsQueue.Front().Pos;
			break;
		case eInputEventType::WHEELMOVE:
			com.CurrWheel += InputEventsQueue.Front().Pos.X;
			break;
		}
		InputEventsQueue.Pop();
	}


}

