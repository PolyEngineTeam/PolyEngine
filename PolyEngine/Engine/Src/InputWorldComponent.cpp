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
