#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Input/KeyBindings.hpp>
#include <Input/InputSystem.hpp>

namespace Poly
{
	struct ControllerState 
	{
		ControllerState() = default;

		core::utils::EnumArray<bool, eControllerButton> CurrButton;
		core::utils::EnumArray<bool, eControllerButton> PrevButton;
		core::utils::EnumArray<float, eControllerAxis> CurrAxis;
		core::utils::EnumArray<float, eControllerAxis> PrevAxis;
	};

	/// <summary>Scene component that holds input data.</summary>
	class ENGINE_DLLEXPORT InputWorldComponent : public ComponentBase
	{
		friend void InputSystem::InputPhase(Scene*);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::InputWorldComponent) { NO_RTTI_PROPERTY(); }

		InputWorldComponent() = default;

		bool IsPressed(eKey key) const { return CurrKey[key]; }
		bool IsPressed(const std::initializer_list<eKey>& list) const;
		bool IsClicked(eKey key) const { return (CurrKey[key] && !PrevKey[key]); }
		bool IsReleased(eKey key) const { return (!CurrKey[key] && PrevKey[key]); }

		bool IsPressed(eMouseButton button) const { return CurrMouseButton[button]; }
		bool IsPressed(const std::initializer_list<eMouseButton>& list) const;
		bool IsClicked(eMouseButton button) const { return (CurrMouseButton[button] && !PrevMouseButton[button]); }
		bool IsReleased(eMouseButton button) const { return (!CurrMouseButton[button] && PrevMouseButton[button]); }

		const core::math::Vector2i& GetMousePos() const { return MousePos; }
		const core::math::Vector2i& GetMousePosDelta() const  { return MouseDelta; }

		const core::math::Vector2i& GetWheelPos() const { return CurrWheel; }
		core::math::Vector2i GetWheelPosDelta() const { return CurrWheel - PrevWheel; }

		bool IsPressed(size_t playerID, eControllerButton button) const;
		bool IsClicked(size_t playerID, eControllerButton button) const;
		bool IsReleased(size_t playerID, eControllerButton button) const;

		float GetControllerAxis(size_t playerID, eControllerAxis axis) const;
		float GetControllerAxisDelta(size_t playerID, eControllerAxis axis) const;

		size_t GetConnectedControllersCount() const { return JoystickIDToPlayerID.size(); }
		std::vector<size_t> GetConnectedControllersIDs() const;
		bool IsControllerConnected(size_t idx) const;

		bool GetIsConsumed() const { return IsConsumed; }
		void SetConsumed() { IsConsumed = true; }
		
		const char* GetCharUTF8() { return CharUTF8; }

	private:
		core::utils::EnumArray<bool, eKey> CurrKey;
		core::utils::EnumArray<bool, eKey> PrevKey;
		core::utils::EnumArray<bool, eMouseButton> CurrMouseButton;
		core::utils::EnumArray<bool, eMouseButton> PrevMouseButton;
		core::math::Vector2i MousePos;
		core::math::Vector2i MouseDelta;
		core::math::Vector2i CurrWheel;
		core::math::Vector2i PrevWheel;
		std::unordered_map<size_t, ControllerState> Controllers;
		std::vector<std::optional<size_t>> PlayerIDToJoystickID;
		std::unordered_map<size_t, size_t> JoystickIDToPlayerID;
		bool IsConsumed = false;
		const char* CharUTF8 = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, InputWorldComponent)
}