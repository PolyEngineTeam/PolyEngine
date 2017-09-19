#pragma once

#include <type_traits>

/*template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> { typedef T type; };
*/
namespace Poly {
	namespace RTTI {
		namespace Impl {

			template <class T>
			struct RawType { typedef T type; };

			template <class T> struct RawType<const T> { typedef typename RawType<T>::type type; };

			template <class T> struct RawType<T*> { typedef typename RawType<T>::type type; };
			template <class T> struct RawType<T* const> { typedef typename RawType<T>::type type; };
			template <class T> struct RawType<T* volatile> { typedef typename RawType<T>::type type; };

			template <class T> struct RawType<T&> { typedef typename RawType<T>::type type; };

			//--------------------------------------------------------------------------
			template<typename T>
			class HasGetTypeInfoFunc_impl {
				typedef char YesType[1];
				typedef char NoType[2];

				template <typename U, RTTI::TypeInfo(U::*)() const>
				class check {};

				template <typename C>
				static YesType& f(check<C, &C::GetTypeInfo>*);

				template <typename C>
				static NoType& f(...);

			public:
				static const bool value = (sizeof(f<typename RawType<T>::type>(0)) == sizeof(YesType));
			};

			//--------------------------------------------------------------------------
			template<class T, class = void>
			struct HasGetTypeInfoFunc : std::integral_constant<bool, false> {};

			//--------------------------------------------------------------------------
			template<class T>
			struct HasGetTypeInfoFunc<T, typename std::enable_if<HasGetTypeInfoFunc_impl<T>::value>::type > : std::integral_constant<bool, true> {};

			//--------------------------------------------------------------------------
			template<typename T, bool = std::is_same<T, typename RawType<T>::type >::value>
			struct RawTypeInfo {
				static inline TypeInfo Get() { return TypeInfo(); }
			};

			//--------------------------------------------------------------------------
			template<typename T>
			struct RawTypeInfo<T, false> {
				static inline TypeInfo Get() { return MetaTypeInfo<typename RawType<T>::type>::GetTypeInfo(); }
			};

		} // namespace Impl
	} // namespace RTTI
} // namespace Poly

  // Define this to detect not registered types
template <typename T>
struct MetaTypeInfo {
	static Poly::RTTI::TypeInfo GetTypeInfo() {
		return T::TYPE_NOT_DECLARED_IN_RTTI_SYSTEM();
	}
};

// Macro for declaring new types
#define RTTI_DECLARE_TYPE(T)              \
template <>                               \
struct MetaTypeInfo< T > {                \
static Poly::RTTI::TypeInfo GetTypeInfo() {   \
return Poly::RTTI::Impl::TypeManager::Get().RegisterOrGetType(#T, Poly::RTTI::Impl::RawTypeInfo<T>::Get(), Poly::RTTI::BaseClasses<T>::Retrive());   \
}                                       \
};

template <typename T>
struct AutoRegisterType {
	AutoRegisterType() {
		T::TYPE_NOT_DEFINED_IN_RTTI_SYSTEM();
	}
};

#define RTTI_CAT_IMPL(a,b) a##b
#define RTTI_CAT(a,b) RTTI_CAT_IMPL(a,b)

// Macro for defining new types
#define RTTI_DEFINE_TYPE(T)                       \
template <>                                       \
struct AutoRegisterType< T > {                    \
AutoRegisterType() {                            \
MetaTypeInfo<T>::GetTypeInfo();               \
}                                               \
};                                                \
static const AutoRegisterType<T> RTTI_CAT(autoRegisterType, __COUNTER__);
