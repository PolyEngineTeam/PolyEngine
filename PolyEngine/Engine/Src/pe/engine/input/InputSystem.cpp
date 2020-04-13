#include <pe/engine/EnginePCH.hpp>

#include <pe/engine/input/InputSystem.hpp>
#include <pe/engine/input/InputWorldComponent.hpp>
#include <pe/api/ecs/Scene.hpp>


namespace pe::engine::input {

using ::pe::api::input::eInputEventType;
using ::pe::api::input::eKey;
using ::pe::api::input::eMouseButton;

void InputSystem::onUpdate(api::ecs::Scene* scene)
{
	if (!m_app)
		return;
	
	InputWorldComponent* com = scene->getComponent<InputWorldComponent>();
	com->m_isConsumed = false;

	com->m_prevKey = com->m_currKey;
	com->m_prevMouseButton = com->m_currMouseButton;

	com->m_charUTF8 = nullptr;

	com->m_mouseDelta = core::math::Vector2i::ZERO;
	com->m_prevWheel = com->m_currWheel;

	for (auto& pair : com->m_controllers)
	{
		pair.second.prevButton = pair.second.currButton;
		pair.second.prevAxis = pair.second.currAxis;
	}

	api::input::InputQueue& InputEventsQueue = m_app->getInputQueue();
	while (!InputEventsQueue.isEmpty())
	{
		api::input::InputEvent& ev = InputEventsQueue.front();
		switch (ev.type)
		{
		case eInputEventType::KEYDOWN:
			if(ev.key < eKey::_COUNT)
				com->m_currKey[ev.key] = true;
			break;
		case eInputEventType::KEYUP:
			if(ev.key < eKey::_COUNT)
				com->m_currKey[ev.key] = false;
			break;
		case eInputEventType::TEXTCHAR:
			com->m_charUTF8 = ev.charUTF8;
			break;
		case eInputEventType::MOUSEBUTTONDOWN:
			if(ev.mouseButton < eMouseButton::_COUNT)
				com->m_currMouseButton[ev.mouseButton] = true;
			break;
		case eInputEventType::MOUSEBUTTONUP:
			if(ev.mouseButton < eMouseButton::_COUNT)
				com->m_currMouseButton[ev.mouseButton] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			com->m_mousePos += ev.pos;
			com->m_mouseDelta = ev.pos;
			break;
		case eInputEventType::MOUSEPOS:
			// MOUSEPOS and MOUSEMOVE are received in pairs.
			// Setting com->MouseDelta here is (0,0)
			// and will result in overwriting com->MouseDelta set by MOUSEMOVE
			com->m_mousePos = ev.pos;
			break;
		case eInputEventType::WHEELMOVE:
			com->m_currWheel += ev.pos;
			break;
		case eInputEventType::CONTROLLER_ADDED:
		{
			com->m_controllers[ev.joystickID] = InputWorldComponent::ControllerState{};
			bool controllerAssigned = false;
			for (size_t i = 0; i < com->m_playerIDToJoystickID.size(); ++i)
			{
				if (!com->m_playerIDToJoystickID[i].has_value())
				{
					com->m_playerIDToJoystickID[i] = ev.joystickID;
					com->m_joystickIDToPlayerID[ev.joystickID] = i;
					controllerAssigned = true;
					core::utils::gConsole.LogDebug("Controller added in existing place");
					break;
				}
			}
			if (!controllerAssigned)
			{
				size_t newPlayerID = com->m_playerIDToJoystickID.size();
				com->m_playerIDToJoystickID.push_back(ev.joystickID);
				com->m_joystickIDToPlayerID[ev.joystickID] = newPlayerID;
				core::utils::gConsole.LogDebug("Controller added in new place");
			}
			break;
		}
		case eInputEventType::CONTROLLER_REMOVED:
		{
			size_t playerID = com->m_joystickIDToPlayerID.at(ev.joystickID);
			com->m_controllers.erase(ev.joystickID);
			com->m_playerIDToJoystickID[playerID] = {};
			com->m_joystickIDToPlayerID.erase(ev.joystickID);
			break;
		}
		case eInputEventType::CONTROLLER_BUTTON_DOWN:
			com->m_controllers.at(ev.joystickID).currButton[ev.controllerButton] = true;
			break;
		case eInputEventType::CONTROLLER_BUTTON_UP:
			com->m_controllers.at(ev.joystickID).currButton[ev.controllerButton] = false;
			break;
		case eInputEventType::CONTROLLER_AXIS_MOTION:
			com->m_controllers.at(ev.joystickID).currAxis[ev.controllerAxis] = ev.axisValue;
			break;
		case eInputEventType::_COUNT:
			HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::push(), which is an invalid value");
			break;
		}
		InputEventsQueue.popFront();
	}
}
}