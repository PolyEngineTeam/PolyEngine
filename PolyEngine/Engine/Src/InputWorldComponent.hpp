#pragma once

#include <initializer_list>
#include <Vector.hpp>
#include <EnumUtils.hpp>
#include "ComponentBase.hpp"
#include "KeyBindings.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT InputWorldComponent : public ComponentBase
	{
	friend void InputSystem::InputPhase(World*);
	public:
		const bool IsPressed(const std::initializer_list<eKey>& list)	
		{
			bool result = true;
			for (const eKey& i : list)
				result = result && CurrKey[i];
			return result;
		}

		const bool IsClicked(eKey key) { return (CurrKey[key] && !PrevKey[key]) ? true : false; }
		const bool IsReleased(eKey key) { return (!CurrKey[key] && PrevKey[key]) ? true : false; }
		const Vector& GetMousePos() { return CurrMouse; }
		const Vector GetMouseDiff() { return CurrMouse - PrevMouse; }
		const int& GetWheelPos() { return CurrWheel; }
		const int GetWheelDiff() { return CurrWheel - PrevWheel; }
	private:
		EnumArray<bool, eKey> CurrKey;
		EnumArray<bool, eKey> PrevKey;
		Vector CurrMouse;
		Vector PrevMouse;
		int CurrWheel;
		int PrevWheel;
	};
}