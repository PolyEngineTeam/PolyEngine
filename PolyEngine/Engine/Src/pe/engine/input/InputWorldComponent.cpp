#include <pe/engine/EnginePCH.hpp>

#include <pe/api/ecs/Entity.hpp>
#include <pe/engine/input/InputWorldComponent.hpp>

RTTI_DEFINE_COMPONENT(::pe::engine::input::InputWorldComponent)

namespace pe::engine::input {

bool InputWorldComponent::isPressed(const std::initializer_list<api::input::eKey>& list) const
{
	bool result = true;
	for (const api::input::eKey& i : list)
		result = result && m_currKey[i];
	return result;
}

bool InputWorldComponent::isPressed(const std::initializer_list<api::input::eMouseButton >& list) const
{
	bool result = true;
	for (const api::input::eMouseButton& i : list)
		result = result && m_currMouseButton[i];
	return result;
}

bool InputWorldComponent::isPressed(size_t playerID, api::input::eControllerButton button) const
{
	ASSERTE(isControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = m_playerIDToJoystickID[playerID].value();
	return m_controllers.at(joystickID).currButton[button];
}

bool InputWorldComponent::isClicked(size_t playerID, api::input::eControllerButton button) const
{
	ASSERTE(isControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = m_playerIDToJoystickID[playerID].value();
	const ControllerState& controller = m_controllers.at(joystickID);
	return controller.currButton[button] && !controller.prevButton[button];
}

bool InputWorldComponent::isReleased(size_t playerID, api::input::eControllerButton button) const
{
	ASSERTE(isControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = m_playerIDToJoystickID[playerID].value();
	const ControllerState& controller = m_controllers.at(joystickID);
	return !controller.currButton[button] && controller.prevButton[button];
}

float InputWorldComponent::getControllerAxis(size_t playerID, api::input::eControllerAxis axis) const
{
	ASSERTE(isControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = m_playerIDToJoystickID[playerID].value();
	return m_controllers.at(joystickID).currAxis[axis];
}

float InputWorldComponent::getControllerAxisDelta(size_t playerID, api::input::eControllerAxis axis) const
{
	ASSERTE(isControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = m_playerIDToJoystickID[playerID].value();
	const ControllerState& controller = m_controllers.at(joystickID);
	return controller.currAxis[axis] && !controller.prevAxis[axis];
}

std::vector<size_t> InputWorldComponent::getConnectedControllersIDs() const
{
	size_t arraySize = getConnectedControllersCount();
	std::vector<size_t> controllersIDs(arraySize);
	for(size_t i = 0; i < m_playerIDToJoystickID.size(); ++i)
	{
		if(m_playerIDToJoystickID[i].has_value())
		{
			controllersIDs.push_back(i);
		}
	}
	return controllersIDs;
}

bool InputWorldComponent::isControllerConnected(size_t idx) const
{
	return m_playerIDToJoystickID.size() > idx && m_playerIDToJoystickID[idx].has_value();
}

}