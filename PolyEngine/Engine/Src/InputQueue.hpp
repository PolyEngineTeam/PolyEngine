#pragma once

#include <Queue.hpp>
#include <Vector.hpp>
#include "KeyBindings.hpp"

namespace Poly
{
	enum class eInputEventType
	{
		KEYDOWN,
		KEYUP,
		MOUSEMOVE,
		WHEELMOVE,
		_COUNT
	};

	struct InputEvent : public BaseObject<>
	{
		InputEvent() = default;
		InputEvent(eInputEventType type, eKey key) : Type(type), Key(key) {}
		InputEvent(eInputEventType type, const Vector& pos) : Type(type), Pos(pos) {}

		eInputEventType Type = eInputEventType::_COUNT;
		eKey Key;
		Vector Pos;
	};

	using InputQueue = Queue<InputEvent>;
}
