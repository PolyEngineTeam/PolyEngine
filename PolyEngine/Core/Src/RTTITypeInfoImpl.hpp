#pragma once

#include <type_traits>

// Define this to detect not registered types
template <typename T>
struct MetaTypeInfo {
	static Poly::RTTI::TypeInfo GetTypeInfo() {
		ASSERTE(false, "Type not declared in rtti system!");
		return Poly::RTTI::TypeInfo();
	}
};

template<typename T> constexpr Poly::RTTI::TypeInfo GetCheckedTypeInfo(typename std::enable_if<std::is_fundamental<T>::value>::type*) { return MetaTypeInfo<T>::GetTypeInfo(); }
template<typename T> constexpr Poly::RTTI::TypeInfo GetCheckedTypeInfo(typename std::enable_if<!std::is_fundamental<T>::value>::type*) { return T::MetaTypeInfo::GetTypeInfo(); }

namespace Poly {
	namespace RTTI {
		namespace Impl {

			template <class T> using RawType = std::remove_pointer<typename std::decay<T>::type>;

			//--------------------------------------------------------------------------
			template<typename T>
			class HasGetTypeInfoFunc {
				template <typename>
				constexpr static auto evaluate(int) -> decltype(std::declval<T>()->GetTypeInfo(), bool{}) { return true; }

				template <typename>
				constexpr static auto evaluate(...) -> decltype(bool{}) { return false; }

			public:
				static const bool value = evaluate<typename RawType<T>::type>(0);
			};

			//--------------------------------------------------------------------------
			template<typename T, bool = std::is_same<T, typename RawType<T>::type >::value>
			struct RawTypeInfo {
				static inline TypeInfo Get() { return TypeInfo(); }
			};

			//--------------------------------------------------------------------------
			template<typename T>
			struct RawTypeInfo<T, false> {
				static inline TypeInfo Get() { return GetCheckedTypeInfo<typename RawType<T>::type>(); }
			};

		} // namespace Impl
	} // namespace RTTI
} // namespace Poly

// Macro for declaring new types
#define RTTI_DECLARE_PRIMITIVE_TYPE(T)              \
template <>                               \
struct MetaTypeInfo< T > {                \
static Poly::RTTI::TypeInfo GetTypeInfo() {   \
return Poly::RTTI::Impl::TypeManager::Get().RegisterOrGetType(#T, Poly::RTTI::Impl::RawTypeInfo<T>::Get(), Poly::RTTI::BaseClasses<T>::Retrieve());   \
}                                       \
};

template <typename T>
struct AutoRegisterType {
	AutoRegisterType() {
		ASSERTE(false, "Type not defined in rtti system!");
	}
};

#define RTTI_CAT_IMPL(a,b) a##b
#define RTTI_CAT(a,b) RTTI_CAT_IMPL(a,b)

// Macro for defining new types
#define RTTI_DEFINE_TYPE(T)                       \
template <> struct AutoRegisterType<T> { AutoRegisterType() { GetCheckedTypeInfo<T>(); } }; \
static const AutoRegisterType<T> RTTI_CAT(autoRegisterType, __COUNTER__);

#define RTTI_GENERATE_TYPE_INFO(T)\
	struct MetaTypeInfo {	\
	static Poly::RTTI::TypeInfo GetTypeInfo() {   \
		return Poly::RTTI::Impl::TypeManager::Get().RegisterOrGetType(#T, Poly::RTTI::Impl::RawTypeInfo<T>::Get(), Poly::RTTI::BaseClasses<T>::Retrieve());   \
	}  \
	};
