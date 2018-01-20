#include "EnginePCH.hpp"

#include "InputSystem.hpp"

using namespace Poly;

void InputSystem::InputPhase(World* world)
{
	InputWorldComponent* com = world->GetWorldComponent<InputWorldComponent>();

	com->PrevKey = com->CurrKey;
	com->PrevMouseButton = com->CurrMouseButton;

	com->PrevMouse = com->CurrMouse;
	com->PrevWheel = com->CurrWheel;

    for (auto it=com->Controllers.begin(); it!=com->Controllers.end(); ++it)
    {
        it->second.PrevButton = it->second.CurrButton;
        it->second.PrevAxis = it->second.CurrAxis;
    }

	InputQueue& InputEventsQueue = gEngine->GetInputQueue();

	while (!InputEventsQueue.IsEmpty())
	{
		InputEvent& ev = InputEventsQueue.Front();
		switch (ev.Type)
		{
		case eInputEventType::KEYDOWN:
			if(ev.Key < eKey::_COUNT)
				com->CurrKey[ev.Key] = true;
			break;
		case eInputEventType::KEYUP:
			if(ev.Key < eKey::_COUNT)
				com->CurrKey[ev.Key] = false;
			break;
		case eInputEventType::MOUSEBUTTONDOWN:
			if(ev.MouseButton < eMouseButton::_COUNT)
				com->CurrMouseButton[ev.MouseButton] = true;
			break;
		case eInputEventType::MOUSEBUTTONUP:
			if(ev.MouseButton < eMouseButton::_COUNT)
				com->CurrMouseButton[ev.MouseButton] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			com->CurrMouse = ev.Pos;
			break;
		case eInputEventType::WHEELMOVE:
			com->CurrWheel += ev.Pos;
			break;
		case eInputEventType::CONTROLLERADDED:
            std::cout << "CONTROLLER ADDED" << std::endl;
			com->Controllers[ev.Controller] = Controller();
            com->ControllerPointers.PushBack(ev.Controller);
			break;
		case eInputEventType::CONTROLLERREMOVED:
			break;
		case eInputEventType::CONTROLLERBUTTONDOWN:
            std::cout << "BUTTON DOWN" << std::endl;
            std::cout << static_cast<int>(ev.ControllerButton) << std::endl;
            com->Controllers.at(ev.Controller).CurrButton[ev.ControllerButton] = true;
			break;
		case eInputEventType::CONTROLLERBUTTONUP:
            std::cout << "BUTTON UP" << std::endl;
            std::cout << static_cast<int>(ev.ControllerButton) << std::endl;
            com->Controllers.at(ev.Controller).CurrButton[ev.ControllerButton] = false;
			break;
		case eInputEventType::CONTROLLERAXIS:
            std::cout << "AXIS" << std::endl;
            std::cout << static_cast<int>(ev.ControllerButton) << std::endl;
            com->Controllers.at(ev.Controller).CurrAxis[ev.ControllerAxis] = ev.AxisValue;
			break;
		case eInputEventType::_COUNT:
			HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
			break;
		}
		InputEventsQueue.PopFront();
	}


}

