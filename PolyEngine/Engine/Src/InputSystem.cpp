#include "EnginePCH.hpp"

#include "InputSystem.hpp"

using namespace Poly;

void InputSystem::InputPhase(World* world)
{
	InputWorldComponent& com = world->GetInputWorldComponent();

	com.PrevKey = com.CurrKey;

	com.PrevMouse = com.CurrMouse;
	com.PrevWheel = com.CurrWheel;

	InputQueue& InputEventsQueue = world->GetEngine()->GetInputQueue();

	while (!InputEventsQueue.Empty())
	{
		InputEvent& ev = InputEventsQueue.Front();
		switch (ev.Type)
		{
		case eInputEventType::KEYDOWN:
			//gConsole.LogDebug("Keydown: {}", (size_t)ev.Key);
			com.CurrKey[ev.Key] = true;
			break;
		case eInputEventType::KEYUP:
			//gConsole.LogDebug("Keyup {}", (size_t)ev.Key);
			com.CurrKey[ev.Key] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			//gConsole.LogDebug("MousePosition: {}", ev.Pos);
			com.CurrMouse = ev.Pos;
			break;
		case eInputEventType::WHEELMOVE:
			//gConsole.LogDebug("MouseWheelMove: {}", ev.Pos.X);
			com.CurrWheel += (int)ev.Pos.X;
			break;
		case eInputEventType::_COUNT:
			HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
			break;
		}
		InputEventsQueue.Pop();
	}


}

