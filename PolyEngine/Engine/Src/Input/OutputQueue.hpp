#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/Vector2i.hpp>
#include <pe/core/storage/Queue.hpp>

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

	struct OutputEvent : public ::pe::core::BaseObject<>
	{
		OutputEvent() = default;
		OutputEvent(eOutputEventType type) : Type(type) {}
		OutputEvent(eOutputEventType type, const ::pe::core::math::Vector2i& pos) : Type(type), Pos(pos) {}
		OutputEvent(eOutputEventType type, eMouseCursorType cursorType) : Type(type), CursorType(cursorType) {}

		eOutputEventType Type = eOutputEventType::_COUNT;
		::pe::core::math::Vector2i Pos;
		eMouseCursorType CursorType = eMouseCursorType::_COUNT;
    };

	using OutputQueue = ::pe::core::storage::Queue<OutputEvent>;
}
