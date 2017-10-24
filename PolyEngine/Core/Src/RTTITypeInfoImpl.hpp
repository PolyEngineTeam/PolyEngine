#pragma once

#include <type_traits>

// Define this to detect not registered types
template <typename T>
struct MetaTypeInfo {
	static Poly::RTTI::TypeInfo GetTypeInfo() {
#if defined(_WIN32)
		STATIC_ASSERTE(false, "Type not declared in rtti system!");
#else
		ASSERTE(false, "Type not declared in rtti system!");
#endif
		return Poly::RTTI::TypeInfo();
	}
};

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
		} // namespace Impl

		template<typename T, typename std::enable_if<std::is_fundamental<T>::value || std::is_enum<T>::value>::type*>
		constexpr TypeInfo GetUnifiedTypeInfo() { return MetaTypeInfo<T>::GetTypeInfo(); }
		template<typename T, typename std::enable_if<!std::is_fundamental<T>::value && !std::is_enum<T>::value>::type*>
		constexpr TypeInfo GetUnifiedTypeInfo() { return T::MetaTypeInfo::GetTypeInfo(); }
		template <typename T> 
		constexpr TypeInfo GetUnifiedTypeInfoFromInstance(const T*) { return GetUnifiedTypeInfo<T>(); }

	} // namespace RTTI
} // namespace Poly

// Macro for declaring new types
#define RTTI_DECLARE_PRIMITIVE_TYPE(T)              \
	template <>                               \
	struct MetaTypeInfo< T > \
	{                \
		static Poly::RTTI::TypeInfo GetTypeInfo() \
		{   \
			return Poly::RTTI::Impl::TypeManager::Get().RegisterOrGetType(#T, Poly::RTTI::BaseClasses<T>::Retrieve());   \
		}                                       \
	};

template <typename T>
struct AutoRegisterType {
	AutoRegisterType() {
#if defined(_WIN32)
		STATIC_ASSERTE(false, "Type not defined in rtti system!");
#else
		ASSERTE(false, "Type not defined in rtti system!");
#endif
	}
};

#define RTTI_CAT_IMPL(a,b) a##b
#define RTTI_CAT(a,b) RTTI_CAT_IMPL(a,b)

// Macro for defining new types
#define RTTI_DEFINE_PRIMITIVE_TYPE(T)                       \
	template <> struct AutoRegisterType<T> { AutoRegisterType() { Poly::RTTI::GetUnifiedTypeInfo<T>(); } }; \
	static const AutoRegisterType<T> RTTI_CAT(autoRegisterType, __COUNTER__);

#define RTTI_DEFINE_TYPE(T)                       \
	template <> struct AutoRegisterType<T> { AutoRegisterType() { Poly::RTTI::GetUnifiedTypeInfo<T>(); } }; \
	static const AutoRegisterType<T> RTTI_CAT(autoRegisterType, __COUNTER__); \
	RTTI_PROPERTY_MANAGER_IMPL(T)


#define RTTI_GENERATE_TYPE_INFO(T)\
	struct MetaTypeInfo { \
		static Poly::RTTI::TypeInfo GetTypeInfo() \
		{ \
			return Poly::RTTI::Impl::TypeManager::Get().RegisterOrGetType(#T, Poly::RTTI::BaseClasses<T>::Retrieve()); \
		} \
	};
