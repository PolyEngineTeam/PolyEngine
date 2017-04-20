#pragma once

#include <Vector.hpp>
#include "ComponentBase.hpp"
#include "KeyBindings.hpp"

namespace Poly
{
	namespace InputSystem
	{

	class InputWorldComponent : public ComponentBase
	{
	friend void InputPhase(World*);
	public:
		bool GetPressed(eKey key)						 { return CurrKey[static_cast<int>(key)]; }
		bool GetPressed(eKey key1, eKey key2)			 { return (CurrKey[static_cast<int>(key1)] && CurrKey[static_cast<int>(key2)]) ? true : false; }
		bool GetPressed(eKey key1, eKey key2, eKey key3) { return (CurrKey[static_cast<int>(key1)] && CurrKey[static_cast<int>(key2)] && CurrKey[static_cast<int>(key3)]) ? true : false; }
		bool GetClicked(eKey key)  { return (CurrKey[static_cast<int>(key)] && !PrevKey[static_cast<int>(key)]) ? true : false; }
		bool GetReleased(eKey key) { return (!CurrKey[static_cast<int>(key)] && PrevKey[static_cast<int>(key)]) ? true : false; }
		const Vector& GetMousePos() { return CurrMouse; }
		const Vector& GetMouseDiff() { return CurrMouse - PrevMouse; }
		const Vector& GetWheelPos() { return CurrWheel; }
		const Vector& GetWheelDiff() { return CurrWheel - PrevWheel; }
	private:
		bool CurrKey[256];
		bool PrevKey[256];
		Vector CurrMouse;
		Vector PrevMouse;
		Vector CurrWheel;
		Vector PrevWheel;
	};

	}
}