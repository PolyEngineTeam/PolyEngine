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

		::pe::core::utils::EnumArray<bool, eControllerButton> CurrButton;
		::pe::core::utils::EnumArray<bool, eControllerButton> PrevButton;
		::pe::core::utils::EnumArray<float, eControllerAxis> CurrAxis;
		::pe::core::utils::EnumArray<float, eControllerAxis> PrevAxis;
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

		const ::pe::core::math::Vector2i& GetMousePos() const { return MousePos; }
		const ::pe::core::math::Vector2i& GetMousePosDelta() const  { return MouseDelta; }

		const ::pe::core::math::Vector2i& GetWheelPos() const { return CurrWheel; }
		::pe::core::math::Vector2i GetWheelPosDelta() const { return CurrWheel - PrevWheel; }

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
		::pe::core::utils::EnumArray<bool, eKey> CurrKey;
		::pe::core::utils::EnumArray<bool, eKey> PrevKey;
		::pe::core::utils::EnumArray<bool, eMouseButton> CurrMouseButton;
		::pe::core::utils::EnumArray<bool, eMouseButton> PrevMouseButton;
		::pe::core::math::Vector2i MousePos;
		::pe::core::math::Vector2i MouseDelta;
		::pe::core::math::Vector2i CurrWheel;
		::pe::core::math::Vector2i PrevWheel;
		std::unordered_map<size_t, ControllerState> Controllers;
		std::vector<std::optional<size_t>> PlayerIDToJoystickID;
		std::unordered_map<size_t, size_t> JoystickIDToPlayerID;
		bool IsConsumed = false;
		const char* CharUTF8 = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, InputWorldComponent)
}