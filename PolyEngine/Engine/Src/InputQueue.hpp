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
		WHEELMOVE
	};

	struct InputEvent : public BaseObject<>
	{	
		InputEvent() {}
		InputEvent(const InputEvent &src) : Type(src.Type), Pos(src.Pos) {}
		InputEvent(eEventType type, eKey key) : Type(type), Key(key) {}
		InputEvent(eEventType type, const Vector& pos) : Type(type), Pos(pos) {}
		~InputEvent() {}

		InputEvent& operator=(const InputEvent& src) { Type = src.Type; Pos = src.Pos; return *this; }

		eEventType Type;
		union
		{
			eKey Key;
			Vector Pos;
		};
	};

	typedef Queue<InputEvent> InputQueue;
}