#pragma once

#include "Defines.hpp"
#include "BaseObject.hpp"

#include "Collections/Dynarray.hpp"
#include "Collections/OrderedMap.hpp"

namespace Poly
{
	namespace Trait
	{
		// Is dynarray
		template <typename> struct IsDynarray : public std::false_type {};
		template <typename T> struct IsDynarray<Dynarray<T>> : public std::true_type {};

		template <typename> struct DynarrayValueType {};
		template <typename T> struct DynarrayValueType<Dynarray<T>> { using type = T; };

		// Is Ordered map
		template <typename> struct IsOrderedMap : public std::false_type {};
		template <typename K, typename V> struct IsOrderedMap<OrderedMap<K,V>> : public std::true_type {};

		template <typename> struct OrderedMapType {};
		template <typename K, typename V> struct OrderedMapType<OrderedMap<K, V>> { using keyType = K; using valueType = V; };

		template <class T> using RawType = std::remove_pointer<typename std::decay<typename std::remove_cv<T>::type >::type>;
	}
}