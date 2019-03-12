#pragma once

#include <Defines.hpp>
#include <Math/Vector2i.hpp>
#include <Collections/Queue.hpp>

namespace Poly
{
	enum class eOutputEventType
	{
		MOUSEPOS,
		CURSORSET,
		CURSORSHOW,
		CURSORHIDE,
		_COUNT
	};

	enum class eMouseCursorType
	{
		ARROW,
		TEXTINPUT,
		RESIZEALL,
		RESIZENS,
		RESIZEEW,
		RESIZENESW,
		RESIZENWSE,
		HAND,
		_COUNT
	};

	struct OutputEvent : public BaseObject<>
	{
		OutputEvent() = default;
		OutputEvent(eOutputEventType type) : Type(type) {}
		OutputEvent(eOutputEventType type, const Vector2i& pos) : Type(type), Pos(pos) {}
		OutputEvent(eOutputEventType type, eMouseCursorType cursorType) : Type(type), CursorType(cursorType) {}

		eOutputEventType Type = eOutputEventType::_COUNT;
		Vector2i Pos;
		eMouseCursorType CursorType = eMouseCursorType::_COUNT;
    };

	using OutputQueue = Queue<OutputEvent>;
}
