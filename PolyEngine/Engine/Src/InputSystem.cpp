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
        {
            SDL_GameController *controller = SDL_GameControllerOpen(ev.ControllerID);
            com->Controllers[controller] = Controller();
            bool controllerAssigned = false;
            for (auto it = com->ControllerPointers.Begin(); it != com->ControllerPointers.End(); ++it) {
                if (*it == nullptr) {
                    *it = controller;
                    controllerAssigned = true;
                    break;
                }
            }
            if (!controllerAssigned) {
                com->ControllerPointers.PushBack(controller);
            }
            break;
        }
		case eInputEventType::CONTROLLERREMOVED:
            for (auto it=com->ControllerPointers.Begin(); it!=com->ControllerPointers.End(); ++it)
            {
                SDL_GameController* controllerPtr = *it;
                SDL_Joystick* jptr = SDL_GameControllerGetJoystick(controllerPtr);
                Sint32 joystickID = SDL_JoystickInstanceID(jptr);
                if(joystickID == ev.ControllerID) {
                    SDL_GameControllerClose(controllerPtr);
                    com->Controllers.erase(controllerPtr);
                    *it = nullptr;
                    break;
                }
            }
            ASSERTE(true, "Could not remove controller");
			break;
		case eInputEventType::CONTROLLERBUTTONDOWN:
        {
            std::cout << static_cast<int>(ev.ControllerButton) << std::endl;
            SDL_GameController *controller = SDL_GameControllerFromInstanceID(ev.ControllerID);
            com->Controllers.at(controller).CurrButton[ev.ControllerButton] = true;
            break;
        }
		case eInputEventType::CONTROLLERBUTTONUP:
        {
            std::cout << static_cast<int>(ev.ControllerButton) << std::endl;
            SDL_GameController *controller = SDL_GameControllerFromInstanceID(ev.ControllerID);
            com->Controllers.at(controller).CurrButton[ev.ControllerButton] = false;
            break;
        }
		case eInputEventType::CONTROLLERAXIS:
        {
            std::cout << static_cast<int>(ev.ControllerButton) << std::endl;
            SDL_GameController *controller = SDL_GameControllerFromInstanceID(ev.ControllerID);
            com->Controllers.at(controller).CurrAxis[ev.ControllerAxis] = ev.AxisValue;
            break;
        }
		case eInputEventType::_COUNT:
			HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
			break;
		}
		InputEventsQueue.PopFront();
	}


}

