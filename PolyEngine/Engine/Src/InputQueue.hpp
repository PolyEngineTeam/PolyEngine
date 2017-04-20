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
		InputEvent() {}
		InputEvent(const InputEvent &src) : Type(src.Type), Pos(src.Pos) {}
		InputEvent(eInputEventType type, eKey key) : Type(type), Key(key) {}
		InputEvent(eInputEventType type, const Vector& pos) : Type(type), Pos(pos) {}
		~InputEvent() {}

		InputEvent& operator=(const InputEvent& src) { Type = src.Type; Pos = src.Pos; return *this; }

		eInputEventType Type;
		union
		{
			eKey Key;
			Vector Pos;
		};
	};

	typedef Queue<InputEvent> InputQueue;
}