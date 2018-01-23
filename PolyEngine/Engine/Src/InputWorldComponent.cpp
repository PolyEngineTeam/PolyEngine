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

bool Poly::InputWorldComponent::IsPressed(size_t controllerID, eControllerButton button) const
{
	if(ControllerPointers.GetSize() <= controllerID)
	{
		return false;
	}
	SDL_GameController* controller = ControllerPointers[controllerID];
	if(controller != nullptr)
	{
		return Controllers.at(controller).CurrButton[button];
	}
	else
	{
		return false;
	}
}

bool Poly::InputWorldComponent::IsClicked(size_t controllerID, eControllerButton button) const
{
	if(ControllerPointers.GetSize() <= controllerID)
	{
		return false;
	}
	SDL_GameController* controller = ControllerPointers[controllerID];
	if(controller != nullptr)
	{
		return Controllers.at(controller).CurrButton[button] && !Controllers.at(controller).PrevButton[button];
	}
	else
	{
		return false;
	}
}

bool Poly::InputWorldComponent::IsReleased(size_t controllerID, eControllerButton button) const
{
	if(ControllerPointers.GetSize() <= controllerID)
	{
		return false;
	}
	SDL_GameController* controller = ControllerPointers[controllerID];
	if(controller != nullptr)
	{
		return !Controllers.at(controller).CurrButton[button] && Controllers.at(controller).PrevButton[button];
	}
	else
	{
		return false;
	}
}

float Poly::InputWorldComponent::GetControllerAxis(size_t controllerID, eControllerAxis axis) const
{
	if(ControllerPointers.GetSize() <= controllerID)
	{
		return 0.0f;
	}
	SDL_GameController* controller = ControllerPointers[controllerID];
	if(controller != nullptr)
	{
		return Controllers.at(controller).CurrAxis[axis];
	}
	else
	{
		return false;
	}
}

float Poly::InputWorldComponent::GetControllerAxisDelta(size_t controllerID, eControllerAxis axis) const
{
	if(ControllerPointers.GetSize() <= controllerID)
	{
		return 0.0f;
	}
	SDL_GameController* controller = ControllerPointers[controllerID];
	if(controller != nullptr)
	{
		return Controllers.at(controller).CurrAxis[axis] - Controllers.at(controller).PrevAxis[axis];
	}
	else
	{
		return false;
	}
}

bool Poly::InputWorldComponent::IsControllerConnected(size_t idx) const
{
	if(ControllerPointers.GetSize() <= idx)
	{
		return false;
	}
	else
	{
		return ControllerPointers[idx] != nullptr;
	}
}