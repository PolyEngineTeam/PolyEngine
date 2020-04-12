#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/Vector2f.hpp>
#include <pe/core/math/Vector2i.hpp>
#include <pe/core/storage/Queue.hpp>
#include <pe/api/input/KeyBindings.hpp>

namespace pe::api::input
{
	enum class eInputEventType
	{
		KEYDOWN,
		KEYUP,
		TEXTCHAR,
		MOUSEBUTTONDOWN,
		MOUSEBUTTONUP,
		MOUSEMOVE,
		MOUSEPOS,
		WHEELMOVE,
        CONTROLLER_ADDED,
        CONTROLLER_REMOVED,
        CONTROLLER_BUTTON_DOWN,
        CONTROLLER_BUTTON_UP,
        CONTROLLER_AXIS_MOTION,
		_COUNT
	};

	struct InputEvent : public ::pe::core::BaseObject<>
	{
		InputEvent() = default;
		InputEvent(eInputEventType type, eKey key) : type(type), key(key) {}
		InputEvent(eInputEventType type, const char* charUTF8) : type(type), charUTF8(charUTF8) {}
		InputEvent(eInputEventType type, eMouseButton button) : type(type), mouseButton(button) {}
		InputEvent(eInputEventType type, const ::pe::core::math::Vector2i& pos) : type(type), pos(pos) {}
		InputEvent(eInputEventType type, size_t id, eControllerButton button) : type(type), joystickID(id), controllerButton(button) {}
		InputEvent(eInputEventType type, size_t id, eControllerAxis axis, float value) : type(type), joystickID(id), controllerAxis(axis), axisValue(value) {}
		InputEvent(eInputEventType type, size_t id) : type(type), joystickID(id) {}

		eInputEventType type = eInputEventType::_COUNT;
		eKey key = eKey::_COUNT;
		const char* charUTF8;
		eMouseButton mouseButton = eMouseButton::_COUNT;
		::pe::core::math::Vector2i pos;
        size_t joystickID;
        eControllerButton controllerButton = eControllerButton::_COUNT;
        eControllerAxis controllerAxis = eControllerAxis::_COUNT;
        float axisValue;
    };

	using InputQueue = ::pe::core::storage::Queue<InputEvent>;
}
