#pragma once

#include <initializer_list>
#include <Math/Vector2i.hpp>
#include <Utils/EnumUtils.hpp>
#include "ECS/ComponentBase.hpp"
#include "Input/KeyBindings.hpp"
#include "Input/InputSystem.hpp"
#include <Utils/Optional.hpp>

namespace Poly
{
	struct ControllerState 
	{
		ControllerState() = default;

		EnumArray<bool, eControllerButton> CurrButton;
		EnumArray<bool, eControllerButton> PrevButton;
		EnumArray<float, eControllerAxis> CurrAxis;
		EnumArray<float, eControllerAxis> PrevAxis;
	};

	/// <summary>World component that holds input data.</summary>
	class ENGINE_DLLEXPORT InputWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(InputWorldComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY();
		}

		friend void InputSystem::InputPhase(World*);

	public:
		InputWorldComponent() = default;

		bool IsPressed(eKey key) const { return CurrKey[key]; }
		bool IsPressed(const std::initializer_list<eKey>& list) const;
		bool IsClicked(eKey key) const { return (CurrKey[key] && !PrevKey[key]); }
		bool IsReleased(eKey key) const { return (!CurrKey[key] && PrevKey[key]); }

		bool IsPressed(eMouseButton button) const { return CurrMouseButton[button]; }
		bool IsPressed(const std::initializer_list<eMouseButton>& list) const;
		bool IsClicked(eMouseButton button) const { return (CurrMouseButton[button] && !PrevMouseButton[button]); }
		bool IsReleased(eMouseButton button) const { return (!CurrMouseButton[button] && PrevMouseButton[button]); }

		const Vector2i& GetMousePos() const { return MousePos; }
		const Vector2i& GetMousePosDelta() const  { return MouseDelta; }

		const Vector2i& GetWheelPos() const { return CurrWheel; }
		Vector2i GetWheelPosDelta() const { return CurrWheel - PrevWheel; }

		bool IsPressed(size_t playerID, eControllerButton button) const;
		bool IsClicked(size_t playerID, eControllerButton button) const;
		bool IsReleased(size_t playerID, eControllerButton button) const;

		float GetControllerAxis(size_t playerID, eControllerAxis axis) const;
		float GetControllerAxisDelta(size_t playerID, eControllerAxis axis) const;

		size_t GetConnectedControllersCount() const { return JoystickIDToPlayerID.size(); }
		Dynarray<size_t> GetConnectedControllersIDs() const;
		bool IsControllerConnected(size_t idx) const;

	private:
		EnumArray<bool, eKey> CurrKey;
		EnumArray<bool, eKey> PrevKey;
		EnumArray<bool, eMouseButton> CurrMouseButton;
		EnumArray<bool, eMouseButton> PrevMouseButton;
		Vector2i MousePos;
		Vector2i MouseDelta;
		Vector2i CurrWheel;
		Vector2i PrevWheel;
		std::unordered_map<size_t, ControllerState> Controllers;
		Dynarray<Optional<size_t>> PlayerIDToJoystickID;
		std::unordered_map<size_t, size_t> JoystickIDToPlayerID;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, InputWorldComponent)
}