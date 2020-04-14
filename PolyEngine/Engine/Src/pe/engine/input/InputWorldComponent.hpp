#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ComponentBase.hpp>
#include <pe/api/input/KeyBindings.hpp>

namespace pe::api::ecs
{
	class Scene;
}

namespace pe::engine::input
{
	/// <summary>Scene component that holds input data.</summary>
	class ENGINE_DLLEXPORT InputWorldComponent : public api::ecs::ComponentBase
	{
		friend class InputSystem;

		struct ControllerState
		{
			ControllerState() = default;

			::pe::core::utils::EnumArray<bool, api::input::eControllerButton> currButton;
			::pe::core::utils::EnumArray<bool, api::input::eControllerButton> prevButton;
			::pe::core::utils::EnumArray<float, api::input::eControllerAxis> currAxis;
			::pe::core::utils::EnumArray<float, api::input::eControllerAxis> prevAxis;
		};

	public:
		RTTI_DECLARE_COMPONENT(::pe::engine::input::InputWorldComponent) { NO_RTTI_PROPERTY(); }

		InputWorldComponent() = default;

		bool isPressed(api::input::eKey key) const { return m_currKey[key]; }
		bool isPressed(const std::initializer_list<api::input::eKey>& list) const;
		bool isClicked(api::input::eKey key) const { return (m_currKey[key] && !m_prevKey[key]); }
		bool isReleased(api::input::eKey key) const { return (!m_currKey[key] && m_prevKey[key]); }

		bool isPressed(api::input::eMouseButton button) const { return m_currMouseButton[button]; }
		bool isPressed(const std::initializer_list<api::input::eMouseButton>& list) const;
		bool isClicked(api::input::eMouseButton button) const { return (m_currMouseButton[button] && !m_prevMouseButton[button]); }
		bool isReleased(api::input::eMouseButton button) const { return (!m_currMouseButton[button] && m_prevMouseButton[button]); }

		const ::pe::core::math::Vector2i& getMousePos() const { return m_mousePos; }
		const ::pe::core::math::Vector2i& getMousePosDelta() const  { return m_mouseDelta; }

		const ::pe::core::math::Vector2i& getWheelPos() const { return m_currWheel; }
		::pe::core::math::Vector2i getWheelPosDelta() const { return m_currWheel - m_prevWheel; }

		bool isPressed(size_t playerID, api::input::eControllerButton button) const;
		bool isClicked(size_t playerID, api::input::eControllerButton button) const;
		bool isReleased(size_t playerID, api::input::eControllerButton button) const;

		float getControllerAxis(size_t playerID, api::input::eControllerAxis axis) const;
		float getControllerAxisDelta(size_t playerID, api::input::eControllerAxis axis) const;

		size_t getConnectedControllersCount() const { return m_joystickIDToPlayerID.size(); }
		std::vector<size_t> getConnectedControllersIDs() const;
		bool isControllerConnected(size_t idx) const;

		bool getIsConsumed() const { return m_isConsumed; }
		void setConsumed() { m_isConsumed = true; }
		
		const char* getCharUTF8() { return m_charUTF8; }

	private:
		::pe::core::utils::EnumArray<bool, api::input::eKey> m_currKey;
		::pe::core::utils::EnumArray<bool, api::input::eKey> m_prevKey;
		::pe::core::utils::EnumArray<bool, api::input::eMouseButton> m_currMouseButton;
		::pe::core::utils::EnumArray<bool, api::input::eMouseButton> m_prevMouseButton;
		::pe::core::math::Vector2i m_mousePos;
		::pe::core::math::Vector2i m_mouseDelta;
		::pe::core::math::Vector2i m_currWheel;
		::pe::core::math::Vector2i m_prevWheel;
		std::unordered_map<size_t, ControllerState> m_controllers;
		std::vector<std::optional<size_t>> m_playerIDToJoystickID;
		std::unordered_map<size_t, size_t> m_joystickIDToPlayerID;
		bool m_isConsumed = false;
		const char* m_charUTF8 = nullptr;
	};
}

//REGISTER_COMPONENT(::pe::api::ecs::ComponentsIDGroup, ::pe::engine::input::InputWorldComponent)