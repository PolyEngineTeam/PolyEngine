#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ComponentBase.hpp>
#include <pe/api/input/KeyBindings.hpp>
#include <pe/engine/input/InputSystem.hpp>

namespace pe::api::ecs
{
	class Scene;
}

namespace pe::engine::input
{
	struct ControllerState 
	{
		ControllerState() = default;

		::pe::core::utils::EnumArray<bool, api::input::eControllerButton> CurrButton;
		::pe::core::utils::EnumArray<bool, api::input::eControllerButton> PrevButton;
		::pe::core::utils::EnumArray<float, api::input::eControllerAxis> CurrAxis;
		::pe::core::utils::EnumArray<float, api::input::eControllerAxis> PrevAxis;
	};

	/// <summary>Scene component that holds input data.</summary>
	class ENGINE_DLLEXPORT InputWorldComponent : public api::ecs::ComponentBase
	{
		friend void InputSystem::InputPhase(api::ecs::Scene*);
	public:
		RTTI_DECLARE_COMPONENT(::pe::engine::input::InputWorldComponent) { NO_RTTI_PROPERTY(); }

		InputWorldComponent() = default;

		bool IsPressed(api::input::eKey key) const { return CurrKey[key]; }
		bool IsPressed(const std::initializer_list<api::input::eKey>& list) const;
		bool IsClicked(api::input::eKey key) const { return (CurrKey[key] && !PrevKey[key]); }
		bool IsReleased(api::input::eKey key) const { return (!CurrKey[key] && PrevKey[key]); }

		bool IsPressed(api::input::eMouseButton button) const { return CurrMouseButton[button]; }
		bool IsPressed(const std::initializer_list<api::input::eMouseButton>& list) const;
		bool IsClicked(api::input::eMouseButton button) const { return (CurrMouseButton[button] && !PrevMouseButton[button]); }
		bool IsReleased(api::input::eMouseButton button) const { return (!CurrMouseButton[button] && PrevMouseButton[button]); }

		const ::pe::core::math::Vector2i& GetMousePos() const { return MousePos; }
		const ::pe::core::math::Vector2i& GetMousePosDelta() const  { return MouseDelta; }

		const ::pe::core::math::Vector2i& GetWheelPos() const { return CurrWheel; }
		::pe::core::math::Vector2i GetWheelPosDelta() const { return CurrWheel - PrevWheel; }

		bool IsPressed(size_t playerID, api::input::eControllerButton button) const;
		bool IsClicked(size_t playerID, api::input::eControllerButton button) const;
		bool IsReleased(size_t playerID, api::input::eControllerButton button) const;

		float GetControllerAxis(size_t playerID, api::input::eControllerAxis axis) const;
		float GetControllerAxisDelta(size_t playerID, api::input::eControllerAxis axis) const;

		size_t GetConnectedControllersCount() const { return JoystickIDToPlayerID.size(); }
		std::vector<size_t> GetConnectedControllersIDs() const;
		bool IsControllerConnected(size_t idx) const;

		bool GetIsConsumed() const { return IsConsumed; }
		void SetConsumed() { IsConsumed = true; }
		
		const char* GetCharUTF8() { return CharUTF8; }

	private:
		::pe::core::utils::EnumArray<bool, api::input::eKey> CurrKey;
		::pe::core::utils::EnumArray<bool, api::input::eKey> PrevKey;
		::pe::core::utils::EnumArray<bool, api::input::eMouseButton> CurrMouseButton;
		::pe::core::utils::EnumArray<bool, api::input::eMouseButton> PrevMouseButton;
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
}

//REGISTER_COMPONENT(::pe::api::ecs::ComponentsIDGroup, ::pe::engine::input::InputWorldComponent)