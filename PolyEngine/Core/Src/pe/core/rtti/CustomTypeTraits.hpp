#pragma once

#include <pe/Defines.hpp>
#include <pe/core/BaseObject.hpp>

namespace Poly
{
	template<typename K, typename V, size_t Bfactor> class OrderedMap;
	template<typename T, typename E> class EnumArray;
	template<typename E> class EnumFlags;
	template<typename T> class IterablePoolAllocator;

	namespace Trait
	{
		// Is std::vector
		template <typename> struct IsStdVector : public std::false_type {};
		template <typename T> struct IsStdVector<std::vector<T>> : public std::true_type {};

		template <typename> struct StdVectorValueType {};
		template <typename T> struct StdVectorValueType<std::vector<T>> { using type = T; };

		// Is std::map
		template <typename> struct IsStdMap : public std::false_type {};
		template <typename K, typename V> struct IsStdMap<std::map<K,V>> : public std::true_type {};

		template <typename> struct StdMapType {};
		template <typename K, typename V> struct StdMapType<std::map<K, V>> { using keyType = K; using valueType = V; };

		// Is std::unordered_map
		template <typename> struct IsStdUnorderedMap : public std::false_type {};
		template <typename K, typename V> struct IsStdUnorderedMap<std::unordered_map<K,V>> : public std::true_type {};

		template <typename> struct StdUnorderedMapType {};
		template <typename K, typename V> struct StdUnorderedMapType<std::unordered_map<K, V>> { using keyType = K; using valueType = V; };

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

		// Is unique ptr
		template <typename> struct IsUniquePtr : public std::false_type {};
		template <typename T, typename D> struct IsUniquePtr<std::unique_ptr<T, D>> : public std::true_type {};

		template <typename> struct UniquePtrType {};
		template <typename T, typename D> struct UniquePtrType<std::unique_ptr<T, D>> { using type = T; using deleter = D; };

		template <class T> using RawType = std::remove_pointer<typename std::decay<typename std::remove_cv<T>::type >::type>;
	}
}