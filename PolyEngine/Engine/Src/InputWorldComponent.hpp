#pragma once

#include <initializer_list>
#include <Vector.hpp>
#include <EnumUtils.hpp>
#include "ComponentBase.hpp"
#include "KeyBindings.hpp"
#include "InputSystem.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT InputWorldComponent : public ComponentBase
	{
	friend void InputSystem::InputPhase(World*);
	public:
		bool IsPressed(const std::initializer_list<eKey>& list) const;
		bool IsClicked(eKey key) const { return (CurrKey[key] && !PrevKey[key]); }
		bool IsReleased(eKey key) const { return (!CurrKey[key] && PrevKey[key]); }
		const Vector& GetMousePos() const { return CurrMouse; }
		Vector GetMouseDiff() const  { return CurrMouse - PrevMouse; }
		const int& GetWheelPos() const { return CurrWheel; }
		int GetWheelDiff() const { return CurrWheel - PrevWheel; }
	private:
		EnumArray<bool, eKey> CurrKey;
		EnumArray<bool, eKey> PrevKey;
		Vector CurrMouse;
		Vector PrevMouse;
		int CurrWheel;
		int PrevWheel;
	};
}