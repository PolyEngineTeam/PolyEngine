#include <EnginePCH.hpp>

#include <Input/InputSystem.hpp>
#include <Input/InputWorldComponent.hpp>
#include <ECS/Scene.hpp>
#include <Utils/Optional.hpp>

using namespace Poly;

void InputSystem::InputPhase(Scene* world)
{
	InputWorldComponent* com = world->GetWorldComponent<InputWorldComponent>();
	com->IsConsumed = false;

	com->PrevKey = com->CurrKey;
	com->PrevMouseButton = com->CurrMouseButton;

	com->CharUTF8 = nullptr;

	com->MouseDelta = Vector2i::ZERO;
	com->PrevWheel = com->CurrWheel;

	for (auto& pair : com->Controllers)
	{
		pair.second.PrevButton = pair.second.CurrButton;
		pair.second.PrevAxis = pair.second.CurrAxis;
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
		case eInputEventType::TEXTCHAR:
			com->CharUTF8 = ev.CharUTF8;
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
			com->MousePos += ev.Pos;
			com->MouseDelta = ev.Pos;
			break;
		case eInputEventType::MOUSEPOS:
			// MOUSEPOS and MOUSEMOVE are received in pairs.
			// Setting com->MouseDelta here is (0,0)
			// and will result in overwriting com->MouseDelta set by MOUSEMOVE
			com->MousePos = ev.Pos;
			break;
		case eInputEventType::WHEELMOVE:
			com->CurrWheel += ev.Pos;
			break;
		case eInputEventType::CONTROLLER_ADDED:
		{
			com->Controllers[ev.JoystickID] = ControllerState();
			bool controllerAssigned = false;
			for (size_t i = 0; i < com->PlayerIDToJoystickID.GetSize(); ++i)
			{
				if (!com->PlayerIDToJoystickID[i].HasValue())
				{
					com->PlayerIDToJoystickID[i] = Optional<size_t>{ev.JoystickID};
					com->JoystickIDToPlayerID[ev.JoystickID] = i;
					controllerAssigned = true;
					Poly::gConsole.LogDebug("Controller added in existing place");
					break;
				}
			}
			if (!controllerAssigned)
			{
				size_t newPlayerID = com->PlayerIDToJoystickID.GetSize();
				com->PlayerIDToJoystickID.PushBack(Optional<size_t>{ev.JoystickID});
				com->JoystickIDToPlayerID[ev.JoystickID] = newPlayerID;
				Poly::gConsole.LogDebug("Controller added in new place");
			}
			break;
		}
		case eInputEventType::CONTROLLER_REMOVED:
		{
			size_t playerID = com->JoystickIDToPlayerID.at(ev.JoystickID);
			com->Controllers.erase(ev.JoystickID);
			com->PlayerIDToJoystickID[playerID] = Optional<size_t>{};
			com->JoystickIDToPlayerID.erase(ev.JoystickID);
			break;
		}
		case eInputEventType::CONTROLLER_BUTTON_DOWN:
			com->Controllers.at(ev.JoystickID).CurrButton[ev.ControllerButton] = true;
			break;
		case eInputEventType::CONTROLLER_BUTTON_UP:
			com->Controllers.at(ev.JoystickID).CurrButton[ev.ControllerButton] = false;
			break;
		case eInputEventType::CONTROLLER_AXIS_MOTION:
			com->Controllers.at(ev.JoystickID).CurrAxis[ev.ControllerAxis] = ev.AxisValue;
			break;
		case eInputEventType::_COUNT:
			HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
			break;
		}
		InputEventsQueue.PopFront();
	}
}