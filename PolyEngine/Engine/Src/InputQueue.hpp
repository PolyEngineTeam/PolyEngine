#pragma once

#include <Queue.hpp>
#include <Vector2i.hpp>
#include "KeyBindings.hpp"

namespace Poly
{
	enum class eInputEventType
	{
		KEYDOWN,
		KEYUP,
		MOUSEBUTTONDOWN,
		MOUSEBUTTONUP,
		MOUSEMOVE,
		WHEELMOVE,
        CONTROLLERADDED,
        CONTROLLERREMOVED,
        CONTROLLERBUTTONDOWN,
        CONTROLLERBUTTONUP,
        CONTROLLERAXIS,
		_COUNT
	};

	struct InputEvent : public BaseObject<>
	{
		InputEvent() = default;
		InputEvent(eInputEventType type, eKey key) : Type(type), Key(key) {}
		InputEvent(eInputEventType type, eMouseButton button) : Type(type), MouseButton(button) {}
		InputEvent(eInputEventType type, const Vector2i& pos) : Type(type), Pos(pos) {}
		InputEvent(eInputEventType type, Sint32 id, eControllerButton button) : Type(type), ControllerID(id), ControllerButton(button) {}
		InputEvent(eInputEventType type, Sint32 id, eControllerAxis axis, float value) : Type(type), ControllerID(id), ControllerAxis(axis), AxisValue(value) {}
		InputEvent(eInputEventType type, Sint32 id) : Type(type), ControllerID(id) {}

		eInputEventType Type = eInputEventType::_COUNT;
		eKey Key = eKey::_COUNT;
		eMouseButton MouseButton = eMouseButton::_COUNT;
		Vector2i Pos;
        Sint32 ControllerID;
        eControllerButton ControllerButton = eControllerButton::_COUNT;
        eControllerAxis ControllerAxis = eControllerAxis::_COUNT;
        float AxisValue;
    };

	using InputQueue = Queue<InputEvent>;
}
