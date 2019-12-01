#include <pe/engine/EnginePCH.hpp>

#include <pe/api/ecs/Entity.hpp>
#include <pe/engine/input/InputWorldComponent.hpp>

RTTI_DEFINE_COMPONENT(::pe::engine::input::InputWorldComponent)

namespace pe::engine::input {

bool InputWorldComponent::IsPressed(const std::initializer_list<api::input::eKey>& list) const
{
	bool result = true;
	for (const api::input::eKey& i : list)
		result = result && CurrKey[i];
	return result;
}

bool InputWorldComponent::IsPressed(const std::initializer_list<api::input::eMouseButton >& list) const
{
	bool result = true;
	for (const api::input::eMouseButton& i : list)
		result = result && CurrMouseButton[i];
	return result;
}

bool InputWorldComponent::IsPressed(size_t playerID, api::input::eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].value();
	return Controllers.at(joystickID).CurrButton[button];
}

bool InputWorldComponent::IsClicked(size_t playerID, api::input::eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].value();
	const ControllerState& controller = Controllers.at(joystickID);
	return controller.CurrButton[button] && !controller.PrevButton[button];
}

bool InputWorldComponent::IsReleased(size_t playerID, api::input::eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].value();
	const ControllerState& controller = Controllers.at(joystickID);
	return !controller.CurrButton[button] && controller.PrevButton[button];
}

float InputWorldComponent::GetControllerAxis(size_t playerID, api::input::eControllerAxis axis) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].value();
	return Controllers.at(joystickID).CurrAxis[axis];
}

float InputWorldComponent::GetControllerAxisDelta(size_t playerID, api::input::eControllerAxis axis) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].value();
	const ControllerState& controller = Controllers.at(joystickID);
	return controller.CurrAxis[axis] && !controller.PrevAxis[axis];
}

std::vector<size_t> InputWorldComponent::GetConnectedControllersIDs() const
{
	size_t arraySize = GetConnectedControllersCount();
	std::vector<size_t> controllersIDs(arraySize);
	for(size_t i = 0; i < PlayerIDToJoystickID.size(); ++i)
	{
		if(PlayerIDToJoystickID[i].has_value())
		{
			controllersIDs.push_back(i);
		}
	}
	return controllersIDs;
}

bool InputWorldComponent::IsControllerConnected(size_t idx) const
{
	return PlayerIDToJoystickID.size() > idx && PlayerIDToJoystickID[idx].has_value();
}

}