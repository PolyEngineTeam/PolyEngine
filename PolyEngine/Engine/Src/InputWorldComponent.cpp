#include "EnginePCH.hpp"

bool Poly::InputWorldComponent::IsPressed(const std::initializer_list<eKey>& list) const
{
	bool result = true;
	for (const eKey& i : list)
		result = result && CurrKey[i];
	return result;
}
