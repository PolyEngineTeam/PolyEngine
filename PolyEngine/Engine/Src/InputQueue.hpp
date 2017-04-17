#pragma once

#include <Queue.hpp>
#include <Vector.hpp>
#include "KeyBindings.hpp"

namespace Poly
{
	enum class eEventType
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
		InputEvent(eEventType type, eKey key) : Type(type), Key(key) {}
		InputEvent(eEventType type, const Vector& pos) : Type(type), Pos(pos) {}

		eEventType Type = eEventType::_COUNT;
		eKey Key;
		Vector Pos;
	};

	typedef Queue<InputEvent> InputQueue;
}