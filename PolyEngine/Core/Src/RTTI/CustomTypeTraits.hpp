#pragma once

#include "Defines.hpp"
#include "BaseObject.hpp"

#include "Collections/Dynarray.hpp"

namespace Poly
{
	namespace Trait
	{
		// Is dynarray
		template <typename> struct IsDynarray : public std::false_type {};
		template <typename T> struct IsDynarray<Dynarray<T>> : public std::true_type {};

		template <typename> struct DynarrayValueType {};
		template <typename T> struct DynarrayValueType<Dynarray<T>> { using type = T; };
	}
}