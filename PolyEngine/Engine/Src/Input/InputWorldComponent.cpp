#include <EnginePCH.hpp>

#include <Input/InputWorldComponent.hpp>

RTTI_DEFINE_COMPONENT(::Poly::InputWorldComponent)

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
	size_t joystickID = PlayerIDToJoystickID[playerID].Value();
	return Controllers.at(joystickID).CurrButton[button];
}

bool Poly::InputWorldComponent::IsClicked(size_t playerID, eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].Value();
	const ControllerState& controller = Controllers.at(joystickID);
	return controller.CurrButton[button] && !controller.PrevButton[button];
}

bool Poly::InputWorldComponent::IsReleased(size_t playerID, eControllerButton button) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].Value();
	const ControllerState& controller = Controllers.at(joystickID);
	return !controller.CurrButton[button] && controller.PrevButton[button];
}

float Poly::InputWorldComponent::GetControllerAxis(size_t playerID, eControllerAxis axis) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].Value();
	return Controllers.at(joystickID).CurrAxis[axis];
}

float Poly::InputWorldComponent::GetControllerAxisDelta(size_t playerID, eControllerAxis axis) const
{
	ASSERTE(IsControllerConnected(playerID), "Controller not connected!");
	size_t joystickID = PlayerIDToJoystickID[playerID].Value();
	const ControllerState& controller = Controllers.at(joystickID);
	return controller.CurrAxis[axis] && !controller.PrevAxis[axis];
}

Poly::Dynarray<size_t> Poly::InputWorldComponent::GetConnectedControllersIDs() const
{
	size_t arraySize = GetConnectedControllersCount();
	Poly::Dynarray<size_t> controllersIDs(arraySize);
	for(size_t i = 0; i < PlayerIDToJoystickID.GetSize(); ++i)
	{
		if(PlayerIDToJoystickID[i].HasValue())
		{
			controllersIDs.PushBack(i);
		}
	}
	return controllersIDs;
}

bool Poly::InputWorldComponent::IsControllerConnected(size_t idx) const
{
	return PlayerIDToJoystickID.GetSize() > idx && PlayerIDToJoystickID[idx].HasValue();
}