#include "EnginePCH.hpp"

using namespace Poly;

void InputSystem::InputPhase(World* world)
{
	InputWorldComponent com = world->GetInputWorldComponent();

	com.PrevKey = com.CurrKey;

	com.PrevMouse = com.CurrMouse;
	com.PrevWheel = com.CurrWheel;

	InputQueue& InputEventsQueue = world->GetEngine()->GetInputQueue();

	while (!InputEventsQueue.Empty())
	{
		switch (InputEventsQueue.Front().Type)
		{
		case eInputEventType::KEYDOWN:
			gConsole.LogDebug("Keydown: {}", (size_t)InputEventsQueue.Front().Key);
			com.CurrKey[InputEventsQueue.Front().Key] = true;
			break;
		case eInputEventType::KEYUP:
			gConsole.LogDebug("Keyup {}", (size_t)InputEventsQueue.Front().Key);
			com.CurrKey[InputEventsQueue.Front().Key] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			gConsole.LogDebug("MousePosition: {}", InputEventsQueue.Front().Pos);
			com.CurrMouse = InputEventsQueue.Front().Pos;
			break;
		case eInputEventType::WHEELMOVE:
			gConsole.LogDebug("MouseWheelMove: {}", InputEventsQueue.Front().Pos.X);
			com.CurrWheel += InputEventsQueue.Front().Pos.X;
			break;
		case eInputEventType::_COUNT:
			HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
			break;
		}
		InputEventsQueue.Pop();
	}


}

