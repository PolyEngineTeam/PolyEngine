#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/Vector2i.hpp>
#include <pe/core/storage/Queue.hpp>

namespace pe::api::input
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
		OutputEvent(eOutputEventType type) : type(type) {}
		OutputEvent(eOutputEventType type, const ::pe::core::math::Vector2i& pos) : type(type), pos(pos) {}
		OutputEvent(eOutputEventType type, eMouseCursorType cursorType) : type(type), cursorType(cursorType) {}

		eOutputEventType type = eOutputEventType::_COUNT;
		::pe::core::math::Vector2i pos;
		eMouseCursorType cursorType = eMouseCursorType::_COUNT;
    };

	using OutputQueue = ::pe::core::storage::Queue<OutputEvent>;
}
