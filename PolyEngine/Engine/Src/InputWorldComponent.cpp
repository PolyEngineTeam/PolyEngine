#include "EnginePCH.hpp"

#include "InputWorldComponent.hpp"

bool Poly::InputWorldComponent::IsPressed(const std::initializer_list<eKey>& list) const
{
	bool result = true;
	for (const eKey& i : list)
		result = result && CurrKey[i];
	return result;
}

bool Poly::InputWorldComponent::IsPressed(const std::initializer_list<eMouseButton >& list) const
{
	bool result = true;
	for (const eMouseButton& i : list)
		result = result && CurrMouseButton[i];
	return result;
}

bool Poly::InputWorldComponent::IsPressed(size_t playerID, eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	i32 joystickID = PlayerIDToJoystickID.at(playerID);
	return Controllers.at(joystickID).CurrButton[button];
}

bool Poly::InputWorldComponent::IsClicked(size_t playerID, eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	i32 joystickID = PlayerIDToJoystickID.at(playerID);
	return Controllers.at(joystickID).CurrButton[button] && !Controllers.at(playerID).PrevButton[button];
}

bool Poly::InputWorldComponent::IsReleased(size_t playerID, eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	i32 joystickID = PlayerIDToJoystickID.at(playerID);
	return !Controllers.at(joystickID).CurrButton[button] && Controllers.at(playerID).PrevButton[button];
}

float Poly::InputWorldComponent::GetControllerAxis(size_t playerID, eControllerAxis axis) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	i32 joystickID = PlayerIDToJoystickID.at(playerID);
	return Controllers.at(joystickID).CurrAxis[axis];
}

float Poly::InputWorldComponent::GetControllerAxisDelta(size_t playerID, eControllerAxis axis) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	i32 joystickID = PlayerIDToJoystickID.at(playerID);
	return Controllers.at(joystickID).CurrAxis[axis] && !Controllers.at(playerID).PrevAxis[axis];
}

size_t* Poly::InputWorldComponent::GetConnectedControllersIDs() const
{
	size_t arraySize = GetConnectedControllersCount();
	size_t* controllersIDs = new size_t[arraySize];
	size_t idx = 0;
	for(auto& pair : PlayerIDToJoystickID)
	{
		controllersIDs[idx] = pair.first;
		++idx;
	}
	std::sort(controllersIDs, controllersIDs + arraySize);
	return controllersIDs;
}

bool Poly::InputWorldComponent::IsControllerConnected(size_t idx) const
{
	return PlayerIDToJoystickID.find(idx) != PlayerIDToJoystickID.end();
}