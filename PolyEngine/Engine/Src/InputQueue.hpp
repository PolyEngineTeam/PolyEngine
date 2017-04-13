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

	struct Event
	{	
		Event() {}
		Event(const Event &src) : Type(src.Type), Pos(src.Pos) {}
		Event(eEventType type, eKey key) : Type(type), Key(key) {}
		Event(eEventType type, Vector pos) : Type(type), Pos(pos) {}
		~Event() {}

		Event& operator=(const Event& src) { Type = src.Type; Pos = src.Pos; return *this; }

		eEventType Type;
		union
		{
			eKey Key;
			Vector Pos;
		};
	};

	typedef Queue<Event> InputQueue;
}