#pragma once

#include <initializer_list>
#include <Vector.hpp>
#include <EnumUtils.hpp>
#include "ComponentBase.hpp"
#include "KeyBindings.hpp"
#include "InputSystem.hpp"

namespace Poly
{
	/// <summary>World component that holds input data.</summary>
	class ENGINE_DLLEXPORT InputWorldComponent : public ComponentBase
	{
	friend void InputSystem::InputPhase(World*);
	public:
		InputWorldComponent() = default;

		bool IsPressed(eKey key) const { return CurrKey[key]; }
		bool IsPressed(const std::initializer_list<eKey>& list) const;
		bool IsClicked(eKey key) const { return (CurrKey[key] && !PrevKey[key]); }
		bool IsReleased(eKey key) const { return (!CurrKey[key] && PrevKey[key]); }

		const Vector& GetMousePos() const { return CurrMouse; }
		Vector GetMousePosDelta() const  { return CurrMouse - PrevMouse; }

		int GetWheelPos() const { return CurrWheel; }
		int GetWheelPosDelta() const { return CurrWheel - PrevWheel; }
	private:
		EnumArray<bool, eKey> CurrKey;
		EnumArray<bool, eKey> PrevKey;
		Vector CurrMouse;
		Vector PrevMouse;
		int CurrWheel = 0;
		int PrevWheel = 0;
	};
}