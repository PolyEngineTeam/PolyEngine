#pragma once

#include "Defines.hpp"
#include "BaseObject.hpp"

namespace Poly
{
	template<typename T> class Dynarray;
	template<typename K, typename V, size_t Bfactor> class OrderedMap;
	template<typename T, typename E> class EnumArray;
	template<typename E> class EnumFlags;
	template<typename T> class IterablePoolAllocator;

	namespace Trait
	{
		// Is dynarray
		template <typename> struct IsDynarray : public std::false_type {};
		template <typename T> struct IsDynarray<Dynarray<T>> : public std::true_type {};

		template <typename> struct DynarrayValueType {};
		template <typename T> struct DynarrayValueType<Dynarray<T>> { using type = T; };

		// Is Ordered map
		template <typename> struct IsOrderedMap : public std::false_type {};
		template <typename K, typename V, size_t Bfactor> struct IsOrderedMap<OrderedMap<K,V, Bfactor>> : public std::true_type {};

		template <typename> struct OrderedMapType {};
		template <typename K, typename V, size_t Bfactor> struct OrderedMapType<OrderedMap<K, V, Bfactor>> { using keyType = K; using valueType = V; static const size_t bFactor = Bfactor; };

		// Is enum array
		template <typename> struct IsEnumArray : public std::false_type {};
		template <typename T, typename E> struct IsEnumArray<EnumArray<T, E>> : public std::true_type {};

		template <typename> struct EnumArrayType {};
		template <typename T, typename E> struct EnumArrayType<EnumArray<T, E>> { using enumType = E; using valueType = T; };

		// Is enum flags
		template <typename> struct IsEnumFlags : public std::false_type {};
		template <typename E> struct IsEnumFlags<EnumFlags<E>> : public std::true_type {};

		template <typename> struct EnumFlagsType {};
		template <typename E> struct EnumFlagsType<EnumFlags<E>> { using type = E; };

		// Is iterable pool allocator
		template <typename> struct IsIterablePoolAllocator : public std::false_type {};
		template <typename E> struct IsIterablePoolAllocator<IterablePoolAllocator<E>> : public std::true_type {};

		template <typename> struct IterablePoolAllocatorType {};
		template <typename E> struct IterablePoolAllocatorType<IterablePoolAllocator<E>> { using type = E; };

		

		template <class T> using RawType = std::remove_pointer<typename std::decay<typename std::remove_cv<T>::type >::type>;
	}
}